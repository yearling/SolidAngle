#pragma once
#include "Core.h"

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
	YMeshUtilities();
	bool YMeshUtilities::BuildStaticMesh(class YStaticMeshRenderData& OutRenderData, TArray<struct YStaticMeshSourceModel>& SourceModels);
};
