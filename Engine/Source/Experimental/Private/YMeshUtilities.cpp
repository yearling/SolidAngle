#include "YMeshUtilities.h"
#include "YMeshCommon.h"
#include "YRawMesh.h"
#include "YStaticMesh.h"
#include "LayoutUV.h"
#include "YStaticMeshResource.h"
#include "NvTriStrip.h"
#include "forsythtriangleorderoptimizer.h"
#include "nvtess.h"
#include "YMeshUtilitiesWrap.h"

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

static bool AreVerticesEqual(YStaticMeshBuildVertex const& A,YStaticMeshBuildVertex const& B,float ComparisonThreshold)
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
// OutVertices是合并后的顶点，
// OutPerSectionIndices: 合并后的Index
void YMeshUtilities::BuildStaticMeshVertexAndIndexBuffers(TArray<YStaticMeshBuildVertex>& OutVertices, TArray<TArray<uint32> >& OutPerSectionIndices, TArray<int32>& OutWedgeMap, const YRawMesh& RawMesh, const TMap<uint32, uint32>& MaterialToSectionMapping, float ComparisonThreshold)
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
		if (MappedIndex != INDEX_NONE)
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
					YMeshUtilities::FindOverlippingCorners(OverlappingCorners, RawMesh.VertexPositions, RawMesh.WedgeIndices, ComparisonThreshold);
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

	static void BuildDepthOnlyIndexBuffer(
		TArray<uint32>& OutDepthIndices,
		const TArray<YStaticMeshBuildVertex>& InVertices,
		const TArray<uint32>& InIndices,
		const TArray<YStaticMeshSection>& InSections
	)
	{
		int32 NumVertices = InVertices.Num();
		if (InIndices.Num() <= 0 || NumVertices <= 0)
		{
			OutDepthIndices.Empty();
			return;
		}

		TArray<YIndexAndZ> VertexIndexZ;
		VertexIndexZ.Empty(NumVertices);
		for (int32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
		{
			new(VertexIndexZ)YIndexAndZ(VertexIndex, InVertices[VertexIndex].Position);
		}
		VertexIndexZ.Sort(YCompareIndexAndZ());

		TArray<uint32> IndexMap;
		IndexMap.AddUninitialized(NumVertices);
		FMemory::Memset(IndexMap.GetData(), 0xFF, NumVertices * sizeof(uint32));
		for (int32 i = 0; i < VertexIndexZ.Num(); ++i)
		{
			uint32 SrcIndex= VertexIndexZ[i].Index;
			float Z = VertexIndexZ[i].Z;
			IndexMap[SrcIndex] = FMath::Min(IndexMap[SrcIndex], SrcIndex);
			for (int32 j = i + 1; j < VertexIndexZ.Num(); ++j)
			{
				YIndexAndZ& OtherYIndexAndZ = VertexIndexZ[j];
				if (FMath::Abs(OtherYIndexAndZ.Z - Z) > THRESH_POINTS_ARE_SAME * 4.01f)
				{
					break;
				}
				
				{
					uint32 OtherIndex = VertexIndexZ[j].Index;
					if (PointsEqual(InVertices[SrcIndex].Position, InVertices[OtherIndex].Position,THRESH_POINTS_ARE_SAME))
					{
						IndexMap[SrcIndex] = FMath::Min(IndexMap[SrcIndex],OtherIndex);
						IndexMap[OtherIndex] = FMath::Min(IndexMap[OtherIndex], SrcIndex);
					}
				}
			}
		}

		OutDepthIndices.Empty();
		for (int32 SectionIndex = 0; SectionIndex < InSections.Num(); ++SectionIndex)
		{
			const YStaticMeshSection& Section = InSections[SectionIndex];
			int32 FirstIndex = Section.FirstIndex;
			int32 LastIndex = FirstIndex + Section.NumTriangles * 3;
			for (int32 SrcIndex = FirstIndex; SrcIndex < LastIndex; ++SrcIndex)
			{
				uint32 VertexIndex = InIndices[SrcIndex];
				OutDepthIndices.Add(IndexMap[VertexIndex]);
			}
		}
	}


	static void BuildStaticAdjacencyIndexBuffer(
		const YPositionVertexBuffer& PositionVertexBuffer,
		const YStaticMeshTangentUVVertexBuffer& VertexBuffer,
		const TArray<uint32>& Indices,
		TArray<uint32>& OutPnAenIndices
	)
	{
		if (Indices.Num())
		{
			YStaticMeshNvRenderBuffer StaticMeshRenderBuffer(PositionVertexBuffer, VertexBuffer, Indices);
			nv::IndexBuffer* PnAENIndexBuffer = nv::tess::buildTessellationBuffer(&StaticMeshRenderBuffer, nv::DBM_PnAenDominantCorner, true);
			check(PnAENIndexBuffer);
			const int32 IndexCount = (int32)PnAENIndexBuffer->getLength();
			OutPnAenIndices.Empty(IndexCount);
			OutPnAenIndices.AddUninitialized(IndexCount);
			for (int32 Index = 0; Index < IndexCount; ++Index)
			{
				OutPnAenIndices[Index] = (*PnAENIndexBuffer)[Index];
			}
			delete PnAENIndexBuffer;
		}
		else
		{
			OutPnAenIndices.Empty();
		}
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
			verifyf(Vertices.Num() != 0, TEXT("No valid vertices found for the mesh."));
			int32 NumTexCoords = 2;
			LODModel.VertexBuffer.Init(Vertices);
			LODModel.PositionVertexBuffer.Init(Vertices);
			
			//Concatenate the per section index buffers
			TArray<uint32> CombinedIndices;
			bool bNeeds32BitIndices = false;
			for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); ++SectionIndex)
			{
				YStaticMeshSection& Section = LODModel.Sections[SectionIndex];
				TArray<uint32> const & SectionIndices = PerSectionIndices[SectionIndex];
				Section.FirstIndex = 0;
				Section.NumTriangles = 0;
				Section.MinVertexIndex = 0;
				Section.MaxVertexIndex = 0;
				if (SectionIndices.Num())
				{
					Section.FirstIndex = CombinedIndices.Num();
					Section.NumTriangles = SectionIndices.Num() / 3;
					CombinedIndices.AddUninitialized(SectionIndices.Num());
					uint32* DestPtr = &CombinedIndices[Section.FirstIndex];
					uint32 const* SrcPtr = SectionIndices.GetData();
					Section.MinVertexIndex = *SrcPtr;
					Section.MaxVertexIndex = *SrcPtr;
					for (int32 Index = 0; Index < SectionIndices.Num(); ++Index)
					{
						uint32 VertexIndex = *SrcPtr++;
						bNeeds32BitIndices |= (VertexIndex > MAX_uint16);
						Section.MinVertexIndex = FMath::Min<uint32>(Section.MinVertexIndex, VertexIndex);
						Section.MaxVertexIndex = FMath::Max<uint32>(Section.MaxVertexIndex, VertexIndex);
						*DestPtr++ = VertexIndex;
					}
				}
			}
			LODModel.IndexBuffer.SetIndices(CombinedIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			
			// build the reversed index buffer
			{
				TArray<uint32> InversedIndices;
				const int32 IndexCount = CombinedIndices.Num();
				InversedIndices.AddUninitialized(IndexCount);
				for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); ++SectionIndex)
				{
					const YStaticMeshSection& SectionInfo = LODModel.Sections[SectionIndex];
					const int32 SectionIndexCount = SectionInfo.NumTriangles * 3;
					for (int32 i = 0; i < SectionIndexCount; ++i)
					{
						InversedIndices[SectionInfo.FirstIndex + i] = CombinedIndices[SectionInfo.FirstIndex + SectionIndexCount - i - 1];
					}
				}
				LODModel.ReversedIndexBuffer.SetIndices(InversedIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			}
			//Build the depth-only index buffer
			TArray<uint32> DepthOnlyIndices;
			{
				BuildDepthOnlyIndexBuffer(DepthOnlyIndices, Vertices, CombinedIndices, LODModel.Sections);
				if (DepthOnlyIndices.Num() < 50000 * 3)
				{
					YMeshUtilities::CacheOptimizeIndexBuffer(DepthOnlyIndices);
				}
				LODModel.DepthOnlyIndexBuffer.SetIndices(DepthOnlyIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			}

			{
				TArray<uint32> ReversedDepthOnlyIndices;
				const int32 IndexCount = DepthOnlyIndices.Num();
				ReversedDepthOnlyIndices.AddUninitialized(IndexCount);
				for (int32 i = 0; i < IndexCount; ++i)
				{
					ReversedDepthOnlyIndices[i] = DepthOnlyIndices[IndexCount - 1 - i];
				}
				LODModel.ReversedDepthOnlyIndexBuffer.SetIndices(ReversedDepthOnlyIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			}
			// Build a list of wireframe edges in the static mesh.
			{
				TArray<YMeshEdge> Edges;
				TArray<uint32> WireframeIndices;

				FStaticMeshEdgeBuilder(CombinedIndices, Vertices, Edges).FindEdges();
				WireframeIndices.Empty(2 * Edges.Num());
				for (int32 EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
				{
					YMeshEdge&	Edge = Edges[EdgeIndex];
					WireframeIndices.Add(Edge.Vertices[0]);
					WireframeIndices.Add(Edge.Vertices[1]);
				}
				LODModel.WireframeIndexBuffer.SetIndices(WireframeIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			}
			// Build the adjacency index buffer used for tessellation.
			{
				TArray<uint32> AdjacencyIndices;

				BuildStaticAdjacencyIndexBuffer(
					LODModel.PositionVertexBuffer,
					LODModel.VertexBuffer,
					CombinedIndices,
					AdjacencyIndices
				);
				LODModel.AdjacencyIndexBuffer.SetIndices(AdjacencyIndices, bNeeds32BitIndices ? EYIndexBufferStride::Force32Bit : EYIndexBufferStride::Force16Bit);
			}
		}
		if (NumValidLODs > 0)
		{
			OutRenderData.MaterialIndexToImportIndex = LODMeshes[0].MaterialIndexToImportIndex;
		}
		FBox BoundingBox(0);
		YPositionVertexBuffer& BasePositionVertexBuffer = OutRenderData.LODResources[0].PositionVertexBuffer;
		for (uint32 VertexIndex = 0; VertexIndex < BasePositionVertexBuffer.GetNumVertices(); VertexIndex++)
		{
			BoundingBox += BasePositionVertexBuffer.VertexPosition(VertexIndex);
		}
		BoundingBox.GetCenterAndExtents(OutRenderData.Bounds.Origin, OutRenderData.Bounds.BoxExtent);
		// Calculate the bounding sphere, using the center of the bounding box as the origin.
		OutRenderData.Bounds.SphereRadius = 0.0f;
		for (uint32 VertexIndex = 0; VertexIndex < BasePositionVertexBuffer.GetNumVertices(); VertexIndex++)
		{
			OutRenderData.Bounds.SphereRadius = FMath::Max(
				(BasePositionVertexBuffer.VertexPosition(VertexIndex) - OutRenderData.Bounds.Origin).Size(),
				OutRenderData.Bounds.SphereRadius
			);
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

	int32 NumValidLODs=0;

	TIndirectArray<YRawMesh> LODMeshes;
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
	if (!Builder.GenerateRenderingMeshes(OutRenderData, SourceModels))
	{
		return false;
	}
	return true;
}


YMeshUtilities::YMeshUtilities()
{

}


