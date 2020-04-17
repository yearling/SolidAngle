#pragma once
#include "Core.h"
#include "YRHIDefinitions.h"
#include "YRHI.h"
#include "YPixelFormat.h"
#include "Misc\SecureHash.h"
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

// StateBlocker
class YRHISamplerState :public YRHIResource
{
public:
	TRefCountPtr<ID3D11SamplerState> Resource;
};

class YRHIRasterizerState :public YRHIResource
{
public:
	TRefCountPtr<ID3D11RasterizerState> Resource;
};

class YRHIDepthStencilState : public YRHIResource
{
public:
	TRefCountPtr<ID3D11DepthStencilState> Resource;
};

class YRHIBlendState :public YRHIResource
{
public:
	TRefCountPtr<ID3D11BlendState> Resource;
};

typedef TArray<D3D11_INPUT_ELEMENT_DESC, TFixedAllocator<YMaxVertexElementCount> > YD3D11VertexElements;

template <>
struct TTypeTraits<D3D11_INPUT_ELEMENT_DESC> : public TTypeTraitsBase<D3D11_INPUT_ELEMENT_DESC>
{
	enum { IsBytewiseComparable = true };
};

// shader binding
class YRHIVertexDeclaration : public YRHIResource
{
public:
	YD3D11VertexElements VertexElements;

	/** Initialization constructor. */
	explicit YRHIVertexDeclaration(const YD3D11VertexElements& InElements)
		: VertexElements(InElements)
	{
	}
};

//
// Shaders
//

class YRHIShader : public YRHIResource
{
public:
	YRHIShader() {}

	void SetHash(FSHAHash InHash) { Hash = InHash; }
	FSHAHash GetHash() const { return Hash; }

	// for debugging only e.g. MaterialName:ShaderFile.usf or ShaderFile.usf/EntryFunc
	FString ShaderName;

private:
	FSHAHash Hash;
};

class YRHIVertexShader : public YRHIShader
{
public:
	enum { StaticFrequency = YSF_Vertex };

	/** The vertex shader resource. */
	TRefCountPtr<ID3D11VertexShader> Resource;

	/** The vertex shader's bytecode, with custom data attached. */
	TArray<uint8> Code;

	// TEMP remove with removal of bound shader state
	int32 Offset;
};


class YRHIHullShader : public YRHIShader
{
public:
	enum { StaticFrequency = YSF_Hull };

	/** The shader resource. */
	TRefCountPtr<ID3D11HullShader> Resource;
};

class YRHIDomainShader : public YRHIShader {
public:
	enum { StaticFrequency = YSF_Domain };

	/** The shader resource. */
	TRefCountPtr<ID3D11DomainShader> Resource;
};
class YRHIPixelShader : public YRHIShader {
public:
	enum { StaticFrequency = YSF_Pixel };

	/** The shader resource. */
	TRefCountPtr<ID3D11PixelShader> Resource;
};
class YRHIGeometryShader : public YRHIShader {
public:
	enum { StaticFrequency = YSF_Geometry };

	/** The shader resource. */
	TRefCountPtr<ID3D11GeometryShader> Resource;
};
class YRHIComputeShader : public YRHIShader {
public:
	enum { StaticFrequency = YSF_Compute };

	/** The shader resource. */
	TRefCountPtr<ID3D11ComputeShader> Resource;
};

//
// Pipeline States
//

class YRHIGraphicsPipelineState : public YRHIResource {};
class YRHIComputePipelineState : public YRHIResource {};

class YRHIBoundShaderState : public YRHIResource
{
public:


	TRefCountPtr<ID3D11InputLayout> InputLayout;
	TRefCountPtr<ID3D11VertexShader> VertexShader;
	TRefCountPtr<ID3D11PixelShader> PixelShader;
	TRefCountPtr<ID3D11HullShader> HullShader;
	TRefCountPtr<ID3D11DomainShader> DomainShader;
	TRefCountPtr<ID3D11GeometryShader> GeometryShader;
	YRHIBoundShaderState();
	~YRHIBoundShaderState();
	/** Initialization constructor. */
	/*YRHIBoundShaderState(
		FVertexDeclarationRHIParamRef InVertexDeclarationRHI,
		FVertexShaderRHIParamRef InVertexShaderRHI,
		FPixelShaderRHIParamRef InPixelShaderRHI,
		FHullShaderRHIParamRef InHullShaderRHI,
		FDomainShaderRHIParamRef InDomainShaderRHI,
		FGeometryShaderRHIParamRef InGeometryShaderRHI,
		ID3D11Device* Direct3DDevice
	);

	~YRHIBoundShaderState();*/

	/**
	 * Get the shader for the given frequency.
	 */
	 //FORCEINLINE FD3D11VertexShader*   GetVertexShader() const { return (FD3D11VertexShader*)CacheLink.GetVertexShader(); }
	 //FORCEINLINE FD3D11PixelShader*    GetPixelShader() const { return (FD3D11PixelShader*)CacheLink.GetPixelShader(); }
	 //FORCEINLINE FD3D11HullShader*     GetHullShader() const { return (FD3D11HullShader*)CacheLink.GetHullShader(); }
	 //FORCEINLINE FD3D11DomainShader*   GetDomainShader() const { return (FD3D11DomainShader*)CacheLink.GetDomainShader(); }
	 //FORCEINLINE FD3D11GeometryShader* GetGeometryShader() const { return (FD3D11GeometryShader*)CacheLink.GetGeometryShader(); }
};

//
// Buffers
//

/** The layout of a uniform buffer in memory. */
struct YRHIUniformBufferLayout
{
	/** The size of the constant buffer in bytes. */
	uint32 ConstantBufferSize;
	/** The offset to the beginning of the resource table. */
	uint32 ResourceOffset;
	/** The type of each resource (EUniformBufferBaseType). */
	TArray<uint8> Resources;

	uint32 GetHash() const
	{
		if (!bComputedHash)
		{
			uint32 TmpHash = ConstantBufferSize << 16;
			// This is to account for 32vs64 bits difference in pointer sizes.
			TmpHash ^= Align(ResourceOffset, 8);
			uint32 N = Resources.Num();
			while (N >= 4)
			{
				TmpHash ^= (Resources[--N] << 0);
				TmpHash ^= (Resources[--N] << 8);
				TmpHash ^= (Resources[--N] << 16);
				TmpHash ^= (Resources[--N] << 24);
			}
			while (N >= 2)
			{
				TmpHash ^= Resources[--N] << 0;
				TmpHash ^= Resources[--N] << 16;
			}
			while (N > 0)
			{
				TmpHash ^= Resources[--N];
			}
			Hash = TmpHash;
			bComputedHash = true;
		}
		return Hash;
	}

