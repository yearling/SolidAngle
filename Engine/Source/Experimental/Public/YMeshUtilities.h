#pragma once
#include "Core.h"
#include "YRawMesh.h"
#include "SStaticMesh.h"
#include "YStaticMeshRenderData.h"
#include "YMeshCommon.h"
DECLARE_LOG_CATEGORY_EXTERN(LogYMeshUtilities, Log, All);
namespace EYTangentOptions
{
	enum Type
	{
		None = 0,
		BlendOverlappingNormals = 0x1,
		IgnoreDegenerateTriangles = 0x2,
	};
};
/**
* Returns true if the specified points are about equal
*/

FORCEINLINE bool PointsEqual(const FVector& V1, const FVector& V2, float ComparisonThreshold)
{
	if (FMath::Abs(V1.X - V2.X) > ComparisonThreshold
		|| FMath::Abs(V1.Y - V2.Y) > ComparisonThreshold
		|| FMath::Abs(V1.Z - V2.Z) > ComparisonThreshold)
	{
		return false;
	}
	return true;
}

/**
 * Returns true if the specified normal vectors are about equal
 */
FORCEINLINE bool NormalsEqual(const FVector& V1, const FVector& V2)
{
	const float Epsilon = THRESH_NORMALS_ARE_SAME;
	return FMath::Abs(V1.X - V2.X) <= Epsilon && FMath::Abs(V1.Y - V2.Y) <= Epsilon && FMath::Abs(V1.Z - V2.Z) <= Epsilon;
}

FORCEINLINE bool UVsEqual(const FVector2D& V1, const FVector2D& V2)
{
	const float Epsilon = 1.0f / 1024.0f;
	return FMath::Abs(V1.X - V2.X) <= Epsilon && FMath::Abs(V1.Y - V2.Y) <= Epsilon;
}

class YMeshUtilities
{
public:
	YMeshUtilities();
	static bool BuildStaticMesh(class YStaticMeshRenderData& OutRenderData, TArray<struct YStaticMeshSourceModel>& SourceModels);
	static void ComputeTangents_MikkTSpace(YRawMesh& RawMesh, const uint32 TangentOptions);
	static void ComputeTriangleTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D>& InUVs, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, float ComparisonThreshold);
	static void ComputeTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D> &InUVs, const TArray<uint32>& SmoothingGroupIndices, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, const uint32 TangentOptions);
	static void FindOverlippingCorners(TMultiMap<int32, int32> &outOverlappingCorners, const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, float ComparisionThreshold);
	static void BuildStaticMeshVertexAndIndexBuffers(TArray<YStaticMeshBuildVertex>& OutVertices, TArray<TArray<uint32> >& OutPerSectionIndices, TArray<int32>& OutWedgeMap, const YRawMesh& RawMesh, const TMap<uint32, uint32>& MaterialToSectionMapping, float ComparisonThreshold);
	static void CacheOptimizeVertexAndIndexBuffer(TArray<YStaticMeshBuildVertex>& Vertices, TArray<TArray<uint32> >& PerSectionIndices, TArray<int32>& WedgeMap);
	static void CacheOptimizeIndexBuffer(TArray<uint16>& Indices);
	static void CacheOptimizeIndexBuffer(TArray<uint32>& Indices);
};
