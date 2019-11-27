#include "Core.h"
#include "YMeshUtilities.h"
#include "YRawMesh.h"


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

void YMeshUtilities::ComputeTangents_MikkTSpace(YRawMesh& RawMesh, TMultiMap<int32, int32> const& OverlappingCorners, uint32 TangentOptions)
{
	bool bBlendOverlappingNormals = (TangentOptions & EYTangentOptions::BlendOverlappingNormals) != 0;
	bool bIgnoreDegenerateTriangles = (TangentOptions & EYTangentOptions::IgnoreDegenerateTriangles) != 0;
	float ComparisionThreshold = bIgnoreDegenerateTriangles ? THRESH_POINTS_ARE_SAME : 0.0f;

	// Compute per-triangle tangents.
	TArray<FVector> TriangleTangentX;
	TArray<FVector> TriangleTangentY;
	TArray<FVector> TriangleTangentZ;
	ComputeTriangleTangents(RawMesh.VertexPositions, RawMesh.WedgeIndices, RawMesh.WedgeTexCoords[0], TriangleTangentX, TriangleTangentY, TriangleTangentZ, bIgnoreDegenerateTriangles ? SMALL_NUMBER : 0.0f);
	// Declare these out here to avoid reallocations.
	TArray<YFanFace> RelevantFacesForCorner[3];
	TArray<int32> AdjacentFaces;
	TArray<int32> DupVerts;

	int32 NumWedges = RawMesh.WedgeIndices.Num();
	int32 NumFaces = NumWedges / 3;
	bool bWedgeNormals = true;
	bool bWedgeTSpace = false;
	for (int32 WedgeIndex = 0; WedgeIndex < RawMesh.WedgeTangentZ.Num(); ++WedgeIndex)
	{
		bWedgeNormals = bWedgeNormals && (!RawMesh.WedgeTangentZ[WedgeIndex].IsNearlyZero());
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
	TArray<YFanFace> RelevantFacesForCorner[3];
	TArray<int32> AdjacentFaces;
	TArray<int32> DupVerts;

	int32 NumWedges = InIndices.Num();
	int32 NumFaces = NumWedges / 3;
}