	explicit YRHIUniformBufferLayout(FName InName) :
		ConstantBufferSize(0),
		ResourceOffset(0),
		Name(InName),
		Hash(0),
		bComputedHash(false)
	{
	}

	enum EInit
	{
		Zero
	};
	explicit YRHIUniformBufferLayout(EInit) :
		ConstantBufferSize(0),
		ResourceOffset(0),
		Name(FName()),
		Hash(0),
		bComputedHash(false)
	{
	}

	void CopyFrom(const YRHIUniformBufferLayout& Source)
	{
		ConstantBufferSize = Source.ConstantBufferSize;
		ResourceOffset = Source.ResourceOffset;
		Resources = Source.Resources;
		Name = Source.Name;
		Hash = Source.Hash;
		bComputedHash = Source.bComputedHash;
	}

	const FName GetDebugName() const { return Name; }

private:
	// for debugging / error message
	FName Name;

	mutable uint32 Hash;
	mutable bool bComputedHash;
};

/** Compare two uniform buffer layouts. */
inline bool operator==(const YRHIUniformBufferLayout& A, const YRHIUniformBufferLayout& B)
{
	return A.ConstantBufferSize == B.ConstantBufferSize
		&& A.ResourceOffset == B.ResourceOffset
		&& A.Resources == B.Resources;
}

class YRHIUniformBuffer : public YRHIResource
{
public:

	/** Initialization constructor. */
	YRHIUniformBuffer(const YRHIUniformBufferLayout& InLayout)
		: Layout(&InLayout)
	{}

	/** @return The number of bytes in the uniform buffer. */
	uint32 GetSize() const { return Layout->ConstantBufferSize; }
	const YRHIUniformBufferLayout& GetLayout() const { return *Layout; }
	/** The D3D11 constant buffer resource */
	TRefCountPtr<ID3D11Buffer> Resource;
private:
	/** Layout of the uniform buffer. */
	const YRHIUniformBufferLayout* Layout;
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
class YRHIStructuredBuffer : public YRHIResource
{
public:

	/** Initialization constructor. */
	YRHIStructuredBuffer(uint32 InStride, uint32 InSize, uint32 InUsage)
		: Stride(InStride)
		, Size(InSize)
		, Usage(InUsage)
	{}

	/** @return The stride in bytes of the structured buffer; must be 2 or 4. */
	uint32 GetStride() const { return Stride; }

	/** @return The number of bytes in the structured buffer. */
	uint32 GetSize() const { return Size; }

	/** @return The usage flags used to create the structured buffer. */
	uint32 GetUsage() const { return Usage; }
	TRefCountPtr<ID3D11Buffer> Resource;
private:
	uint32 Stride;
	uint32 Size;
	uint32 Usage;
};


//
// Textures
//

class  YLastRenderTimeContainer
{
public:
	YLastRenderTimeContainer() : LastRenderTime(-FLT_MAX) {}

	double GetLastRenderTime() const { return LastRenderTime; }
	FORCEINLINE_DEBUGGABLE void SetLastRenderTime(double InLastRenderTime)
	{
		// avoid dirty caches from redundant writes
		if (LastRenderTime != InLastRenderTime)
		{
			LastRenderTime = InLastRenderTime;
		}
	}

private:
	/** The last time the resource was rendered. */
	double LastRenderTime;
};

class YRHITexture : public YRHIResource
{
public:

	/** Initialization constructor. */
	YRHITexture(uint32 InNumMips, uint32 InNumSamples, EYPixelFormat InFormat, uint32 InFlags, YLastRenderTimeContainer* InLastRenderTime, const YClearValueBinding& InClearValue)
		: ClearValue(InClearValue)
		, NumMips(InNumMips)
		, NumSamples(InNumSamples)
		, Format(InFormat)
		, Flags(InFlags)
		, LastRenderTime(InLastRenderTime ? *InLastRenderTime : DefaultLastRenderTime)
	{}

	// Dynamic cast methods.
	virtual class YRHITexture2D* GetTexture2D() { return NULL; }
	virtual class YRHITexture2DArray* GetTexture2DArray() { return NULL; }
	virtual class YRHITexture3D* GetTexture3D() { return NULL; }
	virtual class YRHITextureCube* GetTextureCube() { return NULL; }
	virtual class YRHITextureReference* GetTextureReference() { return NULL; }

	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeResource() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/**
	 * Returns access to the platform-specific native shader resource view pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeShaderResourceView() const
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/**
	 * Returns access to the platform-specific RHI texture baseclass.  This is designed to provide the RHI with fast access to its base classes in the face of multiple inheritance.
	 * @return	The pointer to the platform-specific RHI texture baseclass or NULL if it not initialized or not supported for this RHI
	 */
	virtual void* GetTextureBaseRHI()
	{
		// Override this in derived classes to expose access to the native texture resource
		return nullptr;
	}

	/** @return The number of mip-maps in the texture. */
	uint32 GetNumMips() const { return NumMips; }

	/** @return The format of the pixels in the texture. */
	EYPixelFormat GetFormat() const { return Format; }

	/** @return The flags used to create the texture. */
	uint32 GetFlags() const { return Flags; }

	/* @return the number of samples for multi-sampling. */
	uint32 GetNumSamples() const { return NumSamples; }

	/** @return Whether the texture is multi sampled. */
	bool IsMultisampled() const { return NumSamples > 1; }

	YRHIResourceInfo ResourceInfo;

	/** sets the last time this texture was cached in a resource table. */
	FORCEINLINE_DEBUGGABLE void SetLastRenderTime(float InLastRenderTime)
	{
		LastRenderTime.SetLastRenderTime(InLastRenderTime);
	}

	/** Returns the last render time container, or NULL if none were specified at creation. */
	YLastRenderTimeContainer* GetLastRenderTimeContainer()
	{
		if (&LastRenderTime == &DefaultLastRenderTime)
		{
			return NULL;
		}
		return &LastRenderTime;
	}

	void SetName(const FName& InName)
	{
		TextureName = InName;
	}

	FName GetName() const
	{
		return TextureName;
	}

	bool HasClearValue() const
	{
		return ClearValue.ColorBinding != EYClearBinding::YENoneBound;
	}

	FLinearColor GetClearColor() const
	{
		return ClearValue.GetClearColor();
	}

