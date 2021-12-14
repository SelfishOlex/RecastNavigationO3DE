#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector3.h>
#include <Components/RecastSmartPointer.h>
#include <RecastO3DE/RecastNavigationCrowdBus.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

namespace RecastO3DE
{
    class RecastNavigationCrowdComponent
        : public AZ::Component
        , public RecastNavigationMeshNotificationBus::Handler
        , public RecastNavigationCrowdRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT( RecastNavigationCrowdComponent, "{FC54FFED-D8CD-4F36-8867-5C9D981B01B8}" );

        static void Reflect( AZ::ReflectContext* context );

        static void GetProvidedServices( AZ::ComponentDescriptor::DependencyArrayType& provided );
        static void GetIncompatibleServices( AZ::ComponentDescriptor::DependencyArrayType& incompatible );
        static void GetRequiredServices( AZ::ComponentDescriptor::DependencyArrayType& required );
        static void GetDependentServices( AZ::ComponentDescriptor::DependencyArrayType& dependent );

        // AZ::Component interface implementation
        void Activate() override;
        void Deactivate() override;

        // RecastNavigationMeshNotificationBus
        void OnNavigationMeshUpdated( dtNavMesh* updatedMeshQuery, dtNavMeshQuery* updatedNavMeshQuery ) override;

        // RecastNavigationCrowdRequestBus
        void AddAgent( AZ::EntityId crowdAgent ) override;
        void RemoveAgent( AZ::EntityId crowdAgent ) override;
        void SetNavigationMeshEntity( AZ::EntityId targetEntity ) override;
        void SetTargetEntityForAgent( AZ::EntityId targetEntity, int agentIndex ) override;
        void SetTargetPositionForAgent( const AZ::Vector3& targetPosition, int agentIndex ) override;

        // TickBus
        void OnTick( float deltaTime, AZ::ScriptTimePoint time ) override;

    private:
        AZStd::mutex m_updateLock;
        enum class UpdateState
        {
            NotRunning,
            Running,
            Ready
        };
        UpdateState m_updateState =UpdateState::NotRunning;

        class CommonAgentParams
        {
        public:
            AZ_TYPE_INFO(CommonAgentParams, "{0CE05BAA-5CCE-4F1D-B228-813BFE178667}");

            static void Reflect( AZ::ReflectContext* context );

            float m_collisionQueryRangeFactor = 30.F;
            float m_pathOptimizationRangeFactor = 30.F;
            float m_separationWeight = 10.F;

            /*
                enum UpdateFlags
                {
	                DT_CROWD_ANTICIPATE_TURNS = 1,
	                DT_CROWD_OBSTACLE_AVOIDANCE = 2,
	                DT_CROWD_SEPARATION = 4,
	                DT_CROWD_OPTIMIZE_VIS = 8,			///< Use #dtPathCorridor::optimizePathVisibility() to optimize the agent path.
	                DT_CROWD_OPTIMIZE_TOPO = 16,		///< Use dtPathCorridor::optimizePathTopology() to optimize the agent path.
                };
             */
            int m_updateFlags = 0;
        };

        CommonAgentParams m_commonParams;

        bool m_runOnThread = true;
        float m_timeSinceLastCrowdUpdate = 0.F;

        AZ::EntityId m_navigationMeshEntityId;

        AZ::Vector3 m_targetForCrowd;

        AZ::EntityId m_testTargetEntity;

        int m_maxAgents = 25;
        float m_maxAgentRadius = 10.F;

        AZStd::vector<AZ::EntityId> m_agentEntities;

        struct AgentEntityAndIndex
        {
            AZ::EntityId m_agentEntity;
            int m_agentIndex = -1;
            AZ::Vector3 m_lastPosition;
        };

        AZStd::vector<AgentEntityAndIndex> m_activeAgents;

        RecastPointer<dtCrowd> m_crowd;

        dtNavMeshQuery* m_updatedNavMeshQuery = nullptr; // not owning

        void CreateCrowd( dtNavMesh* updatedNavMesh );
        void AddAgentInternal( AZ::EntityId crowdAgent );
    };
} // namespace RecastO3DE
