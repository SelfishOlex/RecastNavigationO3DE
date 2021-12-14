#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Serialization/EditContext.h>

namespace RecastO3DE
{
    class CrowdAgentSettings
    {
    public:
        AZ_TYPE_INFO(CrowdAgentSettings, "{1EED2752-B160-4E64-A07E-AFF0FABAC254}");

        float m_radius = 1.F;					///< Agent radius. [Limit: >= 0]
        float m_height = 2.F;					///< Agent height. [Limit: > 0]
        float m_maxAcceleration = 1.F;			///< Maximum allowed acceleration. [Limit: >= 0]
        float m_maxSpeed = 10.F;				///< Maximum allowed speed. [Limit: >= 0]

        AZStd::optional<AZ::EntityId> m_targetEntity;
        AZStd::optional<AZ::Vector3> m_targetPosition;

        int m_agentIndex = 0;

        static void Reflect(AZ::ReflectContext* context)
        {
            if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<CrowdAgentSettings>()
                    ->Version(1)
                    ->Field("Radius", &CrowdAgentSettings::m_radius)
                    ->Field("Height", &CrowdAgentSettings::m_height)
                    ->Field("Max Acceleration", &CrowdAgentSettings::m_maxAcceleration)
                    ->Field("Max Speed", &CrowdAgentSettings::m_maxSpeed)
                    ;

                if (AZ::EditContext* ec = serialize->GetEditContext())
                {
                    ec->Class<CrowdAgentSettings>("Crowd Agent Settings", "[Configuration of an agent in a navigation crowd]")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(nullptr, &CrowdAgentSettings::m_radius, "Radius", "")
                        ->DataElement(nullptr, &CrowdAgentSettings::m_height, "Height", "")
                        ->DataElement(nullptr, &CrowdAgentSettings::m_maxAcceleration, "Max Acceleration", "")
                        ->DataElement(nullptr, &CrowdAgentSettings::m_maxSpeed, "Max Speed", "")
                        ;
                }
            }
        }
    };

    class RecastNavigationCrowdAgentRequests
        : public AZ::ComponentBus
    {
    public:

        virtual void SetCrowdAgentSettings(CrowdAgentSettings newSettings) = 0;
        virtual CrowdAgentSettings GetCrowdAgentSettings() = 0;

        virtual void SetCrowdAgentIndex(int agentIndexInCrowd, AZ::EntityId crowdEntity) = 0;

        virtual int GetAgentIndex() = 0;
        virtual AZ::EntityId GetCrowdEntity() = 0;

        virtual void SetCrowdAgentTargetPosition(const AZ::Vector3& worldPosition) = 0;
        virtual void SetCrowdAgentTargetEntity(AZ::EntityId targetEntity) = 0;
    };

    using RecastNavigationCrowdAgentRequestBus = AZ::EBus<RecastNavigationCrowdAgentRequests>;

    class RecastNavigationCrowdAgentNotifications
        : public AZ::ComponentBus
    {
    public:

        virtual void OnCrowdAgentPosition([[maybe_unused]] const AZ::Vector3& newPosition) {}
    };

    using RecastNavigationCrowdAgentNotificationBus = AZ::EBus<RecastNavigationCrowdAgentNotifications>;

}
