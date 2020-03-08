#include "YMeshUtilities.h"
#include "YMeshCommon.h"
#include "YRawMesh.h"
#include "YStaticMesh.h"
#include "LayoutUV.h"
#include "YStaticMeshResource.h"
#include "NvTriStrip.h"
#include "forsythtriangleorderoptimizer.h"

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

static inline FVector GetPositionForWedge(YRawMesh const& Mesh, int32 WedgeIndex)
{
	int32 VertexIndex = Mesh.WedgeIndices[WedgeIndex];
	return Mesh.VertexPositions[VertexIndex];
}

static YStaticMeshBuildVertex BuildStaticMeshVertex(YRawMesh const& RawMesh, int32 WedgeIndex, FVector BuildScale)
{
	YStaticMeshBuildVertex Vertex;
	Vertex.Position = GetPositionForWedge(RawMesh, WedgeIndex) * BuildScale;

	const FMatrix ScaleMatrix = FScaleMatrix(BuildScale).Inverse().GetTransposed();
	Vertex.TangentX = ScaleMatrix.TransformVector(RawMesh.WedgeTangentX[WedgeIndex]).GetSafeNormal();
	Vertex.TangentY = ScaleMatrix.TransformVector(RawMesh.WedgeTangentY[WedgeIndex]).GetSafeNormal();
	Vertex.TangentZ = ScaleMatrix.TransformVector(RawMesh.WedgeTangentZ[WedgeIndex]).GetSafeNormal();

	if (RawMesh.WedgeColors.IsValidIndex(WedgeIndex))
	{
		Vertex.Color = RawMesh.WedgeColors[WedgeIndex];
	}
	else
	{
		Vertex.Color = FColor::White;
	}

	int32 NumTexCoords = FMath::Min<int32>(MAX_MESH_TEXTURE_COORDS, MAX_MESH_TEXTURE_COORDS);
	for (int32 i = 0; i < NumTexCoords; ++i)
	{
		if (RawMesh.WedgeTexCoords[i].IsValidIndex(WedgeIndex))
		{
			Vertex.UVs[i] = RawMesh.WedgeTexCoords[i][WedgeIndex];
		}
		else
		{
			Vertex.UVs[i] = FVector2D(0.0f, 0.0f);
		}
	}
	return Vertex;
}

static bool AreVerticesEqual(
	YStaticMeshBuildVertex const& A,
	YStaticMeshBuildVertex const& B,
	float ComparisonThreshold
)
{
	if (!PointsEqual(A.Position, B.Position, ComparisonThreshold)
		|| !NormalsEqual(A.TangentX, B.TangentX)
		|| !NormalsEqual(A.TangentY, B.TangentY)
		|| !NormalsEqual(A.TangentZ, B.TangentZ)
		|| A.Color != B.Color)
	{
		return false;
	}

	// UVs
	for (int32 UVIndex = 0; UVIndex < MAX_MESH_TEXTURE_COORDS; UVIndex++)
	{
		if (!UVsEqual(A.UVs[UVIndex], B.UVs[UVIndex]))
		{
			return false;
		}
	}

	return true;
}
// OutVertices是合并后的顶点，
// OutPerSectionIndices: 合并后的Index
void YMeshUtilities::BuildStaticMeshVertexAndIndexBuffers(TArray<YStaticMeshBuildVertex>& OutVertices, 
	TArray<TArray<uint32> >& OutPerSectionIndices, 
	TArray<int32>& OutWedgeMap,
	const YRawMesh& RawMesh, const TMap<uint32, uint32>& MaterialToSectionMapping, float ComparisonThreshold)
{
	TMultiMap<int32, int32> OverlappingCornersMap;
	YMeshUtilities::FindOverlippingCorners(OverlappingCornersMap, RawMesh.VertexPositions, RawMesh.WedgeIndices, ComparisonThreshold);
	int32 NumFace = RawMesh.WedgeIndices.Num() / 3;
	TMap<int32, int32> FinalVerts;
	for (int32 FaceIndex = 0; FaceIndex < NumFace; ++FaceIndex)
	{
		FVector WedgePosition[3];
		int32 VertexIndices[3];
		for (int32 CornerIndex : {0, 1, 2})
		{
			int32 WedgeIndex = FaceIndex * 3 + CornerIndex;
			WedgePosition[CornerIndex] = GetPositionForWedge(RawMesh,WedgeIndex);
		}
		//degenerate triangle
		if (PointsEqual(WedgePosition[0], WedgePosition[1], ComparisonThreshold) ||
			PointsEqual(WedgePosition[0], WedgePosition[2], ComparisonThreshold) ||
			PointsEqual(WedgePosition[1], WedgePosition[2], ComparisonThreshold))
		{
			for (int32 CornerIndex : {0, 1, 2})
			{
				OutWedgeMap.Add(INDEX_NONE);
			}
			continue;
		}
		for (int32 CornerIndex : {0, 1, 2})
		{
			int32 WedgeIndex = FaceIndex * 3 + CornerIndex;
			TArray<int32> OverlappingCorners;
			OverlappingCornersMap.MultiFind(WedgeIndex, OverlappingCorners);
			OverlappingCorners.Sort();
			YStaticMeshBuildVertex BuildVertex = BuildStaticMeshVertex(RawMesh, WedgeIndex, FVector(1.0, 1.0, 1.0));
			int32 Index = INDEX_NONE;
			for (int32 OverlappingCornerIndex : OverlappingCorners)
			{
				if (OverlappingCornerIndex > WedgeIndex)
				{
					break;
				}
				int32* FindLocation = FinalVerts.Find(OverlappingCornerIndex);
				if (FindLocation && AreVerticesEqual(BuildVertex, OutVertices[*FindLocation], ComparisonThreshold))
				{
					Index = *FindLocation;
					break;
				}
			}
			if (Index == INDEX_NONE)
			{
				Index = OutVertices.Add(BuildVertex);
				FinalVerts.Add(WedgeIndex, Index);
			}
			VertexIndices[CornerIndex] = Index;
		}
		if (VertexIndices[0] == VertexIndices[1] || VertexIndices[0] == VertexIndices[2] || VertexIndices[1] == VertexIndices[2])
		{
			for (int32 CornerIndex : {0, 1, 2})
			{
				OutWedgeMap.Add(INDEX_NONE);
			}
			continue;
		}
		uint32 SectionIndex = MaterialToSectionMapping.FindChecked(RawMesh.FaceMaterialIndices[FaceIndex]);
		TArray<uint32> &SectionIndices = OutPerSectionIndices[SectionIndex];
		for (int32 CornerIndex : {0, 1, 2})
		{
			SectionIndices.Add(VertexIndices[CornerIndex]);
			OutWedgeMap.Add(VertexIndices[CornerIndex]);
		}
	}
}
/*------------------------------------------------------------------------------
NVTriStrip for cache optimizing index buffers.
------------------------------------------------------------------------------*/