	void GetDepthStencilClearValue(float& OutDepth, uint32& OutStencil) const
	{
		return ClearValue.GetDepthStencil(OutDepth, OutStencil);
	}

	float GetDepthClearValue() const
	{
		float Depth;
		uint32 Stencil;
		ClearValue.GetDepthStencil(Depth, Stencil);
		return Depth;
	}

	uint32 GetStencilClearValue() const
	{
		float Depth;
		uint32 Stencil;
		ClearValue.GetDepthStencil(Depth, Stencil);
		return Stencil;
	}

	const YClearValueBinding GetClearBinding() const
	{
		return ClearValue;
	}
private:
	YClearValueBinding ClearValue;
	uint32 NumMips;
	uint32 NumSamples;
	EYPixelFormat Format;
	uint32 Flags;
	YLastRenderTimeContainer& LastRenderTime;
	YLastRenderTimeContainer DefaultLastRenderTime;
	FName TextureName;
};

class YRHITexture2D : public YRHITexture
{
public:

	/** Initialization constructor. */
	YRHITexture2D(uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, uint32 InNumSamples, EYPixelFormat InFormat, uint32 InFlags, const YClearValueBinding& InClearValue)
		: YRHITexture(InNumMips, InNumSamples, InFormat, InFlags, NULL, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
	{}

	// Dynamic cast methods.
	virtual YRHITexture2D* GetTexture2D() { return this; }

	/** @return The width of the texture. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture. */
	uint32 GetSizeY() const { return SizeY; }

	TRefCountPtr<ID3D11Texture2D> Resource;
private:

	uint32 SizeX;
	uint32 SizeY;
};

class YRHITexture2DArray : public YRHITexture
{
public:

	/** Initialization constructor. */
	YRHITexture2DArray(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, EYPixelFormat InFormat, uint32 InFlags, const YClearValueBinding& InClearValue)
		: YRHITexture(InNumMips, 1, InFormat, InFlags, NULL, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
		, SizeZ(InSizeZ)
	{}

	// Dynamic cast methods.
	virtual YRHITexture2DArray* GetTexture2DArray() { return this; }

	/** @return The width of the textures in the array. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture in the array. */
	uint32 GetSizeY() const { return SizeY; }

	/** @return The number of textures in the array. */
	uint32 GetSizeZ() const { return SizeZ; }
	TRefCountPtr<ID3D11Texture2D> Resource;

private:

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;
};

class  YRHITexture3D : public YRHITexture
{
public:

	/** Initialization constructor. */
	YRHITexture3D(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, uint32 InNumMips, EYPixelFormat InFormat, uint32 InFlags, const YClearValueBinding& InClearValue)
		: YRHITexture(InNumMips, 1, InFormat, InFlags, NULL, InClearValue)
		, SizeX(InSizeX)
		, SizeY(InSizeY)
		, SizeZ(InSizeZ)
	{}

	// Dynamic cast methods.
	virtual YRHITexture3D* GetTexture3D() { return this; }

	/** @return The width of the texture. */
	uint32 GetSizeX() const { return SizeX; }

	/** @return The height of the texture. */
	uint32 GetSizeY() const { return SizeY; }

	/** @return The depth of the texture. */
	uint32 GetSizeZ() const { return SizeZ; }
	TRefCountPtr<ID3D11Texture3D> Resource;
private:

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;
};

class  YRHITextureCube : public YRHITexture
{
public:

	/** Initialization constructor. */
	YRHITextureCube(uint32 InSize, uint32 InNumMips, EYPixelFormat InFormat, uint32 InFlags, const YClearValueBinding& InClearValue)
		: YRHITexture(InNumMips, 1, InFormat, InFlags, NULL, InClearValue)
		, Size(InSize)
	{}

	// Dynamic cast methods.
	virtual YRHITextureCube* GetTextureCube() { return this; }

	/** @return The width and height of each face of the cubemap. */
	uint32 GetSize() const { return Size; }
	TRefCountPtr<ID3D11Texture2D> Resource;
private:

	uint32 Size;
};

class  YRHITextureReference : public YRHITexture
{
public:
	explicit YRHITextureReference(YLastRenderTimeContainer* InLastRenderTime)
		: YRHITexture(0, 0, YPF_Unknown, 0, InLastRenderTime, YClearValueBinding())
	{}

	virtual YRHITextureReference* GetTextureReference() override { return this; }
	inline YRHITexture* GetReferencedTexture() const { return ReferencedTexture.GetReference(); }

	void SetReferencedTexture(YRHITexture* InTexture)
	{
		ReferencedTexture = InTexture;
	}

private:
	TRefCountPtr<YRHITexture> ReferencedTexture;
};

class  YRHITextureReferenceNullImpl : public YRHITextureReference
{
public:
	YRHITextureReferenceNullImpl()
		: YRHITextureReference(NULL)
	{}

	void SetReferencedTexture(YRHITexture* InTexture)
	{
		YRHITextureReference::SetReferencedTexture(InTexture);
	}
};

//
// Misc
//

class YRHIRenderQuery : public YRHIResource {};

class YRHIComputeFence : public YRHIResource
{
public:

	YRHIComputeFence(FName InName)
		: Name(InName)
		, bWriteEnqueued(false)
	{}

	FORCEINLINE FName GetName() const
	{
		return Name;
	}

	FORCEINLINE bool GetWriteEnqueued() const
	{
		return bWriteEnqueued;
	}

	virtual void Reset()
	{
		bWriteEnqueued = false;
	}

	virtual void WriteFence()
	{
		ensureMsgf(!bWriteEnqueued, TEXT("ComputeFence: %s already written this frame. You should use a new label"), *Name.ToString());
		bWriteEnqueued = true;
	}

private:
	//debug name of the label.
	FName Name;

	//has the label been written to since being created.
	//check this when queuing waits to catch GPU hangs on the CPU at command creation time.
	bool bWriteEnqueued;
};

class YRHIViewport : public YRHIResource
{
public:
	YRHIViewport()
		: YRHIResource()
	{
	}
	/**
	 * Returns access to the platform-specific native resource pointer.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeSwapChain() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer texture.  This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferTexture() const { return nullptr; }
	/**
	 * Returns access to the platform-specific native resource pointer to a backbuffer rendertarget. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason
	 */
	virtual void* GetNativeBackBufferRT() const { return nullptr; }

	/**
	 * Returns access to the platform-specific native window. This is designed to be used to provide plugins with access
	 * to the underlying resource and should be used very carefully or not at all.
	 *
	 * @return	The pointer to the native resource or NULL if it not initialized or not supported for this resource type for some reason.
	 * AddParam could represent any additional platform-specific data (could be null).
	 */
	virtual void* GetNativeWindow(void** AddParam = nullptr) const { return nullptr; }

