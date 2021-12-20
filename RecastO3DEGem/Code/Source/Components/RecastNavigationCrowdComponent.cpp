#include "RecastNavigationCrowdComponent.h"


#include <AzCore/Component/TransformBus.h>
#include <AzCore/Jobs/JobFunction.h>
#include <AzCore/Math/ToString.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <Components/RecastHelpers.h>
#include <RecastO3DE/RecastNavigationCrowdAgentBus.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

#pragma optimize("", off)

namespace RecastO3DE
{
    void RecastNavigationCrowdComponent::CommonAgentParams::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<CommonAgentParams>()
                ->Version(1)
                ->Field("m_collisionQueryRangeFactor", &CommonAgentParams::m_collisionQueryRangeFactor)
                ->Field("m_pathOptimizationRangeFactor", &CommonAgentParams::m_pathOptimizationRangeFactor)
                ->Field("m_separationWeight", &CommonAgentParams::m_separationWeight)
                ->Field("m_updateFlags", &CommonAgentParams::m_updateFlags)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<CommonAgentParams>("Common Agent Params", "[Parameters of crowd agents]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(nullptr, &CommonAgentParams::m_collisionQueryRangeFactor, "m_collisionQueryRangeFactor", "")
                    ->DataElement(nullptr, &CommonAgentParams::m_pathOptimizationRangeFactor, "m_pathOptimizationRangeFactor", "")
                    ->DataElement(nullptr, &CommonAgentParams::m_separationWeight, "m_separationWeight", "")
                    ->DataElement(nullptr, &CommonAgentParams::m_updateFlags, "m_updateFlags",
                        "DT_CROWD_ANTICIPATE_TURNS = 1, DT_CROWD_OBSTACLE_AVOIDANCE = 2, DT_CROWD_SEPARATION = 4")
                    ;
            }
        }
    }

    void RecastNavigationCrowdComponent::Reflect(AZ::ReflectContext* context)
    {
        CommonAgentParams::Reflect(context);

        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RecastNavigationCrowdComponent, AZ::Component>()
                ->Version(4)
                ->Field("Navigation Mesh Entity", &RecastNavigationCrowdComponent::m_navigationMeshEntityId)
                ->Field("Target Position", &RecastNavigationCrowdComponent::m_targetForCrowd)
                ->Field("Max Agents", &RecastNavigationCrowdComponent::m_maxAgents)
                ->Field("Max Agent Radius", &RecastNavigationCrowdComponent::m_maxAgentRadius)
                ->Field("Agents", &RecastNavigationCrowdComponent::m_agentEntities)
                ->Field("Test Target Entity", &RecastNavigationCrowdComponent::m_testTargetEntity)
                ->Field("Run on a Thread", &RecastNavigationCrowdComponent::m_runOnThread)
                ->Field("Agent Parameters", &RecastNavigationCrowdComponent::m_commonParams)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<RecastNavigationCrowdComponent>("Recast Navigation Crowd", "[The controller of a crowd of agents]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_navigationMeshEntityId, "Navigation Mesh Entity", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_targetForCrowd, "Target Position", "")

                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_maxAgents, "Max Agents", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_maxAgentRadius, "Max Agent Radius", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_agentEntities, "Agents", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_testTargetEntity, "Test Target Entity", "")

                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_runOnThread, "Run on a Thread", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdComponent::m_commonParams, "Agent Parameters", "")
                    ;
            }
        }
    }

    void RecastNavigationCrowdComponent::GetProvidedServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
    }

    void RecastNavigationCrowdComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void RecastNavigationCrowdComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void RecastNavigationCrowdComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void RecastNavigationCrowdComponent::Activate()
    {
        if (m_navigationMeshEntityId.IsValid())
        {
            RecastNavigationMeshNotificationBus::Handler::BusConnect(m_navigationMeshEntityId);
        }

        RecastNavigationCrowdRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void RecastNavigationCrowdComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();

        // Block until the crowds stopped running
        while (m_updateState == UpdateState::Running)
        {
            AZ_Printf(__FUNCTION__, "waiting on dtCroud to finish update() call");
        }

        m_crowd = {};

        //RecastNavigationMeshNotificationBus::Handler::BusDisconnect();
        //RecastNavigationCrowdRequestBus::Handler::BusDisconnect();
    }

    void RecastNavigationCrowdComponent::OnNavigationMeshUpdated(dtNavMesh* updatedMeshQuery, dtNavMeshQuery* updatedNavMeshQuery)
    {
        m_updatedNavMeshQuery = updatedNavMeshQuery;

        CreateCrowd(updatedMeshQuery);
    }

    void RecastNavigationCrowdComponent::AddAgent(AZ::EntityId crowdAgent)
    {
        if (m_agentEntities.size() < m_maxAgents && crowdAgent.IsValid())
        {
            m_agentEntities.push_back(crowdAgent);

            if (m_updatedNavMeshQuery)
            {
                AddAgentInternal(crowdAgent);
            }
        }
    }

    void RecastNavigationCrowdComponent::RemoveAgent(AZ::EntityId crowdAgent)
    {
        if (crowdAgent.IsValid())
        {
            const auto agentIter = AZStd::find(m_agentEntities.begin(), m_agentEntities.end(), crowdAgent);
            if (agentIter != m_agentEntities.end())
            {
                m_agentEntities.erase(agentIter);
            }

            const auto agentDataIter = AZStd::find_if(m_activeAgents.begin(), m_activeAgents.end(),
                [crowdAgent](const AgentEntityAndIndex& item)
                {
                    return item.m_agentEntity == crowdAgent;
                });

            if (agentDataIter != m_activeAgents.end())
            {
                if (m_crowd)
                {
                    m_crowd->removeAgent(agentDataIter->m_agentIndex);
                }

                m_activeAgents.erase(agentDataIter);
            }
        }
    }

    void RecastNavigationCrowdComponent::SetNavigationMeshEntity(AZ::EntityId targetEntity)
    {
        m_navigationMeshEntityId = targetEntity;

        if (m_navigationMeshEntityId.IsValid())
        {
            RecastNavigationMeshNotificationBus::Handler::BusConnect(m_navigationMeshEntityId);
        }
        else
        {
            RecastNavigationMeshNotificationBus::Handler::BusDisconnect();
        }
    }

    void RecastNavigationCrowdComponent::SetTargetEntityForAgent(AZ::EntityId targetEntity, int agentIndex)
    {
        AZ::Vector3 worldPosition;
        AZ::TransformBus::EventResult(worldPosition, targetEntity, &AZ::TransformBus::Events::GetWorldTranslation);

        //AZ_Printf(__FUNCTION__, "agent %d is on target %llu", agentIndex, targetEntity);

        SetTargetPositionForAgent(worldPosition, agentIndex);
    }

    void RecastNavigationCrowdComponent::SetTargetPositionForAgent(const AZ::Vector3& targetPosition, int agentIndex)
    {
        m_targetForCrowd = targetPosition;

        if (m_crowd)
        {
            RecastVector3 targetInRecastSpace(targetPosition);
            const float* targetRecastPosition = targetInRecastSpace.data();

            const float halfExtents[3] = { 10.F, 10, 10 };

            dtPolyRef targetPoly = 0;
            RecastVector3 targetPoint;

            const dtQueryFilter filter;

            const dtStatus result = m_updatedNavMeshQuery->findNearestPoly(targetRecastPosition, halfExtents, &filter, &targetPoly, targetPoint.data());
            if (result == DT_SUCCESS && targetPoly != 0)
            {
                m_crowd->requestMoveTarget(agentIndex, targetPoly, targetPoint.data());

                //AZ_Printf( __FUNCTION__, "%s", AZ::ToString( targetPosition ).c_str() );
            }
        }
    }

    void RecastNavigationCrowdComponent::OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (m_crowd)
        {
            if (m_activeAgents.empty() == false)
            {
                if (m_runOnThread)
                {
                    {
                        AZStd::lock_guard<AZStd::mutex> lock(m_updateLock);
                        if (m_updateState == UpdateState::NotRunning)
                        {
                            m_updateState = UpdateState::Running;

                            auto* job = AZ::CreateJobFunction([this, deltaTime = m_timeSinceLastCrowdUpdate]()
                                {
                                    m_crowd->update(deltaTime, nullptr);

                                    //AZStd::lock_guard<AZStd::mutex> lock( m_updateLock );
                                    m_updateState = UpdateState::Ready;
                                }, true);

                            m_timeSinceLastCrowdUpdate = 0.F;

                            job->Start();
                        }
                        else
                        {
                            m_timeSinceLastCrowdUpdate += deltaTime;
                        }
                    }

                    {
                        AZStd::lock_guard<AZStd::mutex> lock(m_updateLock);
                        if (m_updateState == UpdateState::Ready)
                        {
                            for (AgentEntityAndIndex& agentData : m_activeAgents)
                            {
                                if (const dtCrowdAgent* agent = m_crowd->getAgent(agentData.m_agentIndex))
                                {
                                    if (agent->active)
                                    {
                                        RecastVector3 recastPosition(agent->npos);

                                        const AZ::Vector3 latestAgentPosition = recastPosition.AsVector3();
                                        if (latestAgentPosition.IsClose(agentData.m_lastPosition, 0.1F) == false)
                                        {
                                            RecastNavigationCrowdAgentNotificationBus::Event(agentData.m_agentEntity,
                                                &RecastNavigationCrowdAgentNotificationBus::Events::OnCrowdAgentPosition, recastPosition.AsVector3());

                                            agentData.m_lastPosition = latestAgentPosition;
                                        }
                                    }
                                }
                            }

                            m_updateState = UpdateState::NotRunning;
                        }
                    }
                }
                else
                {
                    // run on main thread option
                    m_crowd->update(deltaTime, nullptr);
                    
                    for (AgentEntityAndIndex& agentData : m_activeAgents)
                    {
                        if (const dtCrowdAgent* agent = m_crowd->getAgent(agentData.m_agentIndex))
                        {
                            if (agent->active)
                            {
                                RecastVector3 recastPosition(agent->npos);

                                const AZ::Vector3 latestAgentPosition = recastPosition.AsVector3();
                                if (latestAgentPosition.IsClose(agentData.m_lastPosition, 0.1F) == false)
                                {
                                    RecastNavigationCrowdAgentNotificationBus::Event(agentData.m_agentEntity,
                                        &RecastNavigationCrowdAgentNotificationBus::Events::OnCrowdAgentPosition, recastPosition.AsVector3());

                                    agentData.m_lastPosition = latestAgentPosition;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void RecastNavigationCrowdComponent::CreateCrowd(dtNavMesh* updatedNavMesh)
    {
        AZ_Printf(__FUNCTION__, "%llu", GetEntityId());

        m_activeAgents.clear();
        m_crowd.reset(dtAllocCrowd());

        if (m_crowd->init(m_maxAgents, m_maxAgentRadius, updatedNavMesh))
        {
            for (const AZ::EntityId crowdAgentEntity : m_agentEntities)
            {
                AddAgentInternal(crowdAgentEntity);
            }
        }
        else
        {
            m_crowd = {};
        }
    }

    void RecastNavigationCrowdComponent::AddAgentInternal(AZ::EntityId crowdAgent)
    {
        AZ::Vector3 worldPosition;
        AZ::TransformBus::EventResult(worldPosition, crowdAgent, &AZ::TransformBus::Events::GetWorldTranslation);

        CrowdAgentSettings agentSettings;
        RecastNavigationCrowdAgentRequestBus::EventResult(agentSettings, crowdAgent, &RecastNavigationCrowdAgentRequestBus::Events::GetCrowdAgentSettings);

        dtCrowdAgentParams params{
            agentSettings.m_radius,
            agentSettings.m_height,
            agentSettings.m_maxAcceleration,
            agentSettings.m_maxSpeed,
            agentSettings.m_radius * 30.0f,
            agentSettings.m_radius * 30.0f,
            1.0F,
            DT_CROWD_SEPARATION /*| DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OBSTACLE_AVOIDANCE*/,
            0,
            0,
            nullptr };

        const int agentIndex = m_crowd->addAgent(RecastVector3{ worldPosition }.data(), &params);

        RecastNavigationCrowdAgentRequestBus::Event(crowdAgent, &RecastNavigationCrowdAgentRequestBus::Events::SetCrowdAgentIndex, agentIndex, GetEntityId());

        m_activeAgents.push_back(AgentEntityAndIndex{ crowdAgent, agentIndex, worldPosition });

        if (agentSettings.m_targetEntity)
        {
            SetTargetEntityForAgent( *agentSettings.m_targetEntity, agentIndex );
            
            //AZ_Printf(__FUNCTION__, "agent %d is on target %llu", agentIndex, *agentSettings.m_targetEntity);
        }
        else if (agentSettings.m_targetPosition)
        {
            SetTargetPositionForAgent( *agentSettings.m_targetPosition, agentIndex );
        }
        else if (m_testTargetEntity.IsValid())
        {
            SetTargetEntityForAgent( m_testTargetEntity, agentIndex );
        }
    }
}
