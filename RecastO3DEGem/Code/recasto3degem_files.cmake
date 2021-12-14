
set(FILES
    Include/RecastO3DE/RecastO3DEGemBus.h
    Source/RecastO3DEGemModuleInterface.h
    Source/RecastO3DEGemSystemComponent.cpp
    Source/RecastO3DEGemSystemComponent.h

    Include/RecastO3DE/RecastNavigationCrowdAgentBus.h
    Include/RecastO3DE/RecastNavigationCrowdBus.h
    Include/RecastO3DE/RecastNavigationAgentBus.h
    Include/RecastO3DE/RecastNavigationMeshBus.h

    Source/Components/RecastWalkableComponent.cpp
    Source/Components/RecastWalkableComponent.h
    Source/Components/RecastNavigationCrowdAgentComponent.cpp
    Source/Components/RecastNavigationCrowdAgentComponent.h
    Source/Components/RecastNavigationCrowdComponent.cpp
    Source/Components/RecastNavigationCrowdComponent.h
    Source/Components/RecastNavigationMeshComponent.cpp
    Source/Components/RecastNavigationMeshComponent.h
    Source/Components/RecastNavigationAgentComponent.cpp
    Source/Components/RecastNavigationAgentComponent.h
    Source/Components/RecastSmartPointer.h
    Source/Components/RecastHelpers.h

    ../External/Recast/Include/RecastAlloc.h
    ../External/Recast/Include/RecastAssert.h
    ../External/Recast/Include/Recast.h

    ../External/Recast/Source/Recast.cpp
    ../External/Recast/Source/RecastAlloc.cpp
    ../External/Recast/Source/RecastArea.cpp
    ../External/Recast/Source/RecastAssert.cpp
    ../External/Recast/Source/RecastContour.cpp
    ../External/Recast/Source/RecastFilter.cpp
    ../External/Recast/Source/RecastLayers.cpp
    ../External/Recast/Source/RecastMesh.cpp
    ../External/Recast/Source/RecastMeshDetail.cpp
    ../External/Recast/Source/RecastRasterization.cpp
    ../External/Recast/Source/RecastRegion.cpp

    ../External/Detour/Include/DetourAlloc.h
    ../External/Detour/Include/DetourAssert.h
    ../External/Detour/Include/DetourCommon.h
    ../External/Detour/Include/DetourMath.h
    ../External/Detour/Include/DetourNavMesh.h
    ../External/Detour/Include/DetourNavMeshBuilder.h
    ../External/Detour/Include/DetourNavMeshQuery.h
    ../External/Detour/Include/DetourNode.h
    ../External/Detour/Include/DetourStatus.h

    ../External/Detour/Source/DetourAlloc.cpp
    ../External/Detour/Source/DetourAssert.cpp
    ../External/Detour/Source/DetourCommon.cpp
    ../External/Detour/Source/DetourNavMesh.cpp
    ../External/Detour/Source/DetourNavMeshBuilder.cpp
    ../External/Detour/Source/DetourNavMeshQuery.cpp
    ../External/Detour/Source/DetourNode.cpp

    ../External/DetourCrowd/Include/DetourCrowd.h
    ../External/DetourCrowd/Include/DetourLocalBoundary.h
    ../External/DetourCrowd/Include/DetourObstacleAvoidance.h
    ../External/DetourCrowd/Include/DetourPathCorridor.h
    ../External/DetourCrowd/Include/DetourPathQueue.h
    ../External/DetourCrowd/Include/DetourProximityGrid.h

    ../External/DetourCrowd/Source/DetourCrowd.cpp
    ../External/DetourCrowd/Source/DetourLocalBoundary.cpp
    ../External/DetourCrowd/Source/DetourObstacleAvoidance.cpp
    ../External/DetourCrowd/Source/DetourPathCorridor.cpp
    ../External/DetourCrowd/Source/DetourPathQueue.cpp
    ../External/DetourCrowd/Source/DetourProximityGrid.cpp

    ../External/DebugUtils/Include/DebugDraw.h
    ../External/DebugUtils/Include/DetourDebugDraw.h
    ../External/DebugUtils/Include/RecastDebugDraw.h
    ../External/DebugUtils/Include/RecastDump.h

    ../External/DebugUtils/Source/DebugDraw.cpp
    ../External/DebugUtils/Source/DetourDebugDraw.cpp
    ../External/DebugUtils/Source/RecastDebugDraw.cpp
    ../External/DebugUtils/Source/RecastDump.cpp

    ../External/DetourTileCache/Include/DetourTileCache.h
    ../External/DetourTileCache/Include/DetourTileCacheBuilder.h

    ../External/DetourTileCache/Source/DetourTileCache.cpp
    ../External/DetourTileCache/Source/DetourTileCacheBuilder.cpp
)


