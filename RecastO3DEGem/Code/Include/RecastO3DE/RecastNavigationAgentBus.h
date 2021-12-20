
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>

namespace RecastO3DE
{
    class RecastNavigationAgentRequests
        : public AZ::ComponentBus
    {
    public:

        virtual AZStd::vector<AZ::Vector3> PathToEntity( AZ::EntityId targetEntity ) = 0;
        virtual AZStd::vector<AZ::Vector3> PathToPosition( const AZ::Vector3& targetWorldPosition ) = 0;
        virtual void CancelPath() = 0;
    };

    using RecastNavigationAgentRequestBus = AZ::EBus<RecastNavigationAgentRequests>;

} // namespace RecastO3DE
