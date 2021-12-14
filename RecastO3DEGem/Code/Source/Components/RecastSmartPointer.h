
#pragma once

#include <DebugDraw.h>
#include <DetourCrowd.h>
#include <DetourDebugDraw.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>

#include <AzCore/std/smart_ptr/unique_ptr.h>

namespace RecastO3DE
{
    template <typename T>
    struct CustomRecastDeleter;

    template <typename T>
    using RecastPointer = AZStd::unique_ptr<T, CustomRecastDeleter<T>>;

    template <>
    struct CustomRecastDeleter<rcHeightfield>
    {
        void operator ()( rcHeightfield* p )
        {
            rcFreeHeightField( p );
        }
    };

    template <>
    struct CustomRecastDeleter<rcCompactHeightfield>
    {
        void operator ()( rcCompactHeightfield* p )
        {
            rcFreeCompactHeightfield( p );
        }
    };

    template <>
    struct CustomRecastDeleter<rcContourSet>
    {
        void operator ()( rcContourSet* p )
        {
            rcFreeContourSet( p );
        }
    };

    template <>
    struct CustomRecastDeleter<rcPolyMesh>
    {
        void operator ()( rcPolyMesh* p )
        {
            rcFreePolyMesh( p );
        }
    };

    template <>
    struct CustomRecastDeleter<rcPolyMeshDetail>
    {
        void operator ()( rcPolyMeshDetail* p )
        {
            rcFreePolyMeshDetail( p );
        }
    };

    template <>
    struct CustomRecastDeleter<dtNavMesh>
    {
        void operator ()( dtNavMesh* p )
        {
            dtFreeNavMesh( p );
        }
    };

    template <>
    struct CustomRecastDeleter<dtNavMeshQuery>
    {
        void operator ()( dtNavMeshQuery* p )
        {
            dtFreeNavMeshQuery( p );
        }
    };

    template <>
    struct CustomRecastDeleter<dtCrowd>
    {
        void operator ()( dtCrowd* p )
        {
            dtFreeCrowd( p );
        }
    };
}
