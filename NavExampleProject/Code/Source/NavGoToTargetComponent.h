
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Console/Console.h>

#include <NavExampleProject/NavExampleProjectBus.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

namespace NavExampleProject
{
    class NavGoToTargetComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
        , public RecastO3DE::RecastNavigationMeshNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(NavGoToTargetComponent, "{33B8DF6C-544B-4A97-93DD-69C53F9E3C09}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        NavGoToTargetComponent();

        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        // RecastNavigationMeshNotificationBus
        void OnNavigationMeshUpdated(dtNavMesh* updatedNavMesh, dtNavMeshQuery* updatedNavMeshQuery) override;

    private:
        AZ::EntityId m_goalEntityId;

        AZ::Event<const AZStd::vector<AZ::Vector3>&>::Handler m_pathHandler;
        AZ::Event<const AZ::Vector3&, const AZ::Vector3&>::Handler m_nextPointHandler;

        void OnPathFoundEvent(const AZStd::vector<AZ::Vector3>& pathPoints);
        void OnNextPathPoint(const AZ::Vector3& next, const AZ::Vector3& afterNext);

        AZ::Vector3 m_nextTarget = AZ::Vector3::CreateZero();
        float m_speed = 1.f;

        AZStd::vector<AZ::Vector3> m_path;
    };
}
