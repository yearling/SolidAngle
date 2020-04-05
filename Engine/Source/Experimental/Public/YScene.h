#pragma once
#include "Core.h"
#include "YStaticMesh.h"
#include "YMeshBatch.h"
class IPrimitive;
class YScene:public FRefCountedObject
{
public:
	YScene();
	void RegisterToScene(TRefCountPtr<YStaticMesh> InMesh);

public:
	TArray<YMeshBatch> StaticMeshOpaquePrimitives;
	TArray<YMeshBatch> StaticMeshMaskPrimitives;
	TArray<YMeshBatch> StaticMeshTrunslucentPrimitives;
	TArray<TRefCountPtr<YStaticMesh>> StaticMeshArray;
};
