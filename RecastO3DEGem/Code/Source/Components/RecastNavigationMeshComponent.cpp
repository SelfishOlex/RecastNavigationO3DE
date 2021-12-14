#include "RecastNavigationMeshComponent.h"

#include <CrySystemBus.h>
#include <ISystem.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Jobs/JobFunction.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Physics/Shape.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <DebugDraw/DebugDrawBus.h>

#pragma optimize("", off)

namespace RecastO3DE
{
    void RecastNavigationMeshComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<RecastNavigationMeshComponent, AZ::Component>()
                ->Version(3)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<RecastNavigationMeshComponent>("Recast Navigation Mesh", "[Calculates the walkable navigation mesh]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void RecastNavigationMeshComponent::GetProvidedServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
    }

    void RecastNavigationMeshComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void RecastNavigationMeshComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void RecastNavigationMeshComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    RecastNavigationMeshComponent::Geometry RecastNavigationMeshComponent::GetColliderGeometry(
        [[maybe_unused]] const AZ::Aabb& aabb,
        [[maybe_unused]] const AzPhysics::SceneQueryHits& overlapHits)
    {
        Geometry geom;

        AZ::Aabb volumeAabb = aabb;

        AZStd::size_t indicesCount = 0;

        AZStd::vector<AZ::Vector3> vertices;
        AZStd::vector<AZ::u32> indices;

        for (const AzPhysics::SceneQueryHit& overlapHit : overlapHits.m_hits)
        {
            if ((overlapHit.m_resultFlags & AzPhysics::SceneQuery::EntityId) != 0)
            {
                AZ::EntityId hitEntityId = overlapHit.m_entityId;

                vertices.clear();
                indices.clear();

                bool isWalkable = false;
                RecastWalkableRequestBus::EventResult(isWalkable, hitEntityId, &RecastWalkableRequestBus::Events::IsWalkable, GetEntityId());
                if (isWalkable == false)
                {
                    continue;
                }

                // most physics bodies just have world transforms, but some also have local transforms including terrain.
                // we are not applying the local orientation because it causes terrain geometry to be oriented incorrectly

                const AZStd::pair<AZ::Vector3, AZ::Quaternion> localPose = overlapHit.m_shape->GetLocalPose();

                AZ::Transform t = AZ::Transform::CreateIdentity();
                AZ::TransformBus::EventResult(t, hitEntityId, &AZ::TransformBus::Events::GetWorldTM);

                const AZ::Transform worldTransform = t * AZ::Transform::CreateTranslation(overlapHit.m_shape->GetLocalPose().first);

                overlapHit.m_shape->GetGeometry(vertices, indices, &volumeAabb);
                if (!vertices.empty())
                {
                    if (!indices.empty())
                    {
                        for (const AZ::Vector3& vertex : vertices)
                        {
                            geom.m_verts.push_back(RecastVector3(worldTransform.TransformVector(vertex)));
                        }

                        for (size_t i = 2; i < indices.size(); i += 3)
                        {
                            geom.m_indices.push_back(static_cast<AZ::u32>(indicesCount + indices[i]));
                            geom.m_indices.push_back(static_cast<AZ::u32>(indicesCount + indices[i - 1]));
                            geom.m_indices.push_back(static_cast<AZ::u32>(indicesCount + indices[i - 2]));
                        }

                        indicesCount += vertices.size();
                    }
                    else
                    {
                        AZ_Assert(false, "Not implemented");
                    }
                }
            }
        }

        return geom;
    }

    void RecastNavigationMeshComponent::CustomDebugDraw::begin(duDebugDrawPrimitives prim, [[maybe_unused]] float size)
    {
        //AZ_Printf( "RecastNavigationMeshComponent", "duDebugDrawPrimitives = %d", (int)prim );

        m_currentPrim = prim;
        m_verticesToDraw.clear();
    }

