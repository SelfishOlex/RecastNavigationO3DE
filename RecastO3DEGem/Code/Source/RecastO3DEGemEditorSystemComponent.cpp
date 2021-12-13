
#include <AzCore/Serialization/SerializeContext.h>
#include <RecastO3DEGemEditorSystemComponent.h>

namespace RecastO3DEGem
{
    void RecastO3DEGemEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RecastO3DEGemEditorSystemComponent, RecastO3DEGemSystemComponent>()
                ->Version(0);
        }
    }

    RecastO3DEGemEditorSystemComponent::RecastO3DEGemEditorSystemComponent() = default;

    RecastO3DEGemEditorSystemComponent::~RecastO3DEGemEditorSystemComponent() = default;

    void RecastO3DEGemEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("RecastO3DEGemEditorService"));
    }

    void RecastO3DEGemEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("RecastO3DEGemEditorService"));
    }

    void RecastO3DEGemEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void RecastO3DEGemEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void RecastO3DEGemEditorSystemComponent::Activate()
    {
        RecastO3DEGemSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void RecastO3DEGemEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        RecastO3DEGemSystemComponent::Deactivate();
    }

} // namespace RecastO3DEGem
