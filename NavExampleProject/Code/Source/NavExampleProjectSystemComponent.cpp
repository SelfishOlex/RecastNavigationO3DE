
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include "NavExampleProjectSystemComponent.h"

namespace NavExampleProject
{
    void NavExampleProjectSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<NavExampleProjectSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<NavExampleProjectSystemComponent>("NavExampleProject", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void NavExampleProjectSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("NavExampleProjectService"));
    }

    void NavExampleProjectSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("NavExampleProjectService"));
    }

    void NavExampleProjectSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void NavExampleProjectSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
    
    NavExampleProjectSystemComponent::NavExampleProjectSystemComponent()
    {
        if (NavExampleProjectInterface::Get() == nullptr)
        {
            NavExampleProjectInterface::Register(this);
        }
    }

    NavExampleProjectSystemComponent::~NavExampleProjectSystemComponent()
    {
        if (NavExampleProjectInterface::Get() == this)
        {
            NavExampleProjectInterface::Unregister(this);
        }
    }

    void NavExampleProjectSystemComponent::Init()
    {
    }

    void NavExampleProjectSystemComponent::Activate()
    {
        NavExampleProjectRequestBus::Handler::BusConnect();
    }

    void NavExampleProjectSystemComponent::Deactivate()
    {
        NavExampleProjectRequestBus::Handler::BusDisconnect();
    }
}