namespace NvTriStrip
{
	/**
	* Converts 16 bit indices to 32 bit prior to passing them into the real GenerateStrips util method
	*/
	void GenerateStrips(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		PrimitiveGroup** PrimGroups,
		uint32* NumGroups
	)
	{
		if (Is32Bit)
		{
			GenerateStrips((uint32*)Indices, NumIndices, PrimGroups, NumGroups);
		}
		else
		{
			// convert to 32 bit
			uint32 Idx;
			TArray<uint32> NewIndices;
			NewIndices.AddUninitialized(NumIndices);
			for (Idx = 0; Idx < NumIndices; ++Idx)
			{
				NewIndices[Idx] = ((uint16*)Indices)[Idx];
			}
			GenerateStrips(NewIndices.GetData(), NumIndices, PrimGroups, NumGroups);
		}

	}

	/**
	* Orders a triangle list for better vertex cache coherency.
	*
	* *** WARNING: This is safe to call for multiple threads IF AND ONLY IF all
	* threads call SetListsOnly(true) and SetCacheSize(CACHESIZE_GEFORCE3). If
	* NvTriStrip is ever used with different settings the library will need
	* some modifications to be thread-safe. ***
	*/
	template<typename IndexDataType, typename Allocator>
	void CacheOptimizeIndexBuffer(TArray<IndexDataType, Allocator>& Indices)
	{
		static_assert(sizeof(IndexDataType) == 2 || sizeof(IndexDataType) == 4, "Indices must be short or int.");

		PrimitiveGroup*	PrimitiveGroups = NULL;
		uint32			NumPrimitiveGroups = 0;
		bool Is32Bit = sizeof(IndexDataType) == 4;

		SetListsOnly(true);
		SetCacheSize(CACHESIZE_GEFORCE3);

		GenerateStrips((uint8*)Indices.GetData(), Is32Bit, Indices.Num(), &PrimitiveGroups, &NumPrimitiveGroups);

		Indices.Empty();
		Indices.AddUninitialized(PrimitiveGroups->numIndices);

		if (Is32Bit)
		{
			FMemory::Memcpy(Indices.GetData(), PrimitiveGroups->indices, Indices.Num() * sizeof(IndexDataType));
		}
		else
		{
			for (uint32 I = 0; I < PrimitiveGroups->numIndices; ++I)
			{
				Indices[I] = (uint16)PrimitiveGroups->indices[I];
			}
		}

		delete[] PrimitiveGroups;
	}
}

