#pragma once
#include "core.h"
#include "YMeshCommon.h"
#include "YStaticMeshResource.h"


struct YStaticMeshLODResources
{
	TArray<YStaticMeshSection> Sections;
	/** The buffer containing vertex data. */
	YStaticMeshTangentUVVertexBuffer VertexBuffer;
	/** The buffer containing the position vertex data. */
	YPositionVertexBuffer PositionVertexBuffer;
	/** Index buffer resource for rendering. */
	YRawStaticIndexBuffer IndexBuffer;
	/** Reversed index buffer, used to prevent changing culling state between drawcalls. */
	YRawStaticIndexBuffer ReversedIndexBuffer;
	/** Index buffer resource for rendering in depth only passes. */
	YRawStaticIndexBuffer DepthOnlyIndexBuffer;
	/** Reversed depth only index buffer, used to prevent changing culling state between drawcalls. */
	YRawStaticIndexBuffer ReversedDepthOnlyIndexBuffer;
	/** Index buffer resource for rendering wireframe mode. */
	YRawStaticIndexBuffer WireframeIndexBuffer;
	/** Index buffer containing adjacency information required by tessellation. */
	YRawStaticIndexBuffer AdjacencyIndexBuffer;
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

	void AllocateLODResources(int32 NumLODs);
};
