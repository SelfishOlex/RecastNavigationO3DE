#pragma once

#include <AzCore/Component/Component.h>
#include <RecastO3DE/RecastNavigationMeshBus.h>

namespace RecastO3DE
{
    class RecastWalkableComponent
        : public AZ::Component
        , public RecastWalkableRequestBus::Handler
    {
    public:
        AZ_COMPONENT( RecastWalkableComponent, "{9365DB87-581A-48D3-BC4B-13FB000D90AC}" );

        static void Reflect( AZ::ReflectContext* context );

        // AZ::Component interface implementation
        void Activate() override;
        void Deactivate() override;

        // RecastWalkableRequestBus
        bool IsWalkable([[maybe_unused]] AZ::EntityId navigationMeshEntity) override { return m_isWalkable; }

    private:
        bool m_isWalkable = true;
    };

} // namespace RecastO3DE
