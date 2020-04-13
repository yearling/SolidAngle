#pragma once
#include "Core.h"
#include "SStaticMesh.h"
#include "YMeshBatch.h"
class IPrimitive;
class YScene:public FRefCountedObject
{
public:
	YScene();
	void RegisterToScene(TRefCountPtr<SStaticMesh> InMesh);
	void Clear();
public:
	TArray<YMeshBatch> StaticMeshOpaquePrimitives;
	TArray<YMeshBatch> StaticMeshMaskPrimitives;
	TArray<YMeshBatch> StaticMeshTrunslucentPrimitives;
	TArray<TRefCountPtr<SStaticMesh>> StaticMeshArray;
};
