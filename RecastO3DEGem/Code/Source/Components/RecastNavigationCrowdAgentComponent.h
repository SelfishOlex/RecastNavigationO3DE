#pragma once

#include <AzCore/Component/Component.h>
#include <RecastO3DE/RecastNavigationCrowdAgentBus.h>

namespace RecastO3DE
{
    class RecastNavigationCrowdAgentComponent
        : public AZ::Component
        , public RecastNavigationCrowdAgentRequestBus::Handler
    {
    public:
        AZ_COMPONENT(RecastNavigationCrowdAgentComponent, "{E8A21D59-07C1-4BC1-819D-45384B561589}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component interface implementation
        void Activate() override;
        void Deactivate() override;

        // RecastNavigationCrowdAgentRequestBus
        void SetCrowdAgentSettings(CrowdAgentSettings newSettings) override;
        CrowdAgentSettings GetCrowdAgentSettings() override;
        int GetAgentIndex() override { return m_agentSettings.m_agentIndex; }
        AZ::EntityId GetCrowdEntity() override { return m_crowdEntityId; }
        void SetCrowdAgentIndex(int agentIndexInCrowd, AZ::EntityId crowdEntity) override;
        void SetCrowdAgentTargetEntity(AZ::EntityId targetEntity) override;
        void SetCrowdAgentTargetPosition(const AZ::Vector3& worldPosition) override;

    private:
        AZ::EntityId m_crowdEntityId;

        CrowdAgentSettings m_agentSettings;
    };

} // namespace RecastO3DE
