
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <RecastO3DEGemSystemComponent.h>
#include <Components/RecastNavigationAgentComponent.h>
#include <Components/RecastNavigationCrowdAgentComponent.h>
#include <Components/RecastNavigationCrowdComponent.h>
#include <Components/RecastNavigationMeshComponent.h>
#include <Components/RecastWalkableComponent.h>

namespace RecastO3DE
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
                RecastO3DEGemSystemComponent::CreateDescriptor(),
                RecastNavigationAgentComponent::CreateDescriptor(),
                RecastNavigationMeshComponent::CreateDescriptor(),
                RecastWalkableComponent::CreateDescriptor(),
                RecastNavigationCrowdAgentComponent::CreateDescriptor(),
                RecastNavigationCrowdComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<RecastO3DE::RecastO3DEGemSystemComponent>(),
                azrtti_typeid<RecastO3DEGemSystemComponent>(),
            };
        }
    };
}// namespace RecastO3DE
