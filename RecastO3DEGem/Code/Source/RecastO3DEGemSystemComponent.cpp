
#include <RecastO3DEGemSystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <RecastO3DE/RecastO3DEGemBus.h>

namespace RecastO3DE
{
    void RecastO3DEGemSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RecastO3DEGemSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<RecastO3DEGemSystemComponent>("RecastO3DEGem", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void RecastO3DEGemSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RecastO3DEGemService"));
    }

    void RecastO3DEGemSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RecastO3DEGemService"));
    }

    void RecastO3DEGemSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void RecastO3DEGemSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    RecastO3DEGemSystemComponent::RecastO3DEGemSystemComponent()
    {
        if (RecastO3DEGemInterface::Get() == nullptr)
        {
            RecastO3DE::RecastO3DEGemInterface::Register(this);
        }
    }

    RecastO3DEGemSystemComponent::~RecastO3DEGemSystemComponent()
    {
        if (RecastO3DE::RecastO3DEGemInterface::Get() == this)
        {
            RecastO3DE::RecastO3DEGemInterface::Unregister(this);
        }
    }

    void RecastO3DEGemSystemComponent::Init()
    {
    }

    void RecastO3DEGemSystemComponent::Activate()
    {
        RecastO3DE::RecastO3DEGemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void RecastO3DEGemSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        RecastO3DE::RecastO3DEGemRequestBus::Handler::BusDisconnect();
    }

    void RecastO3DEGemSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace RecastO3DE
