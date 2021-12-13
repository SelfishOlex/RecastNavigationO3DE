
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <RecastO3DEGemSystemComponent.h>

namespace RecastO3DEGem
{
    class RecastO3DEGemModuleInterface
        : public AZ::Module
    {
    public:
        AZ_RTTI(RecastO3DEGemModuleInterface, "{73a53c7e-b995-4e14-b24b-116f09bf5800}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RecastO3DEGemModuleInterface, AZ::SystemAllocator, 0);

        RecastO3DEGemModuleInterface()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                RecastO3DEGemSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<RecastO3DEGemSystemComponent>(),
            };
        }
    };
}// namespace RecastO3DEGem
