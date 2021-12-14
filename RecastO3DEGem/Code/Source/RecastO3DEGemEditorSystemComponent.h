
#pragma once

#include <RecastO3DEGemSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

namespace RecastO3DE
{
    /// System component for RecastO3DEGem editor
    class RecastO3DEGemEditorSystemComponent
        : public RecastO3DE::RecastO3DEGemSystemComponent
        , private AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = RecastO3DEGemSystemComponent;
    public:
        AZ_COMPONENT(RecastO3DEGemEditorSystemComponent, "{ff92cb51-bb03-4e6e-a465-f3d7a2db8c54}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        RecastO3DEGemEditorSystemComponent();
        ~RecastO3DEGemEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace RecastO3DE
