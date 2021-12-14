
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>

namespace RecastO3DE
{
    class RecastNavigationAgentRequests
        : public AZ::ComponentBus
    {
    public:

        virtual void PathToEntity( AZ::EntityId targetEntity ) = 0;
        virtual void PathToPosition( const AZ::Vector3& targetWorldPosition ) = 0;
        virtual void CancelPath() = 0;
    };

    using RecastNavigationAgentRequestBus = AZ::EBus<RecastNavigationAgentRequests>;

    class RecastNavigationAgentNotifications
        : public AZ::ComponentBus
    {
    public:

        virtual void OnPathFound( [[maybe_unused]] const AZStd::vector<AZ::Vector3>& path ) {}
        virtual void OnPathFailed() {}

        virtual void OnNextTraversalPoint( [[maybe_unused]] const AZ::Vector3& nextPoint, [[maybe_unused]] const AZ::Vector3& afterNextPoint ) {}
        virtual void OnTraversalCompleted() {}
    };

    using RecastNavigationAgentNotificationBus = AZ::EBus<RecastNavigationAgentNotifications>;

} // namespace RecastO3DE
