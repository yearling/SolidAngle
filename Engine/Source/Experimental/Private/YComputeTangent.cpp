#include "Core.h"
#include "YMeshUtilities.h"
#include "YRawMesh.h"
#include "mikktspace.h"


void YMeshUtilities::ComputeTriangleTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D>& InUVs, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, float ComparisonThreshold)
{
	const int32 NumTriangles = InIndices.Num() / 3;
	OutTangentX.Empty(NumTriangles);
	OutTangentY.Empty(NumTriangles);
	OutTangentZ.Empty(NumTriangles);

	for (int32 TriangleIndex = 0; TriangleIndex < NumTriangles; ++TriangleIndex)
	{
		int32 UVIndex = 0;
		FVector P[3];
		for (int32 i = 0; i < 3; ++i)
		{
			P[i] = InVertices[InIndices[TriangleIndex * 3 + i]];
		}
		const FVector Normal = ((P[1] - P[2]) ^ (P[0] - P[2])).GetSafeNormal(ComparisonThreshold);
		FMatrix	ParameterToLocal(
			FPlane(P[1].X - P[0].X, P[1].Y - P[0].Y, P[1].Z - P[0].Z, 0),
			FPlane(P[2].X - P[0].X, P[2].Y - P[0].Y, P[2].Z - P[0].Z, 0),
			FPlane(P[0].X, P[0].Y, P[0].Z, 0),
			FPlane(0, 0, 0, 1)
		);

		const FVector2D T1 = InUVs[TriangleIndex * 3 + 0];
		const FVector2D T2 = InUVs[TriangleIndex * 3 + 1];
		const FVector2D T3 = InUVs[TriangleIndex * 3 + 2];

		FMatrix ParameterToTexture(
			FPlane(T2.X - T1.X, T2.Y - T1.Y, 0, 0),
			FPlane(T3.X - T1.X, T3.Y - T1.Y, 0, 0),
			FPlane(T1.X, T1.Y, 1, 0),
			FPlane(0, 0, 0, 1)
		);
		// Use InverseSlow to catch singular matrices.  Inverse can miss this sometimes.
		const FMatrix TextureToLocal = ParameterToTexture.Inverse() * ParameterToLocal;
		
		OutTangentX.Add(TextureToLocal.TransformVector(FVector(1, 0, 0)).GetSafeNormal());
		OutTangentY.Add(TextureToLocal.TransformVector(FVector(0, 1, 0)).GetSafeNormal());
		OutTangentZ.Add(Normal);
		//注意不是Gram-Schmidt正交化，只是与Tangent与Normal正交，Bitangent与Normal正交
		FVector::CreateOrthonormalBasis(OutTangentX[TriangleIndex], OutTangentY[TriangleIndex], OutTangentZ[TriangleIndex]);
	}
	check(OutTangentX.Num() == NumTriangles);
	check(OutTangentY.Num() == NumTriangles);
	check(OutTangentZ.Num() == NumTriangles);
}
/**
* Smoothing group interpretation helper structure.
*/
struct YFanFace
{
	int32 FaceIndex;	
	int32 LinkedVertexIndex;

	bool bFilled;
	bool bBlendTangents;
	bool bBlendNormals;
};

//
/*------------------------------------------------------------------------------
MikkTSpace for computing tangents.
------------------------------------------------------------------------------*/

static int MikkGetNumFaces(const SMikkTSpaceContext* Context)
{
	YRawMesh *UserData = (YRawMesh*)(Context->m_pUserData);
	return UserData->WedgeIndices.Num() / 3;
}

static int MikkGetNumVertsOfFace(const SMikkTSpaceContext* Context, const int FaceIdx)
{
	// All of our meshes are triangles.
	return 3;
}

static void MikkGetPosition(const SMikkTSpaceContext* Context, float Position[3], const int FaceIdx, const int VertIdx)
{
	YRawMesh *UserData = (YRawMesh*)(Context->m_pUserData);
	FVector VertexPosition = UserData->GetWedgePosition(FaceIdx * 3 + VertIdx);
	Position[0] = VertexPosition.X;
	Position[1] = VertexPosition.Y;
	Position[2] = VertexPosition.Z;
}