	/**
	 * Sets custom Present handler on the viewport
	 */
	virtual void SetCustomPresent(class FRHICustomPresent*) {}

	/**
	 * Returns currently set custom present handler.
	 */
	virtual class YRHICustomPresent* GetCustomPresent() const { return nullptr; }
};

//
// Views
//
class YRHIShaderResourceView : public YRHIResource
{
public:

	TRefCountPtr<ID3D11ShaderResourceView> View;
	TRefCountPtr<ID3D11Texture2D> Resource;

	YRHIShaderResourceView(ID3D11ShaderResourceView* InView, ID3D11Texture2D* InResource)
		: View(InView)
		, Resource(InResource)
	{}
};

class YRHIUnorderedAccessView : public YRHIResource
{
public:
	TRefCountPtr<ID3D11UnorderedAccessView> View;
	TRefCountPtr<ID3D11Texture2D> Resource;
	void* IHVResourceHandle;

	YRHIUnorderedAccessView(ID3D11UnorderedAccessView* InView, ID3D11Texture2D* InResource)
		: View(InView)
		, Resource(InResource)
		, IHVResourceHandle(nullptr)
	{}
};


typedef YRHISamplerState*              YSamplerStateRHIParamRef;
typedef TRefCountPtr<YRHISamplerState> YSamplerStateRHIRef;

typedef YRHIRasterizerState*              YRasterizerStateRHIParamRef;
typedef TRefCountPtr<YRHIRasterizerState> YRasterizerStateRHIRef;

typedef YRHIDepthStencilState*              YDepthStencilStateRHIParamRef;
typedef TRefCountPtr<YRHIDepthStencilState> YDepthStencilStateRHIRef;

typedef YRHIBlendState*              YBlendStateRHIParamRef;
typedef TRefCountPtr<YRHIBlendState> YBlendStateRHIRef;

typedef YRHIVertexDeclaration*              YVertexDeclarationRHIParamRef;
typedef TRefCountPtr<YRHIVertexDeclaration> YVertexDeclarationRHIRef;

typedef YRHIVertexShader*              YVertexShaderRHIParamRef;
typedef TRefCountPtr<YRHIVertexShader> YVertexShaderRHIRef;

typedef YRHIHullShader*              YHullShaderRHIParamRef;
typedef TRefCountPtr<YRHIHullShader> YHullShaderRHIRef;

typedef YRHIDomainShader*              YDomainShaderRHIParamRef;
typedef TRefCountPtr<YRHIDomainShader> YDomainShaderRHIRef;

typedef YRHIPixelShader*              YPixelShaderRHIParamRef;
typedef TRefCountPtr<YRHIPixelShader> YPixelShaderRHIRef;

typedef YRHIGeometryShader*              YGeometryShaderRHIParamRef;
typedef TRefCountPtr<YRHIGeometryShader> YGeometryShaderRHIRef;

typedef YRHIComputeShader*              YComputeShaderRHIParamRef;
typedef TRefCountPtr<YRHIComputeShader> YComputeShaderRHIRef;

typedef YRHIComputeFence*				YComputeFenceRHIParamRef;
typedef TRefCountPtr<YRHIComputeFence>	YComputeFenceRHIRef;

typedef YRHIBoundShaderState*              YBoundShaderStateRHIParamRef;
typedef TRefCountPtr<YRHIBoundShaderState> YBoundShaderStateRHIRef;

typedef YRHIUniformBuffer*              YUniformBufferRHIParamRef;
typedef TRefCountPtr<YRHIUniformBuffer> YUniformBufferRHIRef;

typedef YRHIIndexBuffer*              YIndexBufferRHIParamRef;
typedef TRefCountPtr<YRHIIndexBuffer> YIndexBufferRHIRef;

typedef YRHIVertexBuffer*              YVertexBufferRHIParamRef;
typedef TRefCountPtr<YRHIVertexBuffer> YVertexBufferRHIRef;

typedef YRHIStructuredBuffer*              YStructuredBufferRHIParamRef;
typedef TRefCountPtr<YRHIStructuredBuffer> YStructuredBufferRHIRef;

typedef YRHITexture*              YTextureRHIParamRef;
typedef TRefCountPtr<YRHITexture> YTextureRHIRef;

typedef YRHITexture2D*              YTexture2DRHIParamRef;
typedef TRefCountPtr<YRHITexture2D> YTexture2DRHIRef;

typedef YRHITexture2DArray*              YTexture2DArrayRHIParamRef;
typedef TRefCountPtr<YRHITexture2DArray> YTexture2DArrayRHIRef;

typedef YRHITexture3D*              YTexture3DRHIParamRef;
typedef TRefCountPtr<YRHITexture3D> YTexture3DRHIRef;

typedef YRHITextureCube*              YTextureCubeRHIParamRef;
typedef TRefCountPtr<YRHITextureCube> YTextureCubeRHIRef;

typedef YRHITextureReference*              YTextureReferenceRHIParamRef;
typedef TRefCountPtr<YRHITextureReference> YTextureReferenceRHIRef;

typedef YRHIRenderQuery*              YRenderQueryRHIParamRef;
typedef TRefCountPtr<YRHIRenderQuery> YRenderQueryRHIRef;

typedef YRHIViewport*              YViewportRHIParamRef;
typedef TRefCountPtr<YRHIViewport> YViewportRHIRef;

typedef YRHIUnorderedAccessView*              YUnorderedAccessViewRHIParamRef;
typedef TRefCountPtr<YRHIUnorderedAccessView> YUnorderedAccessViewRHIRef;

typedef YRHIShaderResourceView*              YShaderResourceViewRHIParamRef;
typedef TRefCountPtr<YRHIShaderResourceView> YShaderResourceViewRHIRef;

typedef YRHIGraphicsPipelineState*              YGraphicsPipelineStateRHIParamRef;
typedef TRefCountPtr<YRHIGraphicsPipelineState> YGraphicsPipelineStateRHIRef;


class YRHIRenderTargetView
{
public:
	YRHITexture* Texture;
	uint32 MipIndex;

	/** Array slice or texture cube face.  Only valid if texture resource was created with TexCreate_TargetArraySlicesIndependently! */
	uint32 ArraySliceIndex;

	EYRenderTargetLoadAction LoadAction;
	EYRenderTargetStoreAction StoreAction;

