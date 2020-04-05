#include "YStaticMesh.h"
#include "YRawMesh.h"
#include "YMeshCommon.h"
#include "YStaticMeshRenderData.h"
#include "Canvas.h"
DEFINE_LOG_CATEGORY(LogYStaticMesh);

YStaticMesh::YStaticMesh()
{
}

YStaticMesh::~YStaticMesh()
{
	if (RenderData)
	{
		for (YStaticMeshLODResources& LOD : RenderData->LODResources)
		{
			LOD.PositionVertexBuffer.ReleaseRHI();
			LOD.VertexBuffer.ReleaseRHI();
			LOD.IndexBuffer.ReleaseRHI();
			LOD.DepthOnlyIndexBuffer.ReleaseRHI();
		}
	}
}

void YStaticMesh::Serialize(FArchive& Ar)
{
	if (Ar.IsLoading())
	{
		RenderData = MakeUnique<YStaticMeshRenderData>();
	}
	RenderData->Serialize(Ar);
	Ar << ExtendedBounds;
}

void YStaticMesh::Render(TSharedRef<YRenderInfo> RenderInfo)
{

}

void YStaticMesh::Build()
{
	if (SourceModels.Num() < 0)
	{
		return;
	}

	if (SourceModels.Num() > MAX_YSTATIC_MESH_LODS)
	{
		UE_LOG(LogYStaticMesh, Warning, TEXT("Cannot build LOD %d.  The maximum allowed is %d.  Skipping."),SourceModels.Num(),MAX_YSTATIC_MESH_LODS );

	}
	
	RenderData = MakeUnique<YStaticMeshRenderData>();


}

void YStaticMesh::InitRenderResource()
{
	check(RenderData);
	if (RenderData)
	{
		for (int LodIndex = 0; LodIndex < RenderData->LODResources.Num(); ++LodIndex)
		{
			YStaticMeshLODResources& LOD = RenderData->LODResources[LodIndex];
			LOD.PositionVertexBuffer.InitRHI();
			LOD.VertexBuffer.InitRHI();
			LOD.IndexBuffer.InitRHI();
			LOD.DepthOnlyIndexBuffer.InitRHI();
		}
	}
}

bool operator==(const YMeshSectionInfo& A, const YMeshSectionInfo& B)
{
	return A.MaterialIndex == B.MaterialIndex
		&& A.bCastShadow == B.bCastShadow
		&& A.bEnableCollision == B.bEnableCollision;
}

bool operator!=(const YMeshSectionInfo& A, const YMeshSectionInfo& B)
{
	return !(A == B);
}

static uint32 GetMeshMaterialKey(int32 LODIndex, int32 SectionIndex)
{
	return ((LODIndex & 0xffff) << 16) | (SectionIndex & 0xffff);
}

void YMeshSectionInfoMap::Clear()
{
	Map.Empty();
}

int32 YMeshSectionInfoMap::GetSectionNumber(int32 LODIndex) const
{
	int32 SectionCount = 0;
	for (auto& Kvp : Map)
	{
		if (((Kvp.Key & 0xffff) >> 16) == LODIndex)
		{
			++SectionCount;
		}
	}
	return SectionCount;
}

YMeshSectionInfo YMeshSectionInfoMap::Get(int32 LODIndex, int32 SectionIndex) const
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	const YMeshSectionInfo* InfoPtr = Map.Find(Key);
	if (InfoPtr == nullptr)
	{
		Key = GetMeshMaterialKey(0, SectionIndex);
		InfoPtr = Map.Find(Key);
	}
	if (InfoPtr != nullptr)
	{
		return *InfoPtr;
	}
	return YMeshSectionInfo(SectionIndex);
}

void YMeshSectionInfoMap::Set(int32 LODIndex, int32 SectionIndex, const YMeshSectionInfo& Info)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.Add(Key, Info);
}

void YMeshSectionInfoMap::Remove(int32 LODIndex, int32 SectionIndex)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.Remove(Key);
}

void YMeshSectionInfoMap::CopyFrom(const YMeshSectionInfoMap& Other)
{
	for (TMap<uint32, YMeshSectionInfo>::TConstIterator It(Other.Map); It; ++It)
	{
		Map.Add(It.Key(), It.Value());
	}
}

bool YMeshSectionInfoMap::AnySectionHasCollision() const
{
	for (TMap<uint32, YMeshSectionInfo>::TConstIterator It(Map); It; ++It)
	{
		uint32 Key = It.Key();
		int32 LODIndex = (int32)(Key >> 16);
		if (LODIndex == 0 && It.Value().bEnableCollision)
		{
			return true;
		}
	}
	return false;
}
FArchive& operator<<(FArchive& Ar, YMeshSectionInfo& Info)
{
	Ar << Info.MaterialIndex;
	Ar << Info.bEnableCollision;
	Ar << Info.bCastShadow;
	return Ar;
}
void YMeshSectionInfoMap::Serialize(FArchive& Ar)
{
	Ar << Map;
}

YStaticMeshSourceModel::YStaticMeshSourceModel()
{
	RawMeshBulkData = new YRawMeshBulkData();
	ScreenSize = 0.0f;
}

YStaticMeshSourceModel::~YStaticMeshSourceModel()
{
	if (RawMeshBulkData)
	{
		delete RawMeshBulkData;
		RawMeshBulkData = NULL;
	}
}

void YStaticMeshSourceModel::SerializeBulkData(FArchive& Ar, UObject* Owner)
{
	RawMeshBulkData->Serialize(Ar);
}

FArchive& operator<<(FArchive& Ar, YStaticMaterial& Elem)
{
	Ar << Elem.MaterialSlotName;
	if (!Ar.IsCooking()/* || Ar.CookingTarget()->HasEditorOnlyData()*/)
	{
		Ar << Elem.ImportedMaterialSlotName;
	}
	return Ar;
}

bool operator== (const YStaticMaterial& LHS, const YStaticMaterial& RHS)
{
	return (LHS.MaterialInterface == RHS.MaterialInterface &&
		LHS.MaterialSlotName == RHS.MaterialSlotName
		&& LHS.ImportedMaterialSlotName == RHS.ImportedMaterialSlotName
		);
}

bool operator== (const YStaticMaterial& LHS, const YMaterialInterface& RHS)
{
	return (LHS.MaterialInterface == &RHS);
}

bool operator== (const YMaterialInterface& LHS, const YStaticMaterial& RHS)
{
	return (RHS.MaterialInterface == &LHS);
}

void YStaticMesh::DebugTangent()
{
	if (!RenderData)
	{
		return;
	}
	//LOD 0

	check(RenderData->LODResources.Num() >= 1);
	YStaticMeshLODResources& LOD = RenderData->LODResources[0];
	for (YStaticMeshSection& MeshSection : LOD.Sections)
	{
		YIndexArrayView IndexArrayView = LOD.IndexBuffer.GetArrayView();
		//LOD.PositionVertexBuffer.VertexPosition()
		//LOD.VertexBuffer.GetVertexUV()
		//LOD.VertexBuffer.VertexTangentX();
		uint32 FirstIndex = MeshSection.FirstIndex;
		for (uint32 TriangleIndex = 0; TriangleIndex < MeshSection.NumTriangles; ++TriangleIndex)
		{
			//DrawPosition
			for (int32 i : {0, 1, 2})
			{
				uint32 WedgeIndex = FirstIndex + TriangleIndex * 3+i;
				uint32 PositionIndex = IndexArrayView[WedgeIndex];
				FVector VertexPosition = LOD.PositionVertexBuffer.VertexPosition(PositionIndex);
				//GCanvas->DrawBall(VertexPosition, FLinearColor::Green, 0.2);
				// Draw Normal
				FVector TangentX = LOD.VertexBuffer.VertexTangentX(PositionIndex);
				FVector TangentY = LOD.VertexBuffer.VertexTangentY(PositionIndex);
				FVector TangentZ = LOD.VertexBuffer.VertexTangentZ(PositionIndex);
				FMatrix TransTangentToLocal(TangentX, TangentY, TangentZ, VertexPosition);
				FVector OriginPosition = TransTangentToLocal.TransformPosition(FVector(0, 0, 0));
				FVector TangentPosition = TransTangentToLocal.TransformPosition(FVector(1, 0, 0));
				FVector BiTangentPosition = TransTangentToLocal.TransformPosition(FVector(0, 1, 0));
				FVector NormalPosition = TransTangentToLocal.TransformPosition(FVector(0, 0, 1));
				GCanvas->DrawLine(OriginPosition, TangentPosition, FLinearColor::Red);
				GCanvas->DrawLine(OriginPosition, BiTangentPosition, FLinearColor::Green);
				GCanvas->DrawLine(OriginPosition, NormalPosition, FLinearColor::Blue);
			}


		}
	}
}


