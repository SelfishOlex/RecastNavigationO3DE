
#include <AzCore/Serialization/EditContext.h>

#include "NavAgentTransportComponent.h"

#include <AzCore/Component/Entity.h>

#include <Components/RecastNavigationAgentComponent.h>

#pragma optimize("", off)

namespace NavExampleProject
{
    void NavAgentTransportComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<NavAgentTransportComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<NavAgentTransportComponent>("Nav Agent Transport", "[Moves the agent according to its crowd commands]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void NavAgentTransportComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("NavAgentTransportComponent"));
    }

    void NavAgentTransportComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("NavAgentTransportComponent"));
    }

    void NavAgentTransportComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("RecastNavigationCrowdAgentComponent"));
    }

    void NavAgentTransportComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
    void NavAgentTransportComponent::Init()
    {
    }

    void NavAgentTransportComponent::Activate()
    {
        RecastO3DE::RecastNavigationCrowdAgentNotificationBus::Handler::BusConnect(GetEntityId());
    }

    void NavAgentTransportComponent::Deactivate()
    {
        RecastO3DE::RecastNavigationCrowdAgentNotificationBus::Handler::BusDisconnect();
    }

    void NavAgentTransportComponent::OnCrowdAgentPosition(const AZ::Vector3& newPosition)
    {
        AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, newPosition);
    }
}