static void MikkGetNormal(const SMikkTSpaceContext* Context, float Normal[3], const int FaceIdx, const int VertIdx)
{
	YRawMesh *UserData = (YRawMesh*)(Context->m_pUserData);
	FVector &VertexNormal = UserData->WedgeTangentZ[FaceIdx * 3 + VertIdx];
	for (int32 i = 0; i < 3; ++i)
	{
		Normal[i] = VertexNormal[i];
	}
}

static void MikkSetTSpaceBasic(const SMikkTSpaceContext* Context, const float Tangent[3], const float BitangentSign, const int FaceIdx, const int VertIdx)
{
	YRawMesh *UserData = (YRawMesh*)(Context->m_pUserData);
	FVector &VertexTangent = UserData->WedgeTangentX[FaceIdx * 3 + VertIdx];
	for (int32 i = 0; i < 3; ++i)
	{
		VertexTangent[i] = Tangent[i];
	}
	FVector Bitangent = BitangentSign * FVector::CrossProduct(UserData->WedgeTangentZ[FaceIdx * 3 + VertIdx], VertexTangent);
	FVector &VertexBitangent = UserData->WedgeTangentY[FaceIdx * 3 + VertIdx];
	for (int32 i = 0; i < 3; ++i)
	{
		VertexBitangent[i] = -Bitangent[i];
	}
}

static void MikkGetTexCoord(const SMikkTSpaceContext* Context, float UV[2], const int FaceIdx, const int VertIdx)
{
	YRawMesh *UserData = (YRawMesh*)(Context->m_pUserData);
	FVector2D &TexCoord = UserData->WedgeTexCoords[0][FaceIdx * 3 + VertIdx];
	UV[0] = TexCoord.X;
	UV[1] = TexCoord.Y;
}

static inline FVector GetPositionForWedge(YRawMesh const& Mesh, int32 WedgeIndex)
{
	int32 VertexIndex = Mesh.WedgeIndices[WedgeIndex];
	return Mesh.VertexPositions[VertexIndex];
}


