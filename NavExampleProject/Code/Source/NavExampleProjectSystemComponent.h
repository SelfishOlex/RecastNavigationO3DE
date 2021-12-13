
#pragma once

#include <AzCore/Component/Component.h>

#include <NavExampleProject/NavExampleProjectBus.h>

namespace NavExampleProject
{
    class NavExampleProjectSystemComponent
        : public AZ::Component
        , protected NavExampleProjectRequestBus::Handler
    {
    public:
        AZ_COMPONENT(NavExampleProjectSystemComponent, "{79235f4c-1baa-424c-b993-925ea5e4daad}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        NavExampleProjectSystemComponent();
        ~NavExampleProjectSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // NavExampleProjectRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
