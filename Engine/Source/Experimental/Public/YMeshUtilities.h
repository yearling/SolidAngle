#pragma once
#include "Core.h"
#include "YRawMesh.h"
#include "YStaticMesh.h"
#include "YStaticMeshRenderData.h"
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
class YMeshUtilities
{
public:
	YMeshUtilities();
	bool BuildStaticMesh(class YStaticMeshRenderData& OutRenderData, TArray<struct YStaticMeshSourceModel>& SourceModels);
	static void ComputeTangents_MikkTSpace(YRawMesh& RawMesh,TMultiMap<int32, int32> const& OverlappingCorners,uint32 TangentOptions);
	static void ComputeTriangleTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D>& InUVs, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY,TArray<FVector>& OutTangentZ,float ComparisonThreshold);
	static void ComputeTangents(const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, const TArray<FVector2D> &InUVs, const TArray<uint32>& SmoothingGroupIndices, TArray<FVector>& OutTangentX, TArray<FVector>& OutTangentY, TArray<FVector>& OutTangentZ, const uint32 TangentOptions);
	static void FindOverlippingCorners(TMultiMap<int32, int32> &outOverlappingCorners, const TArray<FVector>& InVertices, const TArray<uint32>& InIndices, float ComparisionThreshold);
};