//void YMeshUtilities::ComputeTangents_MikkTSpace(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D> &InUVs, const TArray<uint32>& SmoothingGroupIndices, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, const uint32 TangentOptions)
void YMeshUtilities::ComputeTangents_MikkTSpace(YRawMesh& RawMesh, const uint32 TangentOptions)
{

	bool bBlendOverlappingNormals = (TangentOptions & EYTangentOptions::BlendOverlappingNormals) != 0;
	bool bIgnoreDegenerateTriangles = (TangentOptions & EYTangentOptions::IgnoreDegenerateTriangles) != 0;
	float ComparisionThreshold = bIgnoreDegenerateTriangles ? THRESH_POINTS_ARE_SAME : 0.0f;

	// Compute per-triangle tangents.
	TArray<FVector> TriangleTangentX;
	TArray<FVector> TriangleTangentY;
	TArray<FVector> TriangleTangentZ;
	ComputeTriangleTangents(RawMesh.VertexPositions, RawMesh.WedgeIndices, RawMesh.WedgeTexCoords[0], TriangleTangentX, TriangleTangentY, TriangleTangentZ, ComparisionThreshold);
	int32 NumWedges = RawMesh.WedgeIndices.Num();
	int32 NumFaces = NumWedges / 3;
	// Allocate storage for tangents if none were provided.
	bool bWedgeNormals = true;
	bool bWedgeTSpace = false;
	for (int32 WedgeIdx = 0; WedgeIdx < RawMesh.WedgeTangentZ.Num(); ++WedgeIdx)
	{
		bWedgeNormals = bWedgeNormals && (!RawMesh.WedgeTangentZ[WedgeIdx].IsNearlyZero());
	}

	if (RawMesh.WedgeTangentX.Num() > 0 && RawMesh.WedgeTangentY.Num() > 0)
	{
		bWedgeTSpace = true;
		for (int32 WedgeIdx = 0; WedgeIdx < RawMesh.WedgeTangentX.Num()
			&& WedgeIdx < RawMesh.WedgeTangentY.Num(); ++WedgeIdx)
		{
			bWedgeTSpace = bWedgeTSpace && (RawMesh.WedgeTangentX[WedgeIdx].IsNearlyZero()) && (!RawMesh.WedgeTangentY[WedgeIdx].IsNearlyZero());
		}
	}

	// Allocate storage for tangents if none were provided, and calculate normals for MikkTSpace.
	if (RawMesh.WedgeTangentZ.Num() != NumWedges || !bWedgeNormals)
	{
		RawMesh.WedgeTangentZ.Empty(NumWedges);
		RawMesh.WedgeTangentZ.AddZeroed(NumWedges);

		TMultiMap<int32, int32> OverlappingCorners;
		YMeshUtilities::FindOverlippingCorners(OverlappingCorners, RawMesh.VertexPositions, RawMesh.WedgeIndices, ComparisionThreshold);
		for (int32 FaceIndex = 0; FaceIndex < NumFaces; ++FaceIndex)
		{
			int32 WedgeOffset = FaceIndex * 3;
			FVector CornerPosition[3] = { GetPositionForWedge(RawMesh,WedgeOffset + 0),GetPositionForWedge(RawMesh,WedgeOffset + 1), GetPositionForWedge(RawMesh,WedgeOffset + 2) };
			FVector CornerTangentX[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };
			FVector CornerTangentY[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };
			FVector CornerTangentZ[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };

			if (PointsEqual(CornerPosition[0], CornerPosition[1], ComparisionThreshold) ||
				PointsEqual(CornerPosition[1], CornerPosition[2], ComparisionThreshold) ||
				PointsEqual(CornerPosition[2], CornerPosition[0], ComparisionThreshold))
			{
				continue;
			}

			// No need to process triangles if tangents already exist.
			bool bCornerHasNormal[3] = { 0 };
			for (int32 CornerIndex : {0, 1, 2})
			{
				bCornerHasNormal[CornerIndex] = !RawMesh.WedgeTangentZ[WedgeOffset + CornerIndex].IsZero();
			}
			if (bCornerHasNormal[0] && bCornerHasNormal[1] && bCornerHasNormal[2])
			{
				continue;
			}

			float Determinant = FVector::Triple(TriangleTangentX[FaceIndex], TriangleTangentY[FaceIndex], TriangleTangentZ[FaceIndex]);

			TArray<int32> AdjacentFaces;
			for (int32 CornerIndex : {0, 1, 2})
			{
				TArray<int32> FindResults;
				OverlappingCorners.MultiFind(WedgeOffset + CornerIndex, FindResults);
				FindResults.Add(WedgeOffset + CornerIndex);
				for (int32 Neibour : FindResults)
				{
					AdjacentFaces.AddUnique(Neibour / 3);
				}
			}

			AdjacentFaces.Sort();

			TArray<YFanFace> RelevantFanFaces[3];
			for (int32 CornerIndex : {0, 1, 2})
			{
				for (int32 AdjacentFaceIndex : AdjacentFaces)
				{
					YFanFace FanFace;
					int32 Connections = 0;
					if (FaceIndex == AdjacentFaceIndex)
					{
						Connections = 3;
						FanFace.LinkedVertexIndex = CornerIndex;
					}
					else
					{
						for (int32 OtherFaceCornerIndex : { 0, 1, 2 })
						{
							if (PointsEqual(CornerPosition[CornerIndex], GetPositionForWedge(RawMesh,AdjacentFaceIndex * 3 + OtherFaceCornerIndex), ComparisionThreshold))
							{
								Connections++;
								FanFace.LinkedVertexIndex = OtherFaceCornerIndex;
							}
						}
					}
					if (Connections > 0)
					{
						FanFace.FaceIndex = AdjacentFaceIndex;
						FanFace.bFilled = (FaceIndex == AdjacentFaceIndex);
						FanFace.bBlendNormals = FanFace.bFilled;
						FanFace.bBlendTangents = FanFace.bFilled;
						RelevantFanFaces[CornerIndex].Add(FanFace);
					}
				}
			}

			for (int32 CornerIndex : {0, 1, 2})
			{
				if (bCornerHasNormal[CornerIndex])
					continue;
				int32 CurrentWedgeIndex = WedgeOffset + CornerIndex;
				int32 NewConnection = 0;
				do {
					NewConnection = 0;
					for (YFanFace& OtherFaceFace : RelevantFanFaces[CornerIndex])
					{
						if (OtherFaceFace.bFilled)
						{
							for (YFanFace& NextFanFace : RelevantFanFaces[CornerIndex])
							{
								if (!NextFanFace.bFilled)
								{
									// smooth group 
									if (OtherFaceFace.FaceIndex != NextFanFace.FaceIndex && (RawMesh.FaceSmoothingMasks[OtherFaceFace.FaceIndex] & RawMesh.FaceSmoothingMasks[NextFanFace.FaceIndex]))
									{
										int32 CommonVertices = 0;
										int32 CommonNormalVertices = 0;
										for (int32 OtherCornerIndex : {0, 1, 2})
										{
											for (int32 NextCornerIndex : {0, 1, 2})
											{
												int32 OtherWedgeIndex = OtherFaceFace.FaceIndex * 3 + OtherCornerIndex;
												int32 NextWedgeIndex = NextFanFace.FaceIndex * 3 + NextCornerIndex;
												const FVector& OtherWedgePostion = GetPositionForWedge(RawMesh,OtherWedgeIndex);
												const FVector& NextWedgePostion = GetPositionForWedge(RawMesh,NextWedgeIndex);
												if (PointsEqual(OtherWedgePostion, NextWedgePostion, ComparisionThreshold))
												{
													CommonVertices++;
													if (bBlendOverlappingNormals || OtherWedgeIndex == NextWedgeIndex)
													{
														CommonNormalVertices++;
													}
												}

											}
										}
										if (CommonVertices > 1)
										{
											NextFanFace.bFilled = true;
											NextFanFace.bBlendNormals = CommonNormalVertices > 1;
											NewConnection++;
										}
									}
								}
							}
						}
					}
				} while (NewConnection > 0);
			}
			for (int32 CornerIndex : {0, 1, 2})
			{
				if (bCornerHasNormal[CornerIndex])
				{
					CornerTangentZ[CornerIndex] = RawMesh.WedgeTangentZ[WedgeOffset + CornerIndex];
				}
				else
				{
					for (YFanFace& RelevantFace : RelevantFanFaces[CornerIndex])
					{
						if (RelevantFace.bFilled)
						{
							int32 RelevantFaceIndex = RelevantFace.FaceIndex;
							if (RelevantFace.bBlendNormals)
							{
								CornerTangentZ[CornerIndex] += TriangleTangentZ[RelevantFaceIndex];
							}
						}
					}
					if (!RawMesh.WedgeTangentZ[WedgeOffset + CornerIndex].IsZero())
					{
						CornerTangentZ[CornerIndex] = RawMesh.WedgeTangentZ[WedgeOffset + CornerIndex];
					}
				}
			}

			for (int32 CornerIndex : {0, 1, 2})
			{
				CornerTangentZ[CornerIndex].Normalize();
			}

			for (int32 CornerIndex : {0, 1, 2})
			{
				RawMesh.WedgeTangentZ[WedgeOffset + CornerIndex] = CornerTangentZ[CornerIndex];
			}
		}
	}
	if (RawMesh.WedgeTangentX.Num() != NumWedges)
	{
		RawMesh.WedgeTangentX.Empty(NumWedges);
		RawMesh.WedgeTangentX.AddZeroed(NumWedges);
	}
	if (RawMesh.WedgeTangentY.Num() != NumWedges)
	{
		RawMesh.WedgeTangentY.Empty(NumWedges);
		RawMesh.WedgeTangentY.AddZeroed(NumWedges);
	}
	if (!bWedgeTSpace)
	{
		// we can use mikktspace to calculate the tangents
		SMikkTSpaceInterface MikkTInterface;
		MikkTInterface.m_getNormal = MikkGetNormal;
		MikkTInterface.m_getNumFaces = MikkGetNumFaces;
		MikkTInterface.m_getNumVerticesOfFace = MikkGetNumVertsOfFace;
		MikkTInterface.m_getPosition = MikkGetPosition;
		MikkTInterface.m_getTexCoord = MikkGetTexCoord;
		MikkTInterface.m_setTSpaceBasic = MikkSetTSpaceBasic;
		MikkTInterface.m_setTSpace = nullptr;

		SMikkTSpaceContext MikkTContext;
		MikkTContext.m_pInterface = &MikkTInterface;
		MikkTContext.m_pUserData = (void*)(&RawMesh);
		MikkTContext.m_bIgnoreDegenerates = bIgnoreDegenerateTriangles;
		genTangSpaceDefault(&MikkTContext);
	}
}

