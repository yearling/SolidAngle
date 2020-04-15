#pragma once
#include "Core.h"
#include "SStaticMesh.h"
#include "YMeshBatch.h"
#include "SStaticMeshComponent.h"
class IPrimitive;
class YScene:public FRefCountedObject
{
public:
	YScene();
	void RegisterToScene(TRefCountPtr<SStaticMesh> InMesh);
	void RegisterToScene(SStaticMeshComponent *InMesh);
	void Clear();
public:
	TArray<YMeshBatch> StaticMeshOpaquePrimitives;
	TArray<YMeshBatch> StaticMeshMaskPrimitives;
	TArray<YMeshBatch> StaticMeshTrunslucentPrimitives;
	TArray<TRefCountPtr<SStaticMesh>> StaticMeshArray;
};
