#pragma once
#include "Core.h"
#include "YScene.h"
#include "RenderInfo.h"
#include "YMeshBatch.h"
class IYRenderPolicy
{
public:
	IYRenderPolicy();
	virtual ~IYRenderPolicy() {};
	virtual void Render(TArray<YMeshBatch>& MeshBatchArray, TSharedRef<YRenderInfo> RenderInfo) = 0;
	virtual	void InitRenderReosurce()=0;
	virtual void ReleaseRenderResource() = 0;
};
class IYRenderInterface
{
public:
	IYRenderInterface();
	virtual ~IYRenderInterface()=0;
	virtual void InitRenders() = 0;
	virtual void RenderScene(TRefCountPtr<YScene>& Scene, TSharedRef<YRenderInfo> RenderInfo) = 0;
};


