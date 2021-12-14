#include "RecastNavigationCrowdAgentComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <RecastO3DE/RecastNavigationCrowdBus.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

namespace RecastO3DE
{
    void RecastNavigationCrowdAgentComponent::Reflect(AZ::ReflectContext* context)
    {
        CrowdAgentSettings::Reflect(context);

        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RecastNavigationCrowdAgentComponent, AZ::Component>()
                ->Version(3)
                ->Field("Crowd Entity", &RecastNavigationCrowdAgentComponent::m_crowdEntityId)
                ->Field("Agent Settings", &RecastNavigationCrowdAgentComponent::m_agentSettings)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<RecastNavigationCrowdAgentComponent>("Recast Navigation Crowd Agent", "[Queries Recast Navigation Mesh]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(nullptr, &RecastNavigationCrowdAgentComponent::m_crowdEntityId, "Crowd Entity", "")
                    ->DataElement(nullptr, &RecastNavigationCrowdAgentComponent::m_agentSettings, "Agent Settings", "")
                    ;
            }
        }
    }

    void RecastNavigationCrowdAgentComponent::GetProvidedServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
    }

    void RecastNavigationCrowdAgentComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void RecastNavigationCrowdAgentComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void RecastNavigationCrowdAgentComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void RecastNavigationCrowdAgentComponent::Activate()
    {
        RecastNavigationCrowdAgentRequestBus::Handler::BusConnect(GetEntityId());
    }

    void RecastNavigationCrowdAgentComponent::Deactivate()
    {
        RecastNavigationCrowdRequestBus::Event(m_crowdEntityId, &RecastNavigationCrowdRequestBus::Events::RemoveAgent, GetEntityId());

        RecastNavigationCrowdAgentRequestBus::Handler::BusDisconnect();
    }

    void RecastNavigationCrowdAgentComponent::SetCrowdAgentSettings(CrowdAgentSettings newSettings)
    {
        m_agentSettings = newSettings;
    }

    CrowdAgentSettings RecastNavigationCrowdAgentComponent::GetCrowdAgentSettings()
    {
        return m_agentSettings;
    }

    void RecastNavigationCrowdAgentComponent::SetCrowdAgentIndex(int agentIndexInCrowd, AZ::EntityId crowdEntity)
    {
        //AZ_Printf( __FUNCTION__, "%d %llu", agentIndexInCrowd, crowdEntity );

        m_agentSettings.m_agentIndex = agentIndexInCrowd;
        m_crowdEntityId = crowdEntity;
    }

    void RecastNavigationCrowdAgentComponent::SetCrowdAgentTargetEntity(AZ::EntityId targetEntity)
    {
        //AZ_Printf( __FUNCTION__, "%d %llu", m_agentSettings.m_agentIndex, targetEntity );

        m_agentSettings.m_targetEntity = targetEntity;
        m_agentSettings.m_targetPosition = {};

        RecastNavigationCrowdRequestBus::Event(m_crowdEntityId, &RecastNavigationCrowdRequestBus::Events::SetTargetEntityForAgent, targetEntity, m_agentSettings.m_agentIndex);
    }

    void RecastNavigationCrowdAgentComponent::SetCrowdAgentTargetPosition(const AZ::Vector3& worldPosition)
    {
        m_agentSettings.m_targetEntity = {};
        m_agentSettings.m_targetPosition = worldPosition;

        RecastNavigationCrowdRequestBus::Event(m_crowdEntityId, &RecastNavigationCrowdRequestBus::Events::SetTargetPositionForAgent, worldPosition, m_agentSettings.m_agentIndex);
    }
} // namespace RecastO3DE
