#include "YStaticMeshRenderData.h"
#include "YMeshCommon.h"

YStaticMeshRenderData::YStaticMeshRenderData()
{

}

void YStaticMeshRenderData::AllocateLODResources(int32 NumLODs)
{
	check(LODResources.Num() == 0);
	while (LODResources.Num() < NumLODs)
	{
		new(LODResources) YStaticMeshLODResources();
	}
}

void YStaticMeshRenderData::Serialize(FArchive& Ar)
{
	LODResources.Serialize(Ar, nullptr);
	Ar << Bounds;
	for (int i = 0; i < MAX_YSTATIC_MESH_LODS; ++i)
	{
		Ar << ScreenSize[i];
	}
}

void YStaticMeshRenderData::CaculateScreenSize()
{

}

void YStaticMeshLODResources::Serialize(FArchive& Ar, UObject* Owner, int32 Idx)
{
	//bool bNeedsCPUAccess = false;
	bool bNeedsCPUAccess = true;
	Ar << Sections;
	PositionVertexBuffer.Serialize(Ar, bNeedsCPUAccess);
	VertexBuffer.Serialize(Ar, bNeedsCPUAccess);
	IndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	ReversedIndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	DepthOnlyIndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	ReversedDepthOnlyIndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	WireframeIndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	AdjacencyIndexBuffer.Serialize(Ar, bNeedsCPUAccess);
	// Needs to be done now because on cooked platform, indices are discarded after RHIInit.
	DepthOnlyNumTriangles= DepthOnlyIndexBuffer.GetNumIndices() / 3;
}


FArchive& operator<<(FArchive& Ar, YStaticMeshSection& Section)
{
	Ar << Section.MaterialIndex;
	Ar << Section.FirstIndex;
	Ar << Section.NumTriangles;
	Ar << Section.MinVertexIndex;
	Ar << Section.MaxVertexIndex;
	Ar << Section.bEnableCollision;
	Ar << Section.bCastShadow;
	return Ar;
}