    void RecastNavigationMeshComponent::CustomDebugDraw::end()
    {
        constexpr float drawTime = 3.F;

        switch (m_currentPrim)
        {
        case DU_DRAW_POINTS:
            for (size_t i = 0; i < m_verticesToDraw.size(); ++i)
            {
                AZ::Color color;
                color.FromU32(m_verticesToDraw[i].second);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawSphereAtLocation,
                    m_verticesToDraw[i].first, 0.1F, color, drawTime);
            }
            break;
        case DU_DRAW_TRIS:
            for (size_t i = 2; i < m_verticesToDraw.size(); i += 3)
            {
                AZ::Color color;
                color.FromU32(m_verticesToDraw[i].second);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 2].first, m_verticesToDraw[i - 1].first, color, drawTime);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 1].first, m_verticesToDraw[i - 0].first, color, drawTime);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 0].first, m_verticesToDraw[i - 2].first, color, drawTime);
            }
            break;
        case DU_DRAW_QUADS:
            for (size_t i = 3; i < m_verticesToDraw.size(); i += 4)
            {
                AZ::Color color;
                color.FromU32(m_verticesToDraw[i].second);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 3].first, m_verticesToDraw[i - 2].first, color, drawTime);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 2].first, m_verticesToDraw[i - 1].first, color, drawTime);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 1].first, m_verticesToDraw[i - 0].first, color, drawTime);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 0].first, m_verticesToDraw[i - 3].first, color, drawTime);
            }
            break;
        case DU_DRAW_LINES:
        default:
            for (size_t i = 1; i < m_verticesToDraw.size(); i += 2)
            {
                AZ::Color color;
                color.FromU32(m_verticesToDraw[i].second);

                DebugDraw::DebugDrawRequestBus::Broadcast(&DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    m_verticesToDraw[i - 1].first, m_verticesToDraw[i].first, color, drawTime);
            }
            break;
        }
    }

    RecastVector3 RecastNavigationMeshComponent::GetPolyCenter(dtNavMesh* navMesh, dtPolyRef ref)
    {
        RecastVector3 centerRecast;

        float center[3];
        center[0] = 0;
        center[1] = 0;
        center[2] = 0;

        const dtMeshTile* tile = 0;
        const dtPoly* poly = 0;
        dtStatus status = navMesh->getTileAndPolyByRef(ref, &tile, &poly);
        if (dtStatusFailed(status))
        {
            return {};
        }

        if (poly->vertCount == 0)
        {
            return {};
        }

        for (int i = 0; i < (int)poly->vertCount; ++i)
        {
            const float* v = &tile->verts[poly->verts[i] * 3];
            center[0] += v[0];
            center[1] += v[1];
            center[2] += v[2];
        }
        const float s = 1.0f / poly->vertCount;
        center[0] *= s;
        center[1] *= s;
        center[2] *= s;

        centerRecast.m_x = center[0];
        centerRecast.m_y = center[1];
        centerRecast.m_z = center[2];

        return centerRecast;
    }

    bool RecastNavigationMeshComponent::UpdateNavigationMesh()
    {
        m_navMeshReady = false;
        m_waitingOnNavMeshRebuild = true;

        AZ::Vector3 dimension = m_worldBounds.GetExtents();
        AZ::Transform pose = AZ::Transform::CreateFromQuaternionAndTranslation(AZ::Quaternion::CreateIdentity(), m_worldBounds.GetCenter());

        Physics::BoxShapeConfiguration shapeConfiguration;
        shapeConfiguration.m_dimensions = dimension;
        
        AzPhysics::OverlapRequest request = AzPhysics::OverlapRequestHelpers::CreateBoxOverlapRequest(dimension, pose, nullptr);
        request.m_queryType = AzPhysics::SceneQuery::QueryType::Static;
        request.m_collisionGroup = AzPhysics::CollisionGroup::All;

        auto sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::SceneQueryHits results = AZ::Interface<AzPhysics::SceneInterface>::Get()->QueryScene(sceneHandle, &request);

        if (results.m_hits.empty())
        {
            return false;
        }

        AZ_Printf("RecastNavigationMeshComponent", "found %llu physx meshes", results.m_hits.size());

        m_geom = GetColliderGeometry(m_worldBounds, results);

        auto* job = AZ::CreateJobFunction([this]()
            {
                if (UpdateNavigationMesh_JobThread())
                {
                    m_navMeshReady = true;
                }

                m_geom.clear();
            }
        , true);

        job->Start();

        return true;
    }

    bool RecastNavigationMeshComponent::UpdateNavigationMesh_JobThread()
    {
        const float* verts = &m_geom.m_verts.front().m_x;
        const int nverts = static_cast<int>(m_geom.m_verts.size());
        const int* tris = &m_geom.m_indices[0];
        const int ntris = static_cast<int>(m_geom.m_indices.size()) / 3;

        //
        // Step 1. Initialize build config.
        //

        // Init build configuration from GUI
        memset(&m_cfg, 0, sizeof(m_cfg));
        m_cfg.cs = m_cellSize;
        m_cfg.ch = m_cellHeight;
        m_cfg.walkableSlopeAngle = m_agentMaxSlope;
        m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
        m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
        m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
        m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
        m_cfg.maxSimplificationError = m_edgeMaxError;
        m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
        m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
        m_cfg.maxVertsPerPoly = (int)m_maxVertsPerPoly;
        m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
        m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

        // Set the area where the navigation will be build.
        // Here the bounds of the input mesh are used, but the
        // area could be specified by an user defined box, etc.

        RecastVector3 worldMin(m_worldBounds.GetMin());
        RecastVector3 worldMax(m_worldBounds.GetMax());

        rcVcopy(m_cfg.bmin, &worldMin.m_x);
        rcVcopy(m_cfg.bmax, &worldMax.m_x);
        rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

        // Reset build times gathering.
        m_ctx->resetTimers();

        // Start the build process.
        m_ctx->startTimer(RC_TIMER_TOTAL);

        m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
        m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
        m_ctx->log(RC_LOG_PROGRESS, " - %d verts, %d tris", nverts, ntris);

        //
        // Step 2. Rasterize input polygon soup.
        //

        // Allocate voxel heightfield where we rasterize our input data to.
        m_solid.reset(rcAllocHeightfield());
        if (!m_solid)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
            return false;
        }
        if (!rcCreateHeightfield(m_ctx.get(), *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
            return false;
        }

        // Allocate array that can hold triangle area types.
        // If you have multiple meshes you need to process, allocate
        // and array which can hold the max number of triangles you need to process.
        m_triareas.resize(ntris, 0);

        // Find triangles which are walkable based on their slope and rasterize them.
        // If your input data is multiple meshes, you can transform them here, calculate
        // the are type for each of the meshes and rasterize them.
        rcMarkWalkableTriangles(m_ctx.get(), m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas.data());
        if (!rcRasterizeTriangles(m_ctx.get(), verts, nverts, tris, m_triareas.data(), ntris, *m_solid /*, m_cfg.walkableClimb*/))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
            return false;
        }

        if (!m_keepInterResults)
        {
            m_triareas.clear();
        }

        //
        // Step 3. Filter walkables surfaces.
        //

        // Once all geoemtry is rasterized, we do initial pass of filtering to
        // remove unwanted overhangs caused by the conservative rasterization
        // as well as filter spans where the character cannot possibly stand.
        if (m_filterLowHangingObstacles)
            rcFilterLowHangingWalkableObstacles(m_ctx.get(), m_cfg.walkableClimb, *m_solid);
        if (m_filterLedgeSpans)
            rcFilterLedgeSpans(m_ctx.get(), m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
        if (m_filterWalkableLowHeightSpans)
            rcFilterWalkableLowHeightSpans(m_ctx.get(), m_cfg.walkableHeight, *m_solid);

        //if ( false )
        /*{
            m_customDebugDraw.SetColor( AZ::Color( 1.F, 0, 0, 0.1F ) );
            duDebugDrawHeightfieldSolid( &m_customDebugDraw, *m_solid );
        }*/

        //
        // Step 4. Partition walkable surface to simple regions.
        //

        // Compact the heightfield so that it is faster to handle from now on.
        // This will result more cache coherent data as well as the neighbours
        // between walkable cells will be calculated.
        m_chf.reset(rcAllocCompactHeightfield());
        if (!m_chf)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
            return false;
        }
        if (!rcBuildCompactHeightfield(m_ctx.get(), m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
            return false;
        }

        if (!m_keepInterResults)
        {
            m_solid.reset();
        }

        // Erode the walkable area by agent radius.
        if (!rcErodeWalkableArea(m_ctx.get(), m_cfg.walkableRadius, *m_chf))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
            return false;
        }

        {
            // Partition the walkable surface into simple regions without holes.
            // Monotone partitioning does not need distancefield.
            if (!rcBuildRegionsMonotone(m_ctx.get(), *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
            {
                m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
                return false;
            }
        }

        /*{
            m_customDebugDraw.SetColor( AZ::Color( 0.F, 1, 0, 1 ) );
            duDebugDrawCompactHeightfieldSolid( &m_customDebugDraw, *m_chf );
        }*/

        //
        // Step 5. Trace and simplify region contours.
        //

        // Create contours.
        m_cset.reset(rcAllocContourSet());
        if (!m_cset)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
            return false;
        }
        if (!rcBuildContours(m_ctx.get(), *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
            return false;
        }

        /*{
            m_customDebugDraw.SetColor( AZ::Color( 0.F, 0, 1, 1 ) );
            duDebugDrawContours( &m_customDebugDraw, *m_cset );
        }*/

        //
        // Step 6. Build polygons mesh from contours.
        //

        // Build polygon navmesh from the contours.
        m_pmesh.reset(rcAllocPolyMesh());
        if (!m_pmesh)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
            return false;
        }
        if (!rcBuildPolyMesh(m_ctx.get(), *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
            return false;
        }

        /*{
            m_customDebugDraw.SetColor( AZ::Color( 0.F, 1, 1, 1 ) );
            duDebugDrawPolyMesh( &m_customDebugDraw, *m_pmesh );
        }*/

        //
        // Step 7. Create detail mesh which allows to access approximate height on each polygon.
        //

        m_dmesh.reset(rcAllocPolyMeshDetail());
        if (!m_dmesh)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
            return false;
        }

        if (!rcBuildPolyMeshDetail(m_ctx.get(), *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
            return false;
        }

        /*{
            m_customDebugDraw.SetColor( AZ::Color( 0.5F, 1, 1, 1 ) );
            duDebugDrawPolyMeshDetail( &m_customDebugDraw, *m_dmesh );
        }*/

        if (!m_keepInterResults)
        {
            m_chf = nullptr;
            m_cset = nullptr;
        }

        // At this point the navigation mesh data is ready, you can access it from m_pmesh.
        // See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

        //
        // (Optional) Step 8. Create Detour data from Recast poly mesh.
        //

        // The GUI may allow more max points per polygon than Detour can handle.
        // Only build the detour navmesh if we do not exceed the limit.
        if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
        {
            unsigned char* navData = nullptr;
            int navDataSize = 0;

            // Update poly flags from areas.
            for (int i = 0; i < m_pmesh->npolys; ++i)
            {
                if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
                {
                    m_pmesh->flags[i] = RC_WALKABLE_AREA;
                }
            }

            dtNavMeshCreateParams params;
            memset(&params, 0, sizeof(params));
            params.verts = m_pmesh->verts;
            params.vertCount = m_pmesh->nverts;
            params.polys = m_pmesh->polys;
            params.polyAreas = m_pmesh->areas;
            params.polyFlags = m_pmesh->flags;
            params.polyCount = m_pmesh->npolys;
            params.nvp = m_pmesh->nvp;
            params.detailMeshes = m_dmesh->meshes;
            params.detailVerts = m_dmesh->verts;
            params.detailVertsCount = m_dmesh->nverts;
            params.detailTris = m_dmesh->tris;
            params.detailTriCount = m_dmesh->ntris;

            params.offMeshConVerts = nullptr;
            params.offMeshConRad = nullptr;
            params.offMeshConDir = nullptr;
            params.offMeshConAreas = nullptr;
            params.offMeshConFlags = nullptr;
            params.offMeshConUserID = nullptr;
            params.offMeshConCount = 0;

            params.walkableHeight = m_agentHeight;
            params.walkableRadius = m_agentRadius;
            params.walkableClimb = m_agentMaxClimb;
            rcVcopy(params.bmin, m_pmesh->bmin);
            rcVcopy(params.bmax, m_pmesh->bmax);
            params.cs = m_cfg.cs;
            params.ch = m_cfg.ch;
            params.buildBvTree = true;

            if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
            {
                m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
                return false;
            }

            m_navMesh.reset(dtAllocNavMesh());
            if (!m_navMesh)
            {
                dtFree(navData);
                m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
                return false;
            }

            dtStatus status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
            if (dtStatusFailed(status))
            {
                dtFree(navData);
                m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
                return false;
            }

            {
                m_customDebugDraw.SetColor(AZ::Color(0.F, 0.9, 0, 1));
                duDebugDrawNavMesh(&m_customDebugDraw, *m_navMesh, DU_DRAWNAVMESH_COLOR_TILES);
            }

            m_navQuery.reset(dtAllocNavMeshQuery());

            status = m_navQuery->init(m_navMesh.get(), 2048);
            if (dtStatusFailed(status))
            {
                m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
                return false;
            }

            m_ctx->stopTimer(RC_TIMER_TOTAL);
        }

        return true;
    }

    void RecastNavigationMeshComponent::SetWorldBounds(const AZ::Aabb& worldBounds)
    {
        m_worldBounds = worldBounds;
    }

    AZStd::vector<AZ::Vector3> RecastNavigationMeshComponent::FindPathToEntity(AZ::EntityId fromEntity, AZ::EntityId toEntity)
    {
        if (m_navMeshReady)
        {
            if (fromEntity.IsValid() && toEntity.IsValid())
            {
                AZ::Vector3 start, end;
                AZ::TransformBus::EventResult(start, fromEntity, &AZ::TransformBus::Events::GetWorldTranslation);
                AZ::TransformBus::EventResult(end, toEntity, &AZ::TransformBus::Events::GetWorldTranslation);

                return FindPathToPosition(start, end);
            }
        }

        return {};
    }

    AZStd::vector<AZ::Vector3> RecastNavigationMeshComponent::FindPathToPosition(const AZ::Vector3& fromWorldPosition, const AZ::Vector3& targetWorldPosition)
    {
        if (m_navMeshReady == false)
        {
            return {};
        }

        AZStd::vector<AZ::Vector3> pathPoints;

        {
            RecastVector3 startRecast{ fromWorldPosition }, endRecast{ targetWorldPosition };
            const float halfExtents[3] = { 1.F, 1., 1 };

            dtPolyRef startPoly = 0, endPoly = 0;

            RecastVector3 nearestStartPoint, nearestEndPoint;

            const dtQueryFilter filter;

            dtStatus result = m_navQuery->findNearestPoly(startRecast.data(), halfExtents, &filter, &startPoly, nearestStartPoint.data());
            if (result != DT_SUCCESS)
            {
                return {};
            }

            result = m_navQuery->findNearestPoly(endRecast.data(), halfExtents, &filter, &endPoly, nearestEndPoint.data());
            if (result != DT_SUCCESS)
            {
                return {};
            }

            constexpr int maxPathLength = 100;
            dtPolyRef path[maxPathLength] = {};
            int pathLength = 0;

            // find an approximate path
            m_navQuery->findPath(startPoly, endPoly, nearestStartPoint.data(), nearestEndPoint.data(), &filter, path, &pathLength, maxPathLength);

            //AZ_Printf( "Olex", "from %d to %d, findPath = %d", startPoly, endPoly, pathLength );

            AZStd::vector<RecastVector3> approximatePath;
            approximatePath.resize(pathLength);

            for (int pathIndex = 0; pathIndex < pathLength; ++pathIndex)
            {
                RecastVector3 center = GetPolyCenter(m_navMesh.get(), path[pathIndex]);
                approximatePath.push_back(center);

                //AZ_Printf( "Olex", "path %d = %.1f %.1f %.1f", pathIndex, center.m_x, center.m_y, center.m_z );
            }

            constexpr int maxDetailedPathLength = 100;
            RecastVector3 detailedPath[maxDetailedPathLength] = {};
            AZ::u8 detailedPathFlags[maxDetailedPathLength] = {};
            dtPolyRef detailedPolyPathRefs[maxDetailedPathLength] = {};
            int detailedPathCount = 0;

            m_navQuery->findStraightPath(startRecast.data(), endRecast.data(), path, pathLength, detailedPath[0].data(), detailedPathFlags, detailedPolyPathRefs,
                &detailedPathCount, maxDetailedPathLength);

            //AZ_Printf( "RecastNavigationMeshComponent", "Recast find path took %d microseconds", timer.GetElapsedMicroseconds() );

            /*for ( size_t pathIndex = 1; pathIndex < approximatePath.size(); ++pathIndex )
            {
                DebugDraw::DebugDrawRequestBus::Broadcast( &DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    approximatePath[pathIndex - 1].GetAZVector3(), approximatePath[pathIndex].GetAZVector3(), AZ::Color( 1.F, 0, 0, 1 ), 30.F );
            }

            for ( size_t detailedIndex = 1; detailedIndex < approximatePath.size(); ++detailedIndex )
            {
                DebugDraw::DebugDrawRequestBus::Broadcast( &DebugDraw::DebugDrawRequestBus::Events::DrawLineLocationToLocation,
                    detailedPath[detailedIndex - 1].GetAZVector3(), detailedPath[detailedIndex].GetAZVector3(), AZ::Color( 0.F, 1, 0, 1 ), 30.F );
            }*/

            for (int i = 0; i < detailedPathCount; ++i)
            {
                pathPoints.push_back(detailedPath[i].AsVector3());
            }
        }

        return pathPoints;
    }

    bool RecastNavigationMeshComponent::OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel)
    {
        const AzFramework::InputChannelId& inputChannelId = inputChannel.GetInputChannelId();

        const AzFramework::InputDeviceId& device = inputChannel.GetInputDevice().GetInputDeviceId();
        if (AzFramework::InputDeviceKeyboard::IsKeyboardDevice(device))
        {
            if (inputChannelId == AzFramework::InputDeviceKeyboard::Key::AlphanumericF)
            {
                UpdateNavigationMesh();
            }
        }

        return false;
    }

    void RecastNavigationMeshComponent::Activate()
    {
        m_ctx = AZStd::make_unique<CustomContext>();

        AZ::Vector3 position;
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        m_worldBounds = AZ::Aabb::CreateCenterRadius(position, 100.F);

        RecastNavigationMeshRequestBus::Handler::BusConnect(GetEntityId());

        AzFramework::GameEntityContextEventBus::Handler::BusConnect();
        AzFramework::InputChannelEventListener::Connect();
        AZ::TickBus::Handler::BusConnect();
    }

    void RecastNavigationMeshComponent::Deactivate()
    {
        m_navQuery = {};
        m_navMesh = {};

        RecastNavigationMeshRequestBus::Handler::BusDisconnect();
        AzFramework::GameEntityContextEventBus::Handler::BusDisconnect();
        AzFramework::InputChannelEventListener::Disconnect();
        AZ::TickBus::Handler::BusDisconnect();
    }

    void RecastNavigationMeshComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (m_waitingOnNavMeshRebuild)
        {
            if (m_navMeshReady)
            {
                m_waitingOnNavMeshRebuild = false;
                RecastNavigationMeshNotificationBus::Event(GetEntityId(), &RecastNavigationMeshNotificationBus::Events::OnNavigationMeshUpdated, m_navMesh.get(), m_navQuery.get());
            }
        }
    }
} // namespace RecastO3DE
