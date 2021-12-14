#include "RecastWalkableComponent.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace RecastO3DE
{
    void RecastWalkableComponent::Reflect( AZ::ReflectContext* context )
    {
        if ( AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>( context ) )
        {
            serialize->Class<RecastWalkableComponent, AZ::Component>()
                ->Version( 2 )
                ->Field( "Is Walkable", &RecastWalkableComponent::m_isWalkable )
                ;

            if ( AZ::EditContext* ec = serialize->GetEditContext() )
            {
                ec->Class<RecastWalkableComponent>( "Recast Walkable",
                    "[Marks this entity to be considered by navigation mesh]" )
                    ->ClassElement( AZ::Edit::ClassElements::EditorData, "" )
                    ->Attribute( AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC( "Game" ) )
                    ->Attribute( AZ::Edit::Attributes::AutoExpand, true )
                    ->DataElement( nullptr, &RecastWalkableComponent::m_isWalkable, "Is Walkable", "" )
                    ;
            }
        }
    }

    void RecastWalkableComponent::Activate()
    {
        RecastWalkableRequestBus::Handler::BusConnect( GetEntityId() );
    }

    void RecastWalkableComponent::Deactivate()
    {
        RecastWalkableRequestBus::Handler::BusDisconnect();
    }
} // namespace RecastO3DE
