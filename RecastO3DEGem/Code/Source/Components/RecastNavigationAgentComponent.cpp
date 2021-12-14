#include "RecastNavigationAgentComponent.h"


#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <RecastO3DE/RecastNavigationCrowdBus.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

namespace RecastO3DE
{
    void RecastNavigationAgentComponent::Reflect( AZ::ReflectContext* context )
    {
        if ( AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>( context ) )
        {
            serialize->Class<RecastNavigationAgentComponent, AZ::Component>()
                ->Version( 1 )
                ->Field( "Navigation Mesh Entity", &RecastNavigationAgentComponent::m_navigationMeshEntityId )
                ;

            if ( AZ::EditContext* ec = serialize->GetEditContext() )
            {
                ec->Class<RecastNavigationAgentComponent>( "Recast Navigation Agent", "[Queries Recast Navigation Mesh]" )
                    ->ClassElement( AZ::Edit::ClassElements::EditorData, "" )
                    ->Attribute( AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC( "Game" ) )
                    ->Attribute( AZ::Edit::Attributes::AutoExpand, true )
                    ->DataElement( nullptr, &RecastNavigationAgentComponent::m_navigationMeshEntityId, "Navigation Mesh Entity", "" )
                    ;
            }
        }
    }

    void RecastNavigationAgentComponent::GetProvidedServices( [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& provided )
    {
    }

    void RecastNavigationAgentComponent::GetIncompatibleServices( [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible )
    {
    }

    void RecastNavigationAgentComponent::GetRequiredServices( [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required )
    {
    }

    void RecastNavigationAgentComponent::GetDependentServices( [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent )
    {
    }

    void RecastNavigationAgentComponent::Activate()
    {
        RecastNavigationAgentRequestBus::Handler::BusConnect( GetEntityId() );
        AZ::TransformNotificationBus::Handler::BusConnect( GetEntityId() );
        AZ::TickBus::Handler::BusConnect();
    }

    void RecastNavigationAgentComponent::Deactivate()
    {
        RecastNavigationAgentRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
    }

    void RecastNavigationAgentComponent::PathToEntity( AZ::EntityId targetEntity )
    {
        AZ::Vector3 end;
        AZ::TransformBus::EventResult( end, targetEntity, &AZ::TransformBus::Events::GetWorldTranslation );

        PathToPosition( end );
    }

    void RecastNavigationAgentComponent::PathToPosition( const AZ::Vector3& targetWorldPosition )
    {
        m_pathPoints.clear();
        m_currentPathIndex = 0;

        AZ::Vector3 start;
        AZ::TransformBus::EventResult( start, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation );

        RecastNavigationMeshRequestBus::EventResult( m_pathPoints, m_navigationMeshEntityId, &RecastNavigationMeshRequestBus::Events::FindPathToPosition, start, targetWorldPosition );

        if ( m_pathPoints.empty() )
        {
            RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnPathFailed );
        }
        else
        {
            RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnPathFound, m_pathPoints );

            if ( m_pathPoints.size() >= 3 )
            {
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnNextTraversalPoint, m_pathPoints[1], m_pathPoints[2] );
            }
            else if ( m_pathPoints.size() == 2 )
            {
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnNextTraversalPoint, m_pathPoints[1], m_pathPoints[1] );
            }
            else
            {
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnNextTraversalPoint, m_pathPoints[0], m_pathPoints[0] );
            }
        }
    }

    void RecastNavigationAgentComponent::CancelPath()
    {
        m_pathPoints.clear();
        m_currentPathIndex = 0;
        RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnTraversalCompleted );
    }

    void RecastNavigationAgentComponent::OnTick( [[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time )
    {
        // TODO is this needed?
    }

    void RecastNavigationAgentComponent::OnTransformChanged( [[maybe_unused]] const AZ::Transform& local, [[maybe_unused]] const AZ::Transform& world )
    {
        if ( m_pathPoints.empty() || m_currentPathIndex >= m_pathPoints.size() )
        {
            return;
        }

        if ( m_distanceForArrivingToPoint > m_pathPoints[m_currentPathIndex].GetDistance( world.GetTranslation() ) )
        {
            m_currentPathIndex++;

            if ( m_currentPathIndex + 1 == m_pathPoints.size() )
            {
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnNextTraversalPoint,
                    m_pathPoints[m_currentPathIndex], m_pathPoints[m_currentPathIndex] );
            }
            else if ( m_currentPathIndex + 1 < m_pathPoints.size() )
            {
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnNextTraversalPoint,
                    m_pathPoints[m_currentPathIndex], m_pathPoints[m_currentPathIndex + 1] );
            }
            else
            {
                m_pathPoints.clear();
                m_currentPathIndex = 0;
                RecastNavigationAgentNotificationBus::Event( GetEntityId(), &RecastNavigationAgentNotificationBus::Events::OnTraversalCompleted );
            }
        }
    }
} // namespace RecastO3DE
