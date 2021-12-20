
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Console/Console.h>

#include <NavExampleProject/NavExampleProjectBus.h>
#include <RecastO3DE/RecastNavigationCrowdAgentBus.h>

namespace NavExampleProject
{
    class NavAgentTransportComponent
        : public AZ::Component
        , public RecastO3DE::RecastNavigationCrowdAgentNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(NavAgentTransportComponent, "{BB10308B-D203-49B2-8357-4BACFCF3C263}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // RecastNavigationCrowdAgentNotificationBus
        void OnCrowdAgentPosition(const AZ::Vector3& newPosition) override;
    };
}
