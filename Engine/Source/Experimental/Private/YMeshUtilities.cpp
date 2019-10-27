#include "YMeshUtilities.h"
#include "YMeshCommon.h"
#include "YRawMesh.h"
#include "YStaticMesh.h"

DEFINE_LOG_CATEGORY(LogYMeshUtilities);

static float GetComparisonThreshold(bool bRemoveDegenerates)
{
	return bRemoveDegenerates ? THRESH_POINTS_ARE_SAME : 0.0f;
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
			TMultiMap<int32, int32>& OverlappingCorners = *new(LODOverlappingCorners)TMultiMap<int32, int32>;
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

			}
		}
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
}

