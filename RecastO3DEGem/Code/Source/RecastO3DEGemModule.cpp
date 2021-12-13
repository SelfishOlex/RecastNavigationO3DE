

#include <RecastO3DEGemModuleInterface.h>
#include <RecastO3DEGemSystemComponent.h>

namespace RecastO3DEGem
{
    class RecastO3DEGemModule
        : public RecastO3DEGemModuleInterface
    {
    public:
        AZ_RTTI(RecastO3DEGemModule, "{cc1b4ad5-5c05-4a9f-8943-b81431ff6637}", RecastO3DEGemModuleInterface);
        AZ_CLASS_ALLOCATOR(RecastO3DEGemModule, AZ::SystemAllocator, 0);
    };
}// namespace RecastO3DEGem

AZ_DECLARE_MODULE_CLASS(Gem_RecastO3DEGem, RecastO3DEGem::RecastO3DEGemModule)
