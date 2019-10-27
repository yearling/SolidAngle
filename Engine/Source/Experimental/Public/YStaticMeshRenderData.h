#pragma once
#include "core.h"
#include "YMeshCommon.h"


struct YStaticMeshLODResources
{

};

class YStaticMeshRenderData
{
public:
	YStaticMeshRenderData();
	TIndirectArray<YStaticMeshLODResources> LODResources;
	/** Screen size to switch LODs */
	float ScreenSize[MAX_YSTATIC_MESH_LODS];

	/** Bounds of the renderable mesh. */
	FBoxSphereBounds Bounds;
	/** Map of wedge index to vertex index. */
	TArray<int32> WedgeMap;

	/** Map of material index -> original material index at import time. */
	TArray<int32> MaterialIndexToImportIndex;

	/** UV data used for streaming accuracy debug view modes. In sync for rendering thread */
	//TArray<FMeshUVChannelInfo> UVChannelDataPerMaterial;
};
