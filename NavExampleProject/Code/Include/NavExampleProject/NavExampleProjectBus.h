
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace NavExampleProject
{
    class NavExampleProjectRequests
    {
    public:
        AZ_RTTI(NavExampleProjectRequests, "{140f829f-7042-468b-a6af-9731c3f55e11}");
        virtual ~NavExampleProjectRequests() = default;
        // Put your public methods here
    };
    
    class NavExampleProjectBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using NavExampleProjectRequestBus = AZ::EBus<NavExampleProjectRequests, NavExampleProjectBusTraits>;
    using NavExampleProjectInterface = AZ::Interface<NavExampleProjectRequests>;

} // namespace NavExampleProject
