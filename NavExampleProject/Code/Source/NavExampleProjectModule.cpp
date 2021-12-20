
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "NavAgentTransportComponent.h"
#include "NavExampleProjectSystemComponent.h"
#include "NavGoToTargetComponent.h"

namespace NavExampleProject
{
    class NavExampleProjectModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(NavExampleProjectModule, "{14341863-5f76-4dea-9427-5f95ace5847a}", AZ::Module);
        AZ_CLASS_ALLOCATOR(NavExampleProjectModule, AZ::SystemAllocator, 0);

        NavExampleProjectModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                NavExampleProjectSystemComponent::CreateDescriptor(),
                NavGoToTargetComponent::CreateDescriptor(),
                NavAgentTransportComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<NavExampleProjectSystemComponent>(),
            };
        }
    };
}// namespace NavExampleProject

AZ_DECLARE_MODULE_CLASS(Gem_NavExampleProject, NavExampleProject::NavExampleProjectModule)
