#pragma once
#include "Core.h"
#include "YYUT.h"
#include "YRHI.h"
#include "YRHIResource.h"
// RenderResource就是一个接口，用来沟通CPU resource 与 GPU resource的桥梁
// 子类中包含各种RHIRef，其实就是用组合的方式保存GPU resource的资源 
// InitRHI, InitDynaimcRHI是用来通知RHI分配GPU资源的
// 子类里中还要组合一个CPU的资源，InitRHI要把CPU中的资源传给GPU来初始化
class YRenderResource
{
public:
	YRenderResource() {}
	virtual ~YRenderResource() {}
	virtual FString GetFriendlyName() const { return TEXT("undefined"); }
	// Accessors.
	FORCEINLINE bool IsInitialized() const { return bInitialized; }
	virtual void InitDynamicRHI() {}
	virtual void ReleaseDynamicRHI() {}
	virtual void InitRHI() {}
	virtual void ReleaseRHI() {}
	virtual void InitResource();
	virtual void ReleaseResource();
protected:
	bool bInitialized = false;
};

class YVertexBuffer : public YRenderResource
{
public:
	TRefCountPtr<YRHIVertexBuffer> VertexBufferRHI;

	/** Destructor. */
	virtual ~YVertexBuffer() {}

	// FRenderResource interface.
	virtual void ReleaseRHI() override
	{
		VertexBufferRHI.SafeRelease();
	}
	virtual FString GetFriendlyName() const override { return TEXT("FVertexBuffer"); }
};

class YIndexBuffer : public YRenderResource
{
public:
	TRefCountPtr<YRHIIndexBuffer> IndexBufferRHI;

	/** Destructor. */
	virtual ~YIndexBuffer() {}

	// FRenderResource interface.
	virtual void ReleaseRHI() override
	{
		IndexBufferRHI.SafeRelease();
	}
	virtual FString GetFriendlyName() const override { return TEXT("FIndexBuffer"); }
};


