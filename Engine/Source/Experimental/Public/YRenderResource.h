#pragma once
#include "Core.h"
#include "YYUT.h"
#include "YRHI.h"
#include "YRHIResource.h"
// RenderResource����һ���ӿڣ�������ͨCPU resource �� GPU resource������
// �����а�������RHIRef����ʵ��������ϵķ�ʽ����GPU resource����Դ 
// InitRHI, InitDynaimcRHI������֪ͨRHI����GPU��Դ��
// �������л�Ҫ���һ��CPU����Դ��InitRHIҪ��CPU�е���Դ����GPU����ʼ��
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


