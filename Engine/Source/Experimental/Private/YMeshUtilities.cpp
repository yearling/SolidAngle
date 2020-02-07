#include "YMeshUtilities.h"
#include "YMeshCommon.h"
#include "YRawMesh.h"
#include "YStaticMesh.h"

DEFINE_LOG_CATEGORY(LogYMeshUtilities);
struct YIndexAndZ
{
	float Z;
	int32 Index;
	YIndexAndZ() {}
	YIndexAndZ(int32 InIndex, FVector V)
	{
		Z = 0.30f * V.X + 0.33f *V.Y + 0.37f *V.Z;
		Index = InIndex;
	}
};
struct YCompareIndexAndZ
{
	FORCEINLINE bool operator()(const YIndexAndZ& A, const YIndexAndZ&B) const {return A.Z < B.Z;}
};


static float GetComparisonThreshold(bool bRemoveDegenerates)
{
	return bRemoveDegenerates ? THRESH_POINTS_ARE_SAME : 0.0f;
}

void YMeshUtilities::FindOverlippingCorners(TMultiMap<int32, int32> &outOverlappingCorners, const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, float ComparisionThreshold)
{
	// 注意：不包含自己对自己,在后面使用最紧邻查找时要把自己先加进去
	const int32 NumWedges = InIndices.Num();
	//Create a list of vertex  Z/index pairs
	TArray<YIndexAndZ> VertexIndexAndZ;
	VertexIndexAndZ.Reserve(NumWedges);
	for (int32 WedgeIndex = 0; WedgeIndex < NumWedges; ++WedgeIndex)
	{
		VertexIndexAndZ.Add(YIndexAndZ(WedgeIndex, InVertices[InIndices[WedgeIndex]]));
	}
	// Sort the vertices by z value
	VertexIndexAndZ.Sort(YCompareIndexAndZ());
	// Search for duplicates,quickly!
	for (int32 i = 0; i < VertexIndexAndZ.Num(); ++i)
	{
		// only need to search forward,since we add pairs both ways
		for (int32 j = i + 1; j < VertexIndexAndZ.Num(); ++j)
		{
			if (FMath::Abs(VertexIndexAndZ[j].Z - VertexIndexAndZ[i].Z) > ComparisionThreshold)
			{
				break; //can not be more dups
			}
			const FVector& PositionA = InVertices[InIndices[VertexIndexAndZ[i].Index]];
			const FVector& PositionB = InVertices[InIndices[VertexIndexAndZ[j].Index]];
			if (PointsEqual(PositionA, PositionB, ComparisionThreshold))
			{
				outOverlappingCorners.Add(VertexIndexAndZ[i].Index, VertexIndexAndZ[j].Index);
				outOverlappingCorners.Add(VertexIndexAndZ[j].Index, VertexIndexAndZ[i].Index);
			}
		}
	}
}
class YStaticMeshUtilityBuilder
{
public:
	YStaticMeshUtilityBuilder() :Stage(EStage::Uninit) {};
	bool GatherSourceMeshesPerLOD(TArray<YStaticMeshSourceModel> &SourceModels)
	{
		check(Stage == EStage::Uninit);
		// Gather source meshes for each LOD
		for (int32 LODIndex = 0; LODIndex < SourceModels.Num(); ++LODIndex)
		{
			YStaticMeshSourceModel& SrcModel = SourceModels[LODIndex];
			YRawMesh& RawMesh = *new(LODMeshes)YRawMesh;
			if (!SrcModel.RawMeshBulkData->IsEmpty())
			{
				SrcModel.RawMeshBulkData->LoadRawMesh(RawMesh);
				// Make sure the raw mesh is not irreparably malformed.
				if (!RawMesh.IsValidOrFixable())
				{
					UE_LOG(LogYMeshUtilities, Error, TEXT("Raw mesh is corrupt for LOD%d."), LODIndex);
					return false;
				}
				float ComparisonThreshold = GetComparisonThreshold(SrcModel.BuildSettings.bRemoveDegenerates);
				int32 NumWedges = RawMesh.WedgeIndices.Num();
				// Find overlapping corners to accelerate adjacency.
				// Figure out if we should recompute normals and tangents
				bool bRecomputeNormals = SrcModel.BuildSettings.bRecomputeNormals || RawMesh.WedgeTangentZ.Num() != NumWedges;
				bool bRecomputeTangents = SrcModel.BuildSettings.bRecomputeTangents || RawMesh.WedgeTangentX.Num() != NumWedges || RawMesh.WedgeTangentY.Num() != NumWedges;

				// Dump normals and tangents if we are recomputing them 
				if (bRecomputeTangents)
				{
					RawMesh.WedgeTangentX.Empty(NumWedges);
					RawMesh.WedgeTangentX.AddZeroed(NumWedges);
					RawMesh.WedgeTangentY.Empty(NumWedges);
					RawMesh.WedgeTangentY.AddZeroed(NumWedges);
				}

				if (bRecomputeNormals)
				{
					RawMesh.WedgeTangentZ.Empty(NumWedges);
					RawMesh.WedgeTangentZ.AddZeroed(NumWedges);
				}
				
				// Compute any missing tangents
				{
					// Static meshes always blend normals of overlapping corners.
					uint32 TangentOptions = EYTangentOptions::BlendOverlappingNormals;
					if (SrcModel.BuildSettings.bRemoveDegenerates)
					{
						// If removing degenerate triangles, ignor them when computing tangents
						TangentOptions |= EYTangentOptions::IgnoreDegenerateTriangles;
					}

					//MikkTSpace should be use only when the user want to recompute the normals or tangents otherwise should always fallback on builtin
					if (SrcModel.BuildSettings.bUseMikkTSpace && (SrcModel.BuildSettings.bRecomputeNormals || SrcModel.BuildSettings.bRecomputeTangents))
					{
						YMeshUtilities::ComputeTangents_MikkTSpace(RawMesh, TangentOptions);
					}
					uint32 ImportTangentOptions = EYTangentOptions::BlendOverlappingNormals | EYTangentOptions::IgnoreDegenerateTriangles;
					YMeshUtilities::ComputeTangents(RawMesh.VertexPositions, RawMesh.WedgeIndices, RawMesh.WedgeTexCoords[0], RawMesh.FaceSmoothingMasks, RawMesh.WedgeTangentX, RawMesh.WedgeTangentY, RawMesh.WedgeTangentZ, ImportTangentOptions);
				}
			}
		}
		return true;
	}
private:
	enum class EStage
	{
		Uninit,
		Gathered,
		Reduce,
		GenerateRendering,
		ReplaceRaw,
	};

	EStage Stage;

	int32 NumValidLODs;

	TIndirectArray<YRawMesh> LODMeshes;
	TIndirectArray<TMultiMap<int32, int32> > LODOverlappingCorners;
	float LODMaxDeviation[MAX_YSTATIC_MESH_LODS];
	//YMeshBuildSettings LODBuildSettings[MAX_YSTATIC_MESH_LODS];
	bool HasRawMesh[MAX_YSTATIC_MESH_LODS];
};
bool YMeshUtilities::BuildStaticMesh(class YStaticMeshRenderData& OutRenderData, TArray<struct YStaticMeshSourceModel>& SourceModels)
{
	YStaticMeshUtilityBuilder Builder;
	if (!Builder.GatherSourceMeshesPerLOD(SourceModels))
	{
		return false;
	}
	return true;
}


YMeshUtilities::YMeshUtilities()
{

}

