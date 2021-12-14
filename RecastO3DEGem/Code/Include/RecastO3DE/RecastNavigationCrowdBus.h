
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>

namespace RecastO3DE
{
    class RecastNavigationCrowdRequests
        : public AZ::ComponentBus
    {
    public:

        virtual void AddAgent( AZ::EntityId crowdAgent ) = 0;
        virtual void RemoveAgent( AZ::EntityId crowdAgent ) = 0;
        virtual void SetNavigationMeshEntity( AZ::EntityId targetEntity ) = 0;

        virtual void SetTargetPositionForAgent( const AZ::Vector3& targetPosition, int agentIndex ) = 0;
        virtual void SetTargetEntityForAgent( AZ::EntityId targetEntity, int agentIndex ) = 0;
    };

    using RecastNavigationCrowdRequestBus = AZ::EBus<RecastNavigationCrowdRequests>;

    class RecastNavigationCrowdNotifications
        : public AZ::ComponentBus
    {
    public:
    };

    using RecastNavigationCrowdNotificationBus = AZ::EBus<RecastNavigationCrowdNotifications>;

} // namespace RecastO3DE