namespace Forsyth
{
	/**
	* Converts 16 bit indices to 32 bit prior to passing them into the real OptimizeFaces util method
	*/
	void OptimizeFaces(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		uint32 NumVertices,
		uint32* OutIndices,
		uint16 CacheSize
	)
	{
		if (Is32Bit)
		{
			OptimizeFaces((uint32*)Indices, NumIndices, NumVertices, OutIndices, CacheSize);
		}
		else
		{
			// convert to 32 bit
			uint32 Idx;
			TArray<uint32> NewIndices;
			NewIndices.AddUninitialized(NumIndices);
			for (Idx = 0; Idx < NumIndices; ++Idx)
			{
				NewIndices[Idx] = ((uint16*)Indices)[Idx];
			}
			OptimizeFaces(NewIndices.GetData(), NumIndices, NumVertices, OutIndices, CacheSize);
		}

	}
	//
		/**
		* Orders a triangle list for better vertex cache coherency.
		*/
	template<typename IndexDataType, typename Allocator>
	void CacheOptimizeIndexBuffer(TArray<IndexDataType, Allocator>& Indices)
	{
		static_assert(sizeof(IndexDataType) == 2 || sizeof(IndexDataType) == 4, "Indices must be short or int.");
		bool Is32Bit = sizeof(IndexDataType) == 4;

		// Count the number of vertices
		uint32 NumVertices = 0;
		for (int32 Index = 0; Index < Indices.Num(); ++Index)
		{
			if (Indices[Index] > NumVertices)
			{
				NumVertices = Indices[Index];
			}
		}
		NumVertices += 1;

		TArray<uint32> OptimizedIndices;
		OptimizedIndices.AddUninitialized(Indices.Num());
		uint16 CacheSize = 32;
		OptimizeFaces((uint8*)Indices.GetData(), Is32Bit, Indices.Num(), NumVertices, OptimizedIndices.GetData(), CacheSize);

		if (Is32Bit)
		{
			FMemory::Memcpy(Indices.GetData(), OptimizedIndices.GetData(), Indices.Num() * sizeof(IndexDataType));
		}
		else
		{
			for (int32 I = 0; I < OptimizedIndices.Num(); ++I)
			{
				Indices[I] = (uint16)OptimizedIndices[I];
			}
		}
	}
}


void YMeshUtilities::CacheOptimizeVertexAndIndexBuffer(TArray<YStaticMeshBuildVertex>& Vertices, TArray<TArray<uint32> >& PerSectionIndices, TArray<int32>& WedgeMap)
{
	//Copy the vertices since we will be reordering them
	TArray<YStaticMeshBuildVertex> OriginalVertices = Vertices;

	TArray<int32> IndexCache;
	IndexCache.AddUninitialized(Vertices.Num());
	FMemory::Memset(IndexCache.GetData(), INDEX_NONE, IndexCache.Num()*IndexCache.GetTypeSize());
	// Iterate through the section index buffers, 
	// Optimizing index order for the post transform cache (minimizes the number of vertices transformed), 
	// And vertex order for the pre transform cache (minimizes the amount of vertex data fetched by the GPU).
	int32 NextAvailableIndex = 0;
	for (int32 SectionIndex = 0; SectionIndex < PerSectionIndices.Num(); ++SectionIndex)
	{
		TArray<uint32>& Indices = PerSectionIndices[SectionIndex];
		if (Indices.Num())
		{
			YMeshUtilities::CacheOptimizeIndexBuffer(Indices);
			TArray<uint32> OriginIndices = Indices;
			for (int32 Index = 0; Index < Indices.Num(); ++Index)
			{
				const int32 CacheIndex = IndexCache[OriginIndices[Index]];
				if (CacheIndex == INDEX_NONE)
				{
					Indices[Index] = NextAvailableIndex;
					IndexCache[OriginIndices[Index]] = NextAvailableIndex;
					NextAvailableIndex++;
				}
				else
				{
					Indices[Index] = CacheIndex;
				}
				Vertices[Indices[Index]] = OriginalVertices[OriginIndices[Index]];
			}
		}
	}
	for (int32 i = 0; i < WedgeMap.Num(); ++i)
	{
		int32 MappedIndex = WedgeMap[i];
		if (MappedIndex != 0)
		{
			WedgeMap[i] = IndexCache[MappedIndex];
		}
	}
}
#define YY_USE_NVSTRIP 1
void YMeshUtilities::CacheOptimizeIndexBuffer(TArray<uint16>& Indices)
{
	if (YY_USE_NVSTRIP)
	{
		NvTriStrip::CacheOptimizeIndexBuffer(Indices);
	}
	else
	{
		Forsyth::CacheOptimizeIndexBuffer(Indices);
	}
}

