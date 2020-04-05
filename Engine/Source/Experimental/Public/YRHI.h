#pragma once
#include "Core.h"
#include "Containers/ResourceArray.h"
#include <d3d11.h>
#include "YRHIDefinitions.h"

struct YRHIResourceCreateInfo
{
	// for CreateVertexBuffer/CreateStructuredBuffer calls
	YRHIResourceCreateInfo(FResourceArrayInterface* InResourceArray):ResourceArray(InResourceArray)
	{}
	// for CreateVertexBuffer/CreateStructuredBuffer calls
	FResourceArrayInterface* ResourceArray=nullptr;
};

class YRHIResource
{
public:
	YRHIResource();
	virtual ~YRHIResource();
	FORCEINLINE uint32 AddRef() const
	{
		int32 NewValue = NumRefs.Increment();
		checkSlow(NewValue > 0);
		return uint32(NewValue);
	}
	FORCEINLINE uint32 Release() const
	{
		int32 NewValue = NumRefs.Decrement();
		if (NewValue == 0)
		{
			delete this;
		}
		checkSlow(NewValue >= 0);
		return uint32(NewValue);
	}
	FORCEINLINE_DEBUGGABLE uint32 GetRefCount() const
	{
		int32 CurrentValue = NumRefs.GetValue();
		checkSlow(CurrentValue >= 0);
		return uint32(CurrentValue);
	}
private:
	mutable FThreadSafeCounter NumRefs;
};

class YRHIVertexBuffer : public YRHIResource
{
public:

	/**
	 * Initialization constructor.
	 * @apram InUsage e.g. BUF_UnorderedAccess
	 */
	YRHIVertexBuffer(uint32 InSize, uint32 InUsage)
		: Size(InSize)
		, Usage(InUsage)
	{}

	YRHIVertexBuffer(TRefCountPtr<ID3D11Buffer> InBuffer, uint32 InSize, uint32 InUsage)
		: VertexBuffer(InBuffer)
		, Size(InSize)
		, Usage(InUsage)
	{}
	/** @return The number of bytes in the vertex buffer. */
	uint32 GetSize() const { return Size; }

	/** @return The usage flags used to create the vertex buffer. e.g. BUF_UnorderedAccess */
	uint32 GetUsage() const { return Usage; }

public:
	uint32 Size;
	// e.g. BUF_UnorderedAccess
	uint32 Usage;
	TRefCountPtr<ID3D11Buffer> VertexBuffer;
};

class YRHIIndexBuffer : public YRHIResource
{
public:

	/** Initialization constructor. */
	YRHIIndexBuffer(uint32 InStride, uint32 InSize, uint32 InUsage)
		: Stride(InStride)
		, Size(InSize)
		, Usage(InUsage)
	{}
	YRHIIndexBuffer(TRefCountPtr<ID3D11Buffer> InBuffer, uint32 InStride, uint32 InSize, uint32 InUsage)
		: IndexBuffer(InBuffer)
		, Stride(InStride)
		, Size(InSize)
		, Usage(InUsage)
	{}

	/** @return The stride in bytes of the index buffer; must be 2 or 4. */
	uint32 GetStride() const { return Stride; }

	/** @return The number of bytes in the index buffer. */
	uint32 GetSize() const { return Size; }

	/** @return The usage flags used to create the index buffer. */
	uint32 GetUsage() const { return Usage; }

public:
	TRefCountPtr<ID3D11Buffer> IndexBuffer;
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};
