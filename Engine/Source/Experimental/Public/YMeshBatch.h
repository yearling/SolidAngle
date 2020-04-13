#pragma once
#include "core.h"
#include "YMaterialInterface.h"
#include "YRenderResource.h"
struct YMeshBatchElement
{
	YIndexBuffer* IndexBuffer=nullptr;
	uint32 FirstIndex=0;
	uint32 NumPrimitives=0;

	/** Number of instances to draw.  If InstanceRuns is valid, this is actually the number of runs in InstanceRuns. */
	uint32 NumInstances=1;
	uint32 BaseVertexIndex=0;
	uint32 MinVertexIndex=0;
	uint32 MaxVertexIndex=0;
};
class YMeshBatch
{
public:
	YMeshBatch():ReverseCulling(false),
		         bDisableBackfaceCulling(false),
		         CastShadow(true),
		         bUseForMaterial(true),
		         bUseAsOccluder(true),
		         bWireframe(false) 
	             { new(Elements) YMeshBatchElement(); }
	YMaterialInterface* Material=nullptr;
	YVertexBuffer* VertexPositionBuffer=nullptr;
	YVertexBuffer* VertexBuffer = nullptr;
	TArray<YMeshBatchElement, TInlineAllocator<1> > Elements;
	uint32 ReverseCulling : 1;
	uint32 bDisableBackfaceCulling : 1;
	uint32 CastShadow : 1;				// Wheter it can be used in shadow renderpasses.
	uint32 bUseForMaterial : 1;	// Whether it can be used in renderpasses requiring material outputs.
	uint32 bUseAsOccluder : 1;			// Whether it can be used in renderpasses only depending on the raw geometry (i.e. Depth Prepass).
	uint32 bWireframe : 1;
};