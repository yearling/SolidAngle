#include "SStaticMesh.h"
#include "YRawMesh.h"
#include "YMeshCommon.h"
#include "YStaticMeshRenderData.h"
#include "Canvas.h"
#include "Json.h"
#include "SObjectManager.h"
#include "SMaterial.h"
#include "YIntersection.h"
DEFINE_LOG_CATEGORY(LogYStaticMesh);

SStaticMesh::SStaticMesh()
{
}

SStaticMesh::~SStaticMesh()
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

FArchive& SStaticMesh::Serialize(FArchive& Ar)
{
	if (Ar.IsLoading())
	{
		RenderData = MakeUnique<YStaticMeshRenderData>();
	}
	RenderData->Serialize(Ar);
	Ar << ExtendedBounds;
	return Ar;
}

void SStaticMesh::Render(TSharedRef<YRenderInfo> RenderInfo)
{

}

void SStaticMesh::Build()
{
	if (SourceModels.Num() < 0)
	{
		return;
	}

	if (SourceModels.Num() > MAX_YSTATIC_MESH_LODS)
	{
		UE_LOG(LogYStaticMesh, Warning, TEXT("Cannot build LOD %d.  The maximum allowed is %d.  Skipping."), SourceModels.Num(), MAX_YSTATIC_MESH_LODS);

	}

	RenderData = MakeUnique<YStaticMeshRenderData>();


}

void SStaticMesh::InitRenderResource()
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
	return (LHS.Material == RHS.Material &&
		LHS.MaterialSlotName == RHS.MaterialSlotName
		&& LHS.ImportedMaterialSlotName == RHS.ImportedMaterialSlotName
		);
}

bool operator== (const YStaticMaterial& LHS, const YMaterialInterface& RHS)
{
	return (LHS.Material == &RHS);
}

bool operator== (const YMaterialInterface& LHS, const YStaticMaterial& RHS)
{
	return (RHS.Material == &LHS);
}

void SStaticMesh::DebugTangent()
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
		uint32 FirstIndex = MeshSection.FirstIndex;
		for (uint32 TriangleIndex = 0; TriangleIndex < MeshSection.NumTriangles; ++TriangleIndex)
		{
			//DrawPosition
			for (int32 i : {0, 1, 2})
			{
				uint32 WedgeIndex = FirstIndex + TriangleIndex * 3 + i;
				uint32 PositionIndex = IndexArrayView[WedgeIndex];
				FVector VertexPosition = LOD.PositionVertexBuffer.VertexPosition(PositionIndex);
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

bool SStaticMesh::LoadFromPackage(const FString & Path)
{
	if (FPaths::GetExtension(Path).Equals(TEXT("json"), ESearchCase::IgnoreCase))
	{
		FString JsonTxt;
		bool bLoad = FFileHelper::LoadFileToString(JsonTxt, *Path);
		if (!bLoad)
		{
			UE_LOG(LogYStaticMesh, Log, TEXT("file: %s not exist"), *Path);
			return false;
		}
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonTxt);
		TSharedPtr<FJsonObject> RootObject;
		bool bSuccessful = FJsonSerializer::Deserialize(JsonReader, RootObject);
		if (!bSuccessful)
		{
			FString ErrMsg = JsonReader->GetErrorMessage();
			UE_LOG(LogYStaticMesh, Log, TEXT("Json parse failed! \n %s : %s"), *Path, *ErrMsg);
			return false;
		}

		auto Materials = RootObject->Values.Find(TEXT("Materials"));
		if (Materials && (*Materials)->Type == EJson::Array)
		{
			auto& MaterialArray = (*Materials)->AsArray();
			StaticMaterials.AddZeroed(MaterialArray.Num());
			for (int32 i = 0; i < MaterialArray.Num(); ++i)
			{
				auto& MaterialItem = MaterialArray[i]->AsObject();
				int32 MaterialIndex = (int32)MaterialItem->Values["MaterialIndex"]->AsNumber();
				FName SlotUIName = *(MaterialItem->Values["SlotUIName"]->AsString());
				FString MaterialPackage = MaterialItem->Values["MaterialPackage"]->AsString();
				assert(MaterialIndex < MaterialArray.Num());
				YStaticMaterial & StaticMeshMaterial = StaticMaterials[MaterialIndex];
				StaticMeshMaterial.MaterialSlotName = SlotUIName;
				StaticMeshMaterial.ImportedMaterialSlotName = SlotUIName;
				StaticMeshMaterial.Material = SObjectManager::ConstructUnifyFromPackage<SMaterial>(MaterialPackage);
			}
		}

		SectionInfoMap.Clear();
		auto ParametersLODs = RootObject->Values.Find(TEXT("LODs"));
		if (ParametersLODs && (*ParametersLODs)->Type == EJson::Array)
		{
			auto& LODArray = (*ParametersLODs)->AsArray();
			for (int32 LODIndex = 0; LODIndex < LODArray.Num(); ++LODIndex)
			{
				auto& LODItem = LODArray[LODIndex]->AsObject();
				{
					int32 LOD = (int32)LODItem->Values["LOD"]->AsNumber();
					auto& SectionInfoArray = LODItem->Values["SectionInfo"]->AsArray();
					for (int32 SectionIndex = 0; SectionIndex < SectionInfoArray.Num(); ++SectionIndex)
					{
						auto SectionItem = SectionInfoArray[SectionIndex]->AsObject();
						int32 SectionNum = SectionItem->Values["SectionIndex"]->AsNumber();
						int32 MaterialIndex = SectionItem->Values["MaterialIndex"]->AsNumber();
						SectionInfoMap.Set(LOD, SectionNum, YMeshSectionInfo(MaterialIndex));
					}
				}
			}
		}

		FString AssetPatchPath = RootObject->Values["AssetPackage"]->AsString();
		TUniquePtr<FArchive> FileReader(IFileManager::Get().CreateFileReader(*AssetPatchPath));
		if (FileReader)
		{
			Serialize(*FileReader);
		}
		else
		{
			UE_LOG(LogYStaticMesh, Log, TEXT("Can not open file: %s "), *AssetPatchPath);
			return false;
		}
	}
	return true;
}

FBoxSphereBounds SStaticMesh::GetBounds() const
{
	return ExtendedBounds;
}

bool SStaticMesh::RayCast(const YRay & Ray, TArray<YRayCastElement>& OutResult, bool bIsCull/*=true*/)
{
	if (!RenderData)
	{
		return false;
	}
	OutResult.Empty();
	//LOD 0
	check(RenderData->LODResources.Num() >= 1);
	YStaticMeshLODResources& LOD = RenderData->LODResources[0];
	bool bHit = false;
	for (YStaticMeshSection& MeshSection : LOD.Sections)
	{
		YIndexArrayView IndexArrayView = LOD.IndexBuffer.GetArrayView();
		uint32 FirstIndex = MeshSection.FirstIndex;
		for (uint32 TriangleIndex = 0; TriangleIndex < MeshSection.NumTriangles; ++TriangleIndex)
		{
			//DrawPosition
			FVector VertexPosition[3];
			for (int32 i : {0, 1, 2})
			{
				uint32 WedgeIndex = FirstIndex + TriangleIndex * 3 + i;
				uint32 PositionIndex = IndexArrayView[WedgeIndex];
				VertexPosition[i] = LOD.PositionVertexBuffer.VertexPosition(PositionIndex);
			}
			float t, u, v, w;
			bool bBackFace = false;
			if (RayCastTriangleReturnBackFaceResult(Ray, VertexPosition[0], VertexPosition[1], VertexPosition[2], t, u, v, w, bBackFace, true))
			{
				YRayCastElement& NewResult = * new(OutResult) YRayCastElement();
				FVector TangentX[3];
				FVector TangentY[3];
				FVector TangentZ[3];
				FVector2D UVs[2][3];
				bHit = true;
				for (int32 i : {0, 1, 2})
				{
					uint32 WedgeIndex = FirstIndex + TriangleIndex * 3 + i;
					uint32 PositionIndex = IndexArrayView[WedgeIndex];
					VertexPosition[i] = LOD.PositionVertexBuffer.VertexPosition(PositionIndex);
					TangentX[i] = LOD.VertexBuffer.VertexTangentX(PositionIndex);
					TangentY[i] = LOD.VertexBuffer.VertexTangentY(PositionIndex);
					TangentZ[i] = LOD.VertexBuffer.VertexTangentZ(PositionIndex);
					UVs[0][i] = LOD.VertexBuffer.GetVertexUV(PositionIndex, 0);
					UVs[1][i] = LOD.VertexBuffer.GetVertexUV(PositionIndex, 1);
				}
				NewResult.TrianglePoints[0] = VertexPosition[0];
				NewResult.TrianglePoints[1] = VertexPosition[1];
				NewResult.TrianglePoints[2] = VertexPosition[2];
				NewResult.RaycastPoint = VertexPosition[0] * w + VertexPosition[1] * u + VertexPosition[2] * v;
				NewResult.bBackFace = bBackFace;
				NewResult.TangentX = TangentX[0] * w + TangentX[1] * u + TangentX[2] * v;
				NewResult.TangentY = TangentY[0] * w + TangentY[1] * u + TangentY[2] * v;
				NewResult.TangentZ = TangentZ[0] * w + TangentZ[1] * u + TangentZ[2] * v;
				NewResult.TangentX.Normalize();
				NewResult.TangentY.Normalize();
				NewResult.TangentZ.Normalize();
				NewResult.UVs[0] = UVs[0][0] * w + UVs[0][1] * u + UVs[0][2] * v;
				NewResult.UVs[1] = UVs[1][0] * w + UVs[1][1] * u + UVs[1][2] * v;
				NewResult.t = t;
				NewResult.u = u;
				NewResult.v = v;
				NewResult.w = w;
			}
		}
	}
	return bHit;
}