	YRHIRenderTargetView() :
		Texture(NULL),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(EYRenderTargetLoadAction::YELoad),
		StoreAction(EYRenderTargetStoreAction::YEStore)
	{}

	YRHIRenderTargetView(const YRHIRenderTargetView& Other) :
		Texture(Other.Texture),
		MipIndex(Other.MipIndex),
		ArraySliceIndex(Other.ArraySliceIndex),
		LoadAction(Other.LoadAction),
		StoreAction(Other.StoreAction)
	{}

	YRHIRenderTargetView(YRHITexture* InTexture) :
		Texture(InTexture),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(EYRenderTargetLoadAction::YELoad),
		StoreAction(EYRenderTargetStoreAction::YEStore)
	{}

	YRHIRenderTargetView(YRHITexture* InTexture, uint32 InMipIndex, uint32 InArraySliceIndex) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(EYRenderTargetLoadAction::YELoad),
		StoreAction(EYRenderTargetStoreAction::YEStore)
	{}

	YRHIRenderTargetView(YRHITexture* InTexture, uint32 InMipIndex, uint32 InArraySliceIndex, EYRenderTargetLoadAction InLoadAction, EYRenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(InStoreAction)
	{}

	bool operator==(const YRHIRenderTargetView& Other) const
	{
		return
			Texture == Other.Texture &&
			MipIndex == Other.MipIndex &&
			ArraySliceIndex == Other.ArraySliceIndex &&
			LoadAction == Other.LoadAction &&
			StoreAction == Other.StoreAction;
	}
};

class YExclusiveDepthStencil
{
public:
	enum Type
	{
		// don't use those directly, use the combined versions below
		// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
		YDepthNop = 0x00,
		YDepthRead = 0x01,
		YDepthWrite = 0x02,
		YDepthMask = 0x0f,
		YStencilNop = 0x00,
		YStencilRead = 0x10,
		YStencilWrite = 0x20,
		YStencilMask = 0xf0,

		// use those:
		YDepthNop_StencilNop = YDepthNop + YStencilNop,
		YDepthRead_StencilNop = YDepthRead + YStencilNop,
		YDepthWrite_StencilNop = YDepthWrite + YStencilNop,
		YDepthNop_StencilRead = YDepthNop + YStencilRead,
		YDepthRead_StencilRead = YDepthRead + YStencilRead,
		YDepthWrite_StencilRead = YDepthWrite + YStencilRead,
		YDepthNop_StencilWrite = YDepthNop + YStencilWrite,
		YDepthRead_StencilWrite = YDepthRead + YStencilWrite,
		YDepthWrite_StencilWrite = YDepthWrite + YStencilWrite,
	};

private:
	Type Value;

public:
	// constructor
	YExclusiveDepthStencil(Type InValue = YDepthNop_StencilNop)
		: Value(InValue)
	{
	}

	inline bool IsUsingDepthStencil() const
	{
		return Value != YDepthNop_StencilNop;
	}
	inline bool IsDepthWrite() const
	{
		return ExtractDepth() == YDepthWrite;
	}
	inline bool IsStencilWrite() const
	{
		return ExtractStencil() == YStencilWrite;
	}

	inline bool IsAnyWrite() const
	{
		return IsDepthWrite() || IsStencilWrite();
	}

	inline void SetDepthWrite()
	{
		Value = (Type)(ExtractStencil() | YDepthWrite);
	}
	inline void SetStencilWrite()
	{
		Value = (Type)(ExtractDepth() | YStencilWrite);
	}
	inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
	{
		Value = YDepthNop_StencilNop;

		if (bDepth)
		{
			SetDepthWrite();
		}
		if (bStencil)
		{
			SetStencilWrite();
		}
	}
	bool operator==(const YExclusiveDepthStencil& rhs) const
	{
		return Value == rhs.Value;
	}
	inline bool IsValid(YExclusiveDepthStencil& Current) const
	{
		Type Depth = ExtractDepth();

		if (Depth != YDepthNop && Depth != Current.ExtractDepth())
		{
			return false;
		}

		Type Stencil = ExtractStencil();

		if (Stencil != YStencilNop && Stencil != Current.ExtractStencil())
		{
			return false;
		}

		return true;
	}

	uint32 GetIndex() const
	{
		// Note: The array to index has views created in that specific order.

		// we don't care about the Nop versions so less views are needed
		// we combine Nop and Write
		switch (Value)
		{
		case YDepthWrite_StencilNop:
		case YDepthNop_StencilWrite:
		case YDepthWrite_StencilWrite:
		case YDepthNop_StencilNop:
			return 0; // old DSAT_Writable

		case YDepthRead_StencilNop:
		case YDepthRead_StencilWrite:
			return 1; // old DSAT_ReadOnlyDepth

		case YDepthNop_StencilRead:
		case YDepthWrite_StencilRead:
			return 2; // old DSAT_ReadOnlyStencil

		case YDepthRead_StencilRead:
			return 3; // old DSAT_ReadOnlyDepthAndStencil
		}
		// should never happen
		check(0);
		return -1;
	}
	static const uint32 MaxIndex = 4;

private:
	inline Type ExtractDepth() const
	{
		return (Type)(Value & YDepthMask);
	}
	inline Type ExtractStencil() const
	{
		return (Type)(Value & YStencilMask);
	}
};

class YRHIDepthRenderTargetView
{
public:
	YTextureRHIParamRef Texture;

	EYRenderTargetLoadAction		DepthLoadAction;
	EYRenderTargetStoreAction	DepthStoreAction;
	EYRenderTargetLoadAction		StencilLoadAction;

private:
	EYRenderTargetStoreAction	StencilStoreAction;
	YExclusiveDepthStencil		DepthStencilAccess;
public:

	// accessor to prevent write access to StencilStoreAction
	EYRenderTargetStoreAction GetStencilStoreAction() const { return StencilStoreAction; }
	// accessor to prevent write access to DepthStencilAccess
	YExclusiveDepthStencil GetDepthStencilAccess() const { return DepthStencilAccess; }

	YRHIDepthRenderTargetView() :
		Texture(nullptr),
		DepthLoadAction(EYRenderTargetLoadAction::YEClear),
		DepthStoreAction(EYRenderTargetStoreAction::YEStore),
		StencilLoadAction(EYRenderTargetLoadAction::YEClear),
		StencilStoreAction(EYRenderTargetStoreAction::YEStore),
		DepthStencilAccess(YExclusiveDepthStencil::YDepthWrite_StencilWrite)
	{
		Validate();
	}