void YMeshUtilities::CacheOptimizeIndexBuffer(TArray<uint32>& Indices)
{
	if (YY_USE_NVSTRIP)
	{
		NvTriStrip::CacheOptimizeIndexBuffer(Indices);
	}
	else
	{
		Forsyth::CacheOptimizeIndexBuffer(Indices);
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
					else
					{
						YMeshUtilities::ComputeTangents(RawMesh.VertexPositions, RawMesh.WedgeIndices, RawMesh.WedgeTexCoords[0], RawMesh.FaceSmoothingMasks, RawMesh.WedgeTangentX, RawMesh.WedgeTangentY, RawMesh.WedgeTangentZ, TangentOptions);
					}
				}
				// At this point the mesh will have valid tangents.
				check(RawMesh.WedgeTangentX.Num() == NumWedges);
				check(RawMesh.WedgeTangentY.Num() == NumWedges);
				check(RawMesh.WedgeTangentZ.Num() == NumWedges);

				// Generate lightmap UVs
				if (SrcModel.BuildSettings.bGenerateLightmapUVs)
				{
					if (RawMesh.WedgeTexCoords[SrcModel.BuildSettings.SrcLightmapIndex].Num() == 0)
					{
						SrcModel.BuildSettings.SrcLightmapIndex = 0;
					}

					YLayoutUV Packer(&RawMesh, SrcModel.BuildSettings.SrcLightmapIndex, SrcModel.BuildSettings.DstLightmapIndex, SrcModel.BuildSettings.MinLightmapResolution);
					TMultiMap<int32, int32> OverlappingCorners;
					YMeshUtilities::FindOverlippingCorners(OverlappingCorners,RawMesh.VertexPositions, RawMesh.WedgeIndices, ComparisonThreshold);
					Packer.FindCharts(OverlappingCorners);
					bool bPackSuccess = Packer.FindBestPacking();
					if (bPackSuccess)
					{
						Packer.CommitPackedUVs();
					}
				}
			}
		}
		if (LODMeshes.Num() == 0 || LODMeshes[0].WedgeIndices.Num() == 0)
		{
			return false;
		}
		for (int32 LODIndex = 0; LODIndex < SourceModels.Num(); ++LODIndex)
		{
			if (LODMeshes[NumValidLODs].WedgeIndices.Num() > 0)
			{
				NumValidLODs++;
			}
		}
		Stage = EStage::Gathered;
		return true;
	}
	
	bool GenerateRenderingMeshes(YStaticMeshRenderData& OutRenderData, TArray<YStaticMeshSourceModel>& InOutModels)
	{
		OutRenderData.AllocateLODResources(NumValidLODs);
		for (int32 LODIndex = 0; LODIndex < NumValidLODs; ++LODIndex)
		{
			YStaticMeshLODResources& LODModel = OutRenderData.LODResources[LODIndex];
			YRawMesh& RawMesh = LODMeshes[LODIndex];
			TArray<int32> MaterialIndices;
			for (const int32 MaterialIndex : RawMesh.FaceMaterialIndices)
			{
				// Find all unique material indices
				MaterialIndices.AddUnique(MaterialIndex);
			}
			TMap<uint32, uint32> MaterialToSectionMapping;
			TArray<TArray<uint32> > PerSectionIndices;
			for (int32 Index = 0; Index < MaterialIndices.Num(); ++Index)
			{
				const int32 MaterialIndex = MaterialIndices[Index];
				YStaticMeshSection* Section = new(LODModel.Sections) YStaticMeshSection();
				Section->MaterialIndex = MaterialIndex;
				MaterialToSectionMapping.Add(MaterialIndex, Index);
				new(PerSectionIndices)TArray<uint32>;
			}
			// Build and cache optimize vertex and index buffers.
			TArray<YStaticMeshBuildVertex> Vertices;
			{
				TArray<int32>& WedgeMap = OutRenderData.WedgeMap;
				float ComparisonThreshold = GetComparisonThreshold(InOutModels[0].BuildSettings.bRemoveDegenerates);
				YMeshUtilities::BuildStaticMeshVertexAndIndexBuffers(Vertices, PerSectionIndices, WedgeMap, RawMesh, MaterialToSectionMapping, ComparisonThreshold);
				check(WedgeMap.Num() == RawMesh.WedgeIndices.Num());
				if (RawMesh.WedgeIndices.Num() < 100000 * 3)
				{
					YMeshUtilities::CacheOptimizeVertexAndIndexBuffer(Vertices, PerSectionIndices, WedgeMap);
					check(WedgeMap.Num() == RawMesh.WedgeIndices.Num());
				}
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

	int32 NumValidLODs=0;

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