void YMeshUtilities::ComputeTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D> &InUVs, const TArray<uint32>& SmoothingGroupIndices, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, const uint32 TangentOptions)
{
	bool bBlendOverlappingNormals = (TangentOptions & EYTangentOptions::BlendOverlappingNormals) != 0;
	bool bIgnoreDegenerateTriangles = (TangentOptions & EYTangentOptions::IgnoreDegenerateTriangles) != 0;
	float ComparisionThreshold = bIgnoreDegenerateTriangles ? THRESH_POINTS_ARE_SAME : 0.0f;

	// 计算每个面的三角形
	TArray<FVector> TriangleTangentX;
	TArray<FVector> TriangleTangentY;
	TArray<FVector> TriangleTangentZ;
	ComputeTriangleTangents(InVertices, InIndices, InUVs, TriangleTangentX, TriangleTangentY, TriangleTangentZ, bIgnoreDegenerateTriangles ? SMALL_NUMBER : 0.0f);

	// Declare these out here to avoid reallocations.

	int32 NumWedges = InIndices.Num();
	int32 NumFaces = NumWedges / 3;
	// Allocate storage for tangents if none were provided.
	if (OutTangentX.Num() != NumWedges)
	{
		OutTangentX.Empty(NumWedges);
		OutTangentX.AddZeroed(NumWedges);
	}
	if (OutTangentY.Num() != NumWedges)
	{
		OutTangentY.Empty(NumWedges);
		OutTangentY.AddZeroed(NumWedges);
	}
	if (OutTangentZ.Num() != NumWedges)
	{
		OutTangentZ.Empty(NumWedges);
		OutTangentZ.AddZeroed(NumWedges);
	}

	TMultiMap<int32, int32> OverlappingCorners;
	YMeshUtilities::FindOverlippingCorners(OverlappingCorners, InVertices, InIndices, ComparisionThreshold);
	for (int32 FaceIndex = 0; FaceIndex < NumFaces; ++FaceIndex) 
	{
		int32 WedgeOffset = FaceIndex * 3;
		FVector CornerPosition[3] = { InVertices[InIndices[WedgeOffset + 0]],InVertices[InIndices[WedgeOffset + 1]], InVertices[InIndices[WedgeOffset + 2]] };
		FVector CornerTangentX[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };
		FVector CornerTangentY[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };
		FVector CornerTangentZ[3] = { FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector };
		
		if(PointsEqual(CornerPosition[0], CornerPosition[1],ComparisionThreshold) || 
		   PointsEqual(CornerPosition[1], CornerPosition[2],ComparisionThreshold) || 
		   PointsEqual(CornerPosition[2], CornerPosition[0],ComparisionThreshold) )
		{ 
			continue;
		}

		// No need to process triangles if tangents already exist.
		bool bCornerHasTangents[3] = { 0 };
		for (int32 CornerIndex : {0, 1, 2})
		{
			bCornerHasTangents[CornerIndex] = !OutTangentX[WedgeOffset + CornerIndex].IsZero()
				&& !OutTangentY[WedgeOffset + CornerIndex].IsZero()
				&& !OutTangentZ[WedgeOffset + CornerIndex].IsZero();
		}
		if (bCornerHasTangents[0] && bCornerHasTangents[1] && bCornerHasTangents[2])
		{
			continue;
		}
		
		float Determinant = FVector::Triple(TriangleTangentX[FaceIndex], TriangleTangentY[FaceIndex], TriangleTangentZ[FaceIndex]);

		TArray<int32> AdjacentFaces;
		for (int32 CornerIndex : {0, 1, 2})
		{
			TArray<int32> FindResults;
			OverlappingCorners.MultiFind(WedgeOffset + CornerIndex, FindResults);
			FindResults.Add(WedgeOffset + CornerIndex);
			for (int32 Neibour : FindResults)
			{
				AdjacentFaces.AddUnique(Neibour/3);
			}
		}
		
		AdjacentFaces.Sort();

		TArray<YFanFace> RelevantFanFaces[3];
		for (int32 CornerIndex : {0, 1, 2})
		{
			for (int32 AdjacentFaceIndex : AdjacentFaces)
			{
				YFanFace FanFace;
				int32 Connections = 0;
				if (FaceIndex == AdjacentFaceIndex)
				{
					Connections=3;
					FanFace.LinkedVertexIndex = CornerIndex;
				}
				else
				{
					for (int32 OtherFaceCornerIndex : { 0, 1, 2 })
					{
						if (PointsEqual(CornerPosition[CornerIndex], InVertices[InIndices[AdjacentFaceIndex * 3 + OtherFaceCornerIndex]],ComparisionThreshold))
						{
							Connections++;
							FanFace.LinkedVertexIndex = OtherFaceCornerIndex;
						}
					}
				}
				if (Connections > 0)
				{
					FanFace.FaceIndex = AdjacentFaceIndex;
					FanFace.bFilled = (FaceIndex == AdjacentFaceIndex);
					FanFace.bBlendNormals = FanFace.bFilled;
					FanFace.bBlendTangents = FanFace.bFilled;
					RelevantFanFaces[CornerIndex].Add(FanFace);
				}
			}
		}
	
		for (int32 CornerIndex : {0, 1, 2})
		{
			int32 CurrentWedgeIndex = WedgeOffset + CornerIndex;
			int32 NewConnection = 0;
			do {
				NewConnection = 0;
				for (YFanFace& OtherFaceFace : RelevantFanFaces[CornerIndex])
				{
					if (OtherFaceFace.bFilled)
					{
						for (YFanFace& NextFanFace : RelevantFanFaces[CornerIndex])
						{
							if (!NextFanFace.bFilled)
							{
								// smooth group 
								if (OtherFaceFace.FaceIndex != NextFanFace.FaceIndex && (SmoothingGroupIndices[OtherFaceFace.FaceIndex] & SmoothingGroupIndices[NextFanFace.FaceIndex]))
								{
									int32 CommonVertices = 0;
									int32 CommonTangentVertices = 0;
									int32 CommonNormalVertices = 0;
									for (int32 OtherCornerIndex : {0, 1, 2})
									{
										for (int32 NextCornerIndex : {0, 1, 2})
										{
											int32 OtherWedgeIndex = OtherFaceFace.FaceIndex * 3 + OtherCornerIndex;
											int32 NextWedgeIndex = NextFanFace.FaceIndex * 3 + NextCornerIndex;
											const FVector& OtherWedgePostion = InVertices[InIndices[OtherWedgeIndex]];
											const FVector& NextWedgePostion = InVertices[InIndices[NextWedgeIndex]];
											const FVector2D& OtherWedgeUV = InUVs[OtherWedgeIndex];
											const FVector2D& NextWedgeUV = InUVs[NextWedgeIndex];
											if (PointsEqual(OtherWedgePostion,NextWedgePostion,ComparisionThreshold))
											{
												CommonVertices++;

												if (UVsEqual(OtherWedgeUV, NextWedgeUV))
												{
													CommonTangentVertices++;
												}

												if (bBlendOverlappingNormals || OtherWedgeIndex == NextWedgeIndex)
												{
													CommonNormalVertices++;
												}
											}
											
										}
									}
									if (CommonVertices > 1)
									{
										NextFanFace.bFilled = true;
										NextFanFace.bBlendNormals = CommonNormalVertices > 1;
										NewConnection++;
										if (CommonTangentVertices > 1)
										{
											float NextDeterminant = FVector::Triple(TriangleTangentX[NextFanFace.FaceIndex], TriangleTangentY[NextFanFace.FaceIndex], TriangleTangentZ[NextFanFace.FaceIndex]);
											if (NextDeterminant*Determinant > 0.0f)
											{
												NextFanFace.bBlendTangents = true;
											}
										}
									}
								}
							}
						}
					}
				}
			} while (NewConnection>0);
		}
		for (int32 CornerIndex : {0, 1, 2})
		{
			for (YFanFace& RelevantFace : RelevantFanFaces[CornerIndex])
			{
				if (RelevantFace.bFilled)
				{
					int32 RelevantFaceIndex = RelevantFace.FaceIndex;
					if(RelevantFace.bBlendTangents)
					{
						CornerTangentX[CornerIndex] += TriangleTangentX[RelevantFaceIndex];
						CornerTangentY[CornerIndex] += TriangleTangentY[RelevantFaceIndex];
					}

					if (RelevantFace.bBlendNormals)
					{
						CornerTangentZ[CornerIndex] += TriangleTangentZ[RelevantFaceIndex];
					}
				}
			}
			if (!OutTangentX[WedgeOffset + CornerIndex].IsZero())
			{
				CornerTangentX[CornerIndex] = OutTangentX[WedgeOffset + CornerIndex];
			}
			if (!OutTangentY[WedgeOffset + CornerIndex].IsZero())
			{
				CornerTangentY[CornerIndex] = OutTangentY[WedgeOffset + CornerIndex];
			}
			if (!OutTangentZ[WedgeOffset + CornerIndex].IsZero())
			{
				CornerTangentZ[CornerIndex] = OutTangentZ[WedgeOffset + CornerIndex];
			}
		}

		for (int32 CornerIndex : {0, 1, 2})
		{
			CornerTangentX[CornerIndex].Normalize();
			CornerTangentY[CornerIndex].Normalize();
			CornerTangentZ[CornerIndex].Normalize();

			CornerTangentX[CornerIndex] -= CornerTangentZ[CornerIndex] * (CornerTangentX[CornerIndex] | CornerTangentZ[CornerIndex]);
			CornerTangentX[CornerIndex].Normalize();
			CornerTangentY[CornerIndex] -= CornerTangentX[CornerIndex] * (CornerTangentX[CornerIndex] | CornerTangentY[CornerIndex]);
			CornerTangentY[CornerIndex].Normalize();
			CornerTangentY[CornerIndex] -= CornerTangentZ[CornerIndex] * (CornerTangentZ[CornerIndex] | CornerTangentY[CornerIndex]);
			CornerTangentY[CornerIndex].Normalize();
		}

		for (int32 CornerIndex : {0, 1, 2})
		{
			OutTangentX[WedgeOffset + CornerIndex] = CornerTangentX[CornerIndex];
			OutTangentY[WedgeOffset + CornerIndex] = CornerTangentY[CornerIndex];
			OutTangentZ[WedgeOffset + CornerIndex] = CornerTangentZ[CornerIndex];
		}
	}
	for (int32 WedgeIndex = 0; WedgeIndex < InIndices.Num(); ++WedgeIndex)
	{
		FVector together = OutTangentX[WedgeIndex] + OutTangentY[WedgeIndex];
	}
}

