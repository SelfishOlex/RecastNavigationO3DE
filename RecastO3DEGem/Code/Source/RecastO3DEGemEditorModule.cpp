
#include <RecastO3DEGemModuleInterface.h>
#include <RecastO3DEGemEditorSystemComponent.h>

namespace RecastO3DEGem
{
    class RecastO3DEGemEditorModule
        : public RecastO3DEGemModuleInterface
    {
    public:
        AZ_RTTI(RecastO3DEGemEditorModule, "{cc1b4ad5-5c05-4a9f-8943-b81431ff6637}", RecastO3DEGemModuleInterface);
        AZ_CLASS_ALLOCATOR(RecastO3DEGemEditorModule, AZ::SystemAllocator, 0);

        RecastO3DEGemEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                RecastO3DEGemEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<RecastO3DEGemEditorSystemComponent>(),
            };
        }
    };
}// namespace RecastO3DEGem

AZ_DECLARE_MODULE_CLASS(Gem_RecastO3DEGem, RecastO3DEGem::RecastO3DEGemEditorModule)