	YRHIDepthRenderTargetView(YTextureRHIParamRef InTexture) :
		Texture(InTexture),
		DepthLoadAction(EYRenderTargetLoadAction::YEClear),
		DepthStoreAction(EYRenderTargetStoreAction::YEStore),
		StencilLoadAction(EYRenderTargetLoadAction::YEClear),
		StencilStoreAction(EYRenderTargetStoreAction::YEStore),
		DepthStencilAccess(YExclusiveDepthStencil::YDepthWrite_StencilWrite)
	{
		Validate();
	}

	YRHIDepthRenderTargetView(YTextureRHIParamRef InTexture, EYRenderTargetLoadAction InLoadAction, EYRenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(YExclusiveDepthStencil::YDepthWrite_StencilWrite)
	{
		Validate();
	}

	YRHIDepthRenderTargetView(YTextureRHIParamRef InTexture, EYRenderTargetLoadAction InLoadAction, EYRenderTargetStoreAction InStoreAction, YExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	YRHIDepthRenderTargetView(YTextureRHIParamRef InTexture, EYRenderTargetLoadAction InDepthLoadAction, EYRenderTargetStoreAction InDepthStoreAction, EYRenderTargetLoadAction InStencilLoadAction, EYRenderTargetStoreAction InStencilStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(YExclusiveDepthStencil::YDepthWrite_StencilWrite)
	{
		Validate();
	}

	YRHIDepthRenderTargetView(YTextureRHIParamRef InTexture, EYRenderTargetLoadAction InDepthLoadAction, EYRenderTargetStoreAction InDepthStoreAction, EYRenderTargetLoadAction InStencilLoadAction, EYRenderTargetStoreAction InStencilStoreAction, YExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	void Validate() const
	{
		ensureMsgf(DepthStencilAccess.IsDepthWrite() || DepthStoreAction == EYRenderTargetStoreAction::YENoAction, TEXT("Depth is read-only, but we are performing a store.  This is a waste on mobile.  If depth can't change, we don't need to store it out again"));
		ensureMsgf(DepthStencilAccess.IsStencilWrite() || StencilStoreAction == EYRenderTargetStoreAction::YENoAction, TEXT("Stencil is read-only, but we are performing a store.  This is a waste on mobile.  If stencil can't change, we don't need to store it out again"));
	}

	bool operator==(const YRHIDepthRenderTargetView& Other) const
	{
		return
			Texture == Other.Texture &&
			DepthLoadAction == Other.DepthLoadAction &&
			DepthStoreAction == Other.DepthStoreAction &&
			StencilLoadAction == Other.StencilLoadAction &&
			StencilStoreAction == Other.StencilStoreAction &&
			DepthStencilAccess == Other.DepthStencilAccess;
	}
};



class YRHISetRenderTargetsInfo
{
public:
	// Color Render Targets Info
	YRHIRenderTargetView ColorRenderTarget[YMaxSimultaneousRenderTargets];
	int32 NumColorRenderTargets;
	bool bClearColor;

	// Depth/Stencil Render Target Info
	YRHIDepthRenderTargetView DepthStencilRenderTarget;
	bool bClearDepth;
	bool bClearStencil;

	// UAVs info.
	YUnorderedAccessViewRHIRef UnorderedAccessView[YMaxSimultaneousUAVs];
	int32 NumUAVs;

	YRHISetRenderTargetsInfo() :
		NumColorRenderTargets(0),
		bClearColor(false),
		bClearDepth(false),
		bClearStencil(false),
		NumUAVs(0)
	{}

	YRHISetRenderTargetsInfo(int32 InNumColorRenderTargets, const YRHIRenderTargetView* InColorRenderTargets, const YRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
		NumColorRenderTargets(InNumColorRenderTargets),
		bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == EYRenderTargetLoadAction::YEClear),
		DepthStencilRenderTarget(InDepthStencilRenderTarget),
		bClearDepth(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.DepthLoadAction == EYRenderTargetLoadAction::YEClear),
		bClearStencil(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.StencilLoadAction == EYRenderTargetLoadAction::YEClear),
		NumUAVs(0)
	{
		check(InNumColorRenderTargets <= 0 || InColorRenderTargets);
		for (int32 Index = 0; Index < InNumColorRenderTargets; ++Index)
		{
			ColorRenderTarget[Index] = InColorRenderTargets[Index];
		}
	}
	// @todo metal mrt: This can go away after all the cleanup is done
	void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
	{
		if (bInClearDepth)
		{
			DepthStencilRenderTarget.DepthLoadAction = EYRenderTargetLoadAction::YEClear;
		}
		if (bInClearStencil)
		{
			DepthStencilRenderTarget.StencilLoadAction = EYRenderTargetLoadAction::YEClear;
		}
		bClearDepth = bInClearDepth;
		bClearStencil = bInClearStencil;
	}

	uint32 CalculateHash() const
	{
		// Need a separate struct so we can memzero/remove dependencies on reference counts
		struct FHashableStruct
		{
			// Depth goes in the last slot
			YRHITexture* Texture[YMaxSimultaneousRenderTargets + 1];
			uint32 MipIndex[YMaxSimultaneousRenderTargets];
			uint32 ArraySliceIndex[YMaxSimultaneousRenderTargets];
			EYRenderTargetLoadAction LoadAction[YMaxSimultaneousRenderTargets];
			EYRenderTargetStoreAction StoreAction[YMaxSimultaneousRenderTargets];

			EYRenderTargetLoadAction		DepthLoadAction;
			EYRenderTargetStoreAction	DepthStoreAction;
			EYRenderTargetLoadAction		StencilLoadAction;
			EYRenderTargetStoreAction	StencilStoreAction;
			YExclusiveDepthStencil		DepthStencilAccess;

			bool bClearDepth;
			bool bClearStencil;
			bool bClearColor;
			YRHIUnorderedAccessView* UnorderedAccessView[YMaxSimultaneousUAVs];

			void Set(const YRHISetRenderTargetsInfo& RTInfo)
			{
				FMemory::Memzero(*this);
				for (int32 Index = 0; Index < RTInfo.NumColorRenderTargets; ++Index)
				{
					Texture[Index] = RTInfo.ColorRenderTarget[Index].Texture;
					MipIndex[Index] = RTInfo.ColorRenderTarget[Index].MipIndex;
					ArraySliceIndex[Index] = RTInfo.ColorRenderTarget[Index].ArraySliceIndex;
					LoadAction[Index] = RTInfo.ColorRenderTarget[Index].LoadAction;
					StoreAction[Index] = RTInfo.ColorRenderTarget[Index].StoreAction;
				}

				Texture[YMaxSimultaneousRenderTargets] = RTInfo.DepthStencilRenderTarget.Texture;
				DepthLoadAction = RTInfo.DepthStencilRenderTarget.DepthLoadAction;
				DepthStoreAction = RTInfo.DepthStencilRenderTarget.DepthStoreAction;
				StencilLoadAction = RTInfo.DepthStencilRenderTarget.StencilLoadAction;
				StencilStoreAction = RTInfo.DepthStencilRenderTarget.GetStencilStoreAction();
				DepthStencilAccess = RTInfo.DepthStencilRenderTarget.GetDepthStencilAccess();

				bClearDepth = RTInfo.bClearDepth;
				bClearStencil = RTInfo.bClearStencil;
				bClearColor = RTInfo.bClearColor;

				for (int32 Index = 0; Index < YMaxSimultaneousUAVs; ++Index)
				{
					UnorderedAccessView[Index] = RTInfo.UnorderedAccessView[Index];
				}
			}
		};

		FHashableStruct RTHash;
		FMemory::Memzero(RTHash);
		RTHash.Set(*this);
		return FCrc::MemCrc32(&RTHash, sizeof(RTHash));
	}
};

struct YBoundShaderStateInput
{
	YVertexDeclarationRHIParamRef VertexDeclarationRHI;
	YVertexShaderRHIParamRef VertexShaderRHI;
	YHullShaderRHIParamRef HullShaderRHI;
	YDomainShaderRHIParamRef DomainShaderRHI;
	YPixelShaderRHIParamRef PixelShaderRHI;
	YGeometryShaderRHIParamRef GeometryShaderRHI;

	FORCEINLINE YBoundShaderStateInput()
		: VertexDeclarationRHI(nullptr)
		, VertexShaderRHI(nullptr)
		, HullShaderRHI(nullptr)
		, DomainShaderRHI(nullptr)
		, PixelShaderRHI(nullptr)
		, GeometryShaderRHI(nullptr)
	{
	}

	FORCEINLINE YBoundShaderStateInput(
		YVertexDeclarationRHIParamRef InVertexDeclarationRHI,
		YVertexShaderRHIParamRef InVertexShaderRHI,
		YHullShaderRHIParamRef InHullShaderRHI,
		YDomainShaderRHIParamRef InDomainShaderRHI,
		YPixelShaderRHIParamRef InPixelShaderRHI,
		YGeometryShaderRHIParamRef InGeometryShaderRHI
	)
		: VertexDeclarationRHI(InVertexDeclarationRHI)
		, VertexShaderRHI(InVertexShaderRHI)
		, HullShaderRHI(InHullShaderRHI)
		, DomainShaderRHI(InDomainShaderRHI)
		, PixelShaderRHI(InPixelShaderRHI)
		, GeometryShaderRHI(InGeometryShaderRHI)
	{
	}
};

class YGraphicsPipelineStateInitializer
{
public:
	enum YOptionalState : uint32
	{
		YOS_SetStencilRef = 1 << 0,
		YOS_SetBlendFactor = 1 << 1,
	};

	using TRenderTargetFormats = TStaticArray<EYPixelFormat, YMaxSimultaneousRenderTargets>;
	using TRenderTargetFlags = TStaticArray<uint32, YMaxSimultaneousRenderTargets>;
	using TRenderTargetLoadActions = TStaticArray<EYRenderTargetLoadAction, YMaxSimultaneousRenderTargets>;
	using TRenderTargetStoreActions = TStaticArray<EYRenderTargetStoreAction, YMaxSimultaneousRenderTargets>;

	YGraphicsPipelineStateInitializer()
		: BlendState(nullptr)
		, RasterizerState(nullptr)
		, DepthStencilState(nullptr)
		, PrimitiveType(YPT_Num)
		, RenderTargetsEnabled(0)
		, DepthStencilTargetFormat(YPF_Unknown)
		, DepthStencilTargetFlag(0)
		, DepthStencilTargetLoadAction(EYRenderTargetLoadAction::YENoAction)
		, DepthStencilTargetStoreAction(EYRenderTargetStoreAction::YENoAction)
		, NumSamples(0)
		, OptState(0)
		, StencilRef(0)
		, BlendFactor(1.0f, 1.0f, 1.0f)
	{
		for (uint32 i = 0; i < YMaxSimultaneousRenderTargets; ++i)
		{
			RenderTargetFormats[i] = YPF_Unknown;
			RenderTargetFlags[i] = 0;
			RenderTargetLoadActions[i] = EYRenderTargetLoadAction::YENoAction;
			RenderTargetStoreActions[i] = EYRenderTargetStoreAction::YENoAction;
		}
	}

	YGraphicsPipelineStateInitializer(
		YBoundShaderStateInput				InBoundShaderState,
		YBlendStateRHIParamRef				InBlendState,
		YRasterizerStateRHIParamRef			InRasterizerState,
		YDepthStencilStateRHIParamRef		InDepthStencilState,
		uint32								InStencilRef,
		FLinearColor						InBlendFactor,
		EYPrimitiveType						InPrimitiveType,
		uint32								InRenderTargetsEnabled,
		const TRenderTargetFormats&			InRenderTargetFormats,
		const TRenderTargetFlags&			InRenderTargetFlags,
		const TRenderTargetLoadActions&		InRenderTargetLoadActions,
		const TRenderTargetStoreActions&	InRenderTargetStoreActions,
		EYPixelFormat						InDepthStencilTargetFormat,
		uint32								InDepthStencilTargetFlag,
		EYRenderTargetLoadAction			InDepthStencilTargetLoadAction,
		EYRenderTargetStoreAction			InDepthStencilTargetStoreAction,
		uint32								InNumSamples
		)
		: BoundShaderState(InBoundShaderState)
		, BlendState(InBlendState)
		, RasterizerState(InRasterizerState)
		, DepthStencilState(InDepthStencilState)
		, PrimitiveType(InPrimitiveType)
		, RenderTargetsEnabled(InRenderTargetsEnabled)
		, RenderTargetFormats(InRenderTargetFormats)
		, RenderTargetFlags(InRenderTargetFlags)
		, RenderTargetLoadActions(InRenderTargetLoadActions)
		, RenderTargetStoreActions(InRenderTargetStoreActions)
		, DepthStencilTargetFormat(InDepthStencilTargetFormat)
		, DepthStencilTargetFlag(InDepthStencilTargetFlag)
		, DepthStencilTargetLoadAction(InDepthStencilTargetLoadAction)
		, DepthStencilTargetStoreAction(InDepthStencilTargetStoreAction)
		, NumSamples(InNumSamples)
		, OptState(YOptionalState::YOS_SetStencilRef | YOptionalState::YOS_SetBlendFactor)
		, StencilRef(InStencilRef)
		, BlendFactor(InBlendFactor)
	{
	}

	bool operator==(YGraphicsPipelineStateInitializer& rhs)
	{
		if (BoundShaderState.VertexDeclarationRHI != rhs.BoundShaderState.VertexDeclarationRHI || 
			BoundShaderState.VertexShaderRHI != rhs.BoundShaderState.VertexShaderRHI ||
			BoundShaderState.PixelShaderRHI != rhs.BoundShaderState.PixelShaderRHI ||
			BoundShaderState.GeometryShaderRHI != rhs.BoundShaderState.GeometryShaderRHI ||
			BoundShaderState.DomainShaderRHI != rhs.BoundShaderState.DomainShaderRHI ||
			BoundShaderState.HullShaderRHI != rhs.BoundShaderState.HullShaderRHI ||
			BlendState != rhs.BlendState || 
			RasterizerState != rhs.RasterizerState || 
			DepthStencilState != rhs.DepthStencilState ||
			StencilRef != rhs.StencilRef || 
			BlendFactor != rhs.BlendFactor || 
			PrimitiveType != rhs.PrimitiveType || 
			RenderTargetsEnabled != rhs.RenderTargetsEnabled ||
			RenderTargetFormats != rhs.RenderTargetFormats || 
			RenderTargetFlags != rhs.RenderTargetFlags || 
			RenderTargetLoadActions != rhs.RenderTargetLoadActions ||
			RenderTargetStoreActions != rhs.RenderTargetStoreActions || 
			DepthStencilTargetFormat != rhs.DepthStencilTargetFormat || 
			DepthStencilTargetFlag != rhs.DepthStencilTargetFlag ||
			DepthStencilTargetLoadAction != rhs.DepthStencilTargetLoadAction || 
			DepthStencilTargetStoreAction != rhs.DepthStencilTargetStoreAction || 
			NumSamples != rhs.NumSamples ||
			OptState != rhs.OptState) 
		{
			return false;
		}

		return true;
	}

#define COMPARE_FIELD_BEGIN(Field) \
		if (Field != rhs.Field) \
		{ return Field COMPARE_OP rhs.Field; }

#define COMPARE_FIELD(Field) \
		else if (Field != rhs.Field) \
		{ return Field COMPARE_OP rhs.Field; }

#define COMPARE_FIELD_END \
		else { return false; }

	bool operator<(YGraphicsPipelineStateInitializer& rhs) const
	{
#define COMPARE_OP <

		COMPARE_FIELD_BEGIN(BoundShaderState.VertexDeclarationRHI)
		COMPARE_FIELD(BoundShaderState.VertexShaderRHI)
		COMPARE_FIELD(BoundShaderState.PixelShaderRHI)
		COMPARE_FIELD(BoundShaderState.GeometryShaderRHI)
		COMPARE_FIELD(BoundShaderState.DomainShaderRHI)
		COMPARE_FIELD(BoundShaderState.HullShaderRHI)
		COMPARE_FIELD(BlendState)
		COMPARE_FIELD(RasterizerState)
		COMPARE_FIELD(DepthStencilState)
		COMPARE_FIELD(PrimitiveType)
		COMPARE_FIELD_END;

#undef COMPARE_OP
	}

	bool operator>(YGraphicsPipelineStateInitializer& rhs) const
	{
#define COMPARE_OP >

		COMPARE_FIELD_BEGIN(BoundShaderState.VertexDeclarationRHI)
		COMPARE_FIELD(BoundShaderState.VertexShaderRHI)
		COMPARE_FIELD(BoundShaderState.PixelShaderRHI)
		COMPARE_FIELD(BoundShaderState.GeometryShaderRHI)
		COMPARE_FIELD(BoundShaderState.DomainShaderRHI)
		COMPARE_FIELD(BoundShaderState.HullShaderRHI)
		COMPARE_FIELD(BlendState)
		COMPARE_FIELD(RasterizerState)
		COMPARE_FIELD(DepthStencilState)
		COMPARE_FIELD(PrimitiveType)
		COMPARE_FIELD_END;

#undef COMPARE_OP
	}

#undef COMPARE_FIELD_BEGIN
#undef COMPARE_FIELD
#undef COMPARE_FIELD_END

	uint32 GetOptionalSetState()
	{
		return OptState;
	}

	uint32 GetStencilRef() const
	{
		return StencilRef;
	}

	void SetStencilRef(uint32 InStencilRef)
	{
		OptState |= YOptionalState::YOS_SetStencilRef;
		StencilRef = InStencilRef;
	}

	void ClearSetStencilRef()
	{
		OptState &= ~YOptionalState::YOS_SetStencilRef;
	}

	FLinearColor GetBlendFactor() const
	{
		return BlendFactor;
	}

	void SetBlendFactor(FLinearColor InBlendFactor)
	{
		OptState |= YOptionalState::YOS_SetBlendFactor;
		BlendFactor = InBlendFactor;
	}

	void ClearSetBlendFactor()
	{
		OptState &= ~YOptionalState::YOS_SetBlendFactor;
	}

	// TODO: [PSO API] - As we migrate reuse existing API objects, but eventually we can move to the direct initializers. 
	// When we do that work, move this to RHI.h as its more appropriate there, but here for now since dependent typdefs are here.
	YBoundShaderStateInput			BoundShaderState;
	YBlendStateRHIParamRef			BlendState;
	YRasterizerStateRHIParamRef		RasterizerState;
	YDepthStencilStateRHIParamRef	DepthStencilState;
	EYPrimitiveType					PrimitiveType;
	uint32							RenderTargetsEnabled;
	TRenderTargetFormats			RenderTargetFormats;
	TRenderTargetFlags				RenderTargetFlags;
	TRenderTargetLoadActions		RenderTargetLoadActions;
	TRenderTargetStoreActions		RenderTargetStoreActions;
	EYPixelFormat					DepthStencilTargetFormat;
	uint32							DepthStencilTargetFlag;
	EYRenderTargetLoadAction		DepthStencilTargetLoadAction;
	EYRenderTargetStoreAction		DepthStencilTargetStoreAction;
	uint32							NumSamples;

private:
	uint32							OptState;
	uint32							StencilRef;
	FLinearColor					BlendFactor;

	friend class YMeshDrawingPolicy;
};
