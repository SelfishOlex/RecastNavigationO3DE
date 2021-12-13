
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace RecastO3DEGem
{
    class RecastO3DEGemRequests
    {
    public:
        AZ_RTTI(RecastO3DEGemRequests, "{82e5cfb5-6a1a-4bd1-b48d-cd817474d611}");
        virtual ~RecastO3DEGemRequests() = default;
        // Put your public methods here
    };
    
    class RecastO3DEGemBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using RecastO3DEGemRequestBus = AZ::EBus<RecastO3DEGemRequests, RecastO3DEGemBusTraits>;
    using RecastO3DEGemInterface = AZ::Interface<RecastO3DEGemRequests>;

} // namespace RecastO3DEGem
