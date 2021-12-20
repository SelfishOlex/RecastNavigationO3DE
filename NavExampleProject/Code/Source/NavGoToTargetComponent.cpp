
#include <AzCore/Serialization/EditContext.h>

#include "NavGoToTargetComponent.h"

#include <AzCore/Component/Entity.h>
#include <AzCore/Math/ToString.h>

#include <Components/RecastNavigationAgentComponent.h>

#pragma optimize("", off)

namespace NavExampleProject
{
    void NavGoToTargetComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<NavGoToTargetComponent, AZ::Component>()
                ->Version(3)
                ->Field("Goal Entity", &NavGoToTargetComponent::m_goalEntityId)
                ->Field("Speed", &NavGoToTargetComponent::m_speed)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<NavGoToTargetComponent>("Nav Go To Target", "[Moves the entity towards the goal entity]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(nullptr, &NavGoToTargetComponent::m_goalEntityId, "Goal Entity", "Move to this entity")
                    ->DataElement(nullptr, &NavGoToTargetComponent::m_speed, "Speed", "maximum speed of the entity along the path")
                    ;
            }
        }
    }

    void NavGoToTargetComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("NavGoToTargetComponent"));
    }

    void NavGoToTargetComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("NavGoToTargetComponent"));
    }

    void NavGoToTargetComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("RecastNavigationAgentComponent"));
    }

    void NavGoToTargetComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
    void NavGoToTargetComponent::Init()
    {
    }

    void NavGoToTargetComponent::Activate()
    {
        AZ::TickBus::Handler::BusConnect();
        RecastO3DE::RecastNavigationMeshNotificationBus::Handler::BusConnect(GetEntityId());
        
        AZ::TransformBus::EventResult(m_nextTarget, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
    }

    void NavGoToTargetComponent::Deactivate()
    {
        RecastO3DE::RecastNavigationMeshNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
    }

    void NavGoToTargetComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        AZ::Vector3 current = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(current, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);

        if (current.GetDistance(m_nextTarget) > .1f)
        {
            AZ::Vector3 newPosition = current.Lerp(m_nextTarget, m_speed/current.GetDistance(m_nextTarget) * deltaTime);
            
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, newPosition);
        }
        else
        {
            if (m_path.empty() == false)
            {
                m_path.erase(m_path.begin());
                if (m_path.empty() == false)
                    m_nextTarget = m_path.front();
            }
        }
    }

    void NavGoToTargetComponent::OnNavigationMeshUpdated(
        [[maybe_unused]] dtNavMesh* updatedNavMesh,
        [[maybe_unused]] dtNavMeshQuery* updatedNavMeshQuery)
    {
        if (auto* agent = GetEntity()->FindComponent<RecastO3DE::RecastNavigationAgentComponent>())
        {
            agent->SetPathFoundEvent(m_pathHandler);
            agent->SetNextTraversalPointEvent(m_nextPointHandler);

            m_path = agent->PathToEntity(m_goalEntityId);
            if (m_path.empty() == false)
            {
                m_nextTarget = m_path.front();
            }
        }
    }

    void NavGoToTargetComponent::OnPathFoundEvent(const AZStd::vector<AZ::Vector3>& pathPoints)
    {
        m_path = pathPoints;
        if (m_path.empty() == false)
        {
            m_nextTarget = m_path.front();
        }

        AZ_Printf("NavExample", "path received %d", pathPoints.size());
        for (auto& point : pathPoints)
        {
            AZ_Printf("NavExample", "path point %s", AZ::ToString(point).c_str());
        }
    }

    void NavGoToTargetComponent::OnNextPathPoint([[maybe_unused]] const AZ::Vector3& next, 
        [[maybe_unused]] const AZ::Vector3& afterNext)
    {
        /*if (afterNext.IsZero() == false)
        {
            m_nextTarget= afterNext;
        }*/
    }

    NavGoToTargetComponent::NavGoToTargetComponent()
        : m_pathHandler([this](const AZStd::vector<AZ::Vector3>& pathPoints)
            {
                OnPathFoundEvent(pathPoints);
            }),
        m_nextPointHandler([this](const AZ::Vector3& next, const AZ::Vector3& afterNext)
            {
                OnNextPathPoint(next, afterNext);
            })
    {
    }
}
