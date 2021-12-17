#pragma once
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/TransformBus.h>
#include <RecastO3DE/RecastNavigationAgentBus.h>

namespace RecastO3DE
{
    class RecastNavigationAgentComponent
        : public AZ::Component
        , public RecastNavigationAgentRequestBus::Handler
        , public AZ::TransformNotificationBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(RecastNavigationAgentComponent, "{6BAF2338-85D9-4F1C-AD7E-4DCAFEC8AF08}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component interface implementation
        void Activate() override;
        void Deactivate() override;

        // RecastNavigationAgentRequestBus
        void PathToEntity(AZ::EntityId targetEntity) override;
        void PathToPosition(const AZ::Vector3& targetWorldPosition) override;
        void CancelPath() override;

        // TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        // TransformNotificationBus
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        void SetPathFoundEvent(AZ::Event<const AZStd::vector<AZ::Vector3>&>::Handler handler);
        void SetNextTraversalPointEvent(AZ::Event<const AZ::Vector3&, const AZ::Vector3&>::Handler handler);

    private:
        AZ::EntityId m_navigationMeshEntityId;

        AZStd::vector<AZ::Vector3> m_pathPoints;
        size_t m_currentPathIndex = 0;

        float m_distanceForArrivingToPoint = 1.f;

        AZ::Event<const AZStd::vector<AZ::Vector3>&> m_pathFoundEvent;
        AZ::Event<const AZ::Vector3&, const AZ::Vector3&> m_nextTraversalPointEvent;
    };

} // namespace RecastO3DE
