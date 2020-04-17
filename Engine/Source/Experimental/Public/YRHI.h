#pragma once
#include "Core.h"
#include "Containers/ResourceArray.h"
#include <d3d11.h>
#include "YRHIDefinitions.h"


struct YVertexElement
{
	uint8 StreamIndex;
	uint8 Offset;
	TEnumAsByte<EYVertexElementType> Type;
	uint8 AttributeIndex;
	uint16 Stride; //整体一个Stream 元素的大小，dx11的api没有用
	/**
	 * Whether to use instance index or vertex index to consume the element.
	 * eg if bUseInstanceIndex is 0, the element will be repeated for every instance.
	 */
	uint16 bUseInstanceIndex;

	YVertexElement() {}
	YVertexElement(uint8 InStreamIndex, uint8 InOffset, EYVertexElementType InType, uint8 InAttributeIndex, uint16 InStride, bool bInUseInstanceIndex = false) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride),
		bUseInstanceIndex(bInUseInstanceIndex)
	{}
	/**
	* Suppress the compiler generated assignment operator so that padding won't be copied.
	* This is necessary to get expected results for code that zeros, assigns and then CRC's the whole struct.
	*/
	void operator=(const YVertexElement& Other)
	{
		StreamIndex = Other.StreamIndex;
		Offset = Other.Offset;
		Type = Other.Type;
		AttributeIndex = Other.AttributeIndex;
		bUseInstanceIndex = Other.bUseInstanceIndex;
	}

	friend FArchive& operator<<(FArchive& Ar, YVertexElement& Element)
	{
		Ar << Element.StreamIndex;
		Ar << Element.Offset;
		Ar << Element.Type;
		Ar << Element.AttributeIndex;
		Ar << Element.Stride;
		Ar << Element.bUseInstanceIndex;
		return Ar;
	}
};
typedef TArray<YVertexElement, TFixedAllocator<YMaxVertexElementCount> > YVertexDeclarationElementList;


struct YSamplerStateInitializerRHI
{
	YSamplerStateInitializerRHI() {}
	YSamplerStateInitializerRHI(
		EYSamplerFilter InFilter,
		EYSamplerAddressMode InAddressU = YAM_Wrap,
		EYSamplerAddressMode InAddressV = YAM_Wrap,
		EYSamplerAddressMode InAddressW = YAM_Wrap,
		int32 InMipBias = 0,
		int32 InMaxAnisotropy = 0,
		float InMinMipLevel = 0,
		float InMaxMipLevel = FLT_MAX,
		uint32 InBorderColor = 0,
		/** Only supported in D3D11 */
		EYSamplerCompareFunction InSamplerComparisonFunction = YSCF_Never
	)
		: Filter(InFilter)
		, AddressU(InAddressU)
		, AddressV(InAddressV)
		, AddressW(InAddressW)
		, MipBias(InMipBias)
		, MinMipLevel(InMinMipLevel)
		, MaxMipLevel(InMaxMipLevel)
		, MaxAnisotropy(InMaxAnisotropy)
		, BorderColor(InBorderColor)
		, SamplerComparisonFunction(InSamplerComparisonFunction)
	{
	}
	TEnumAsByte<EYSamplerFilter> Filter;
	TEnumAsByte<EYSamplerAddressMode> AddressU;
	TEnumAsByte<EYSamplerAddressMode> AddressV;
	TEnumAsByte<EYSamplerAddressMode> AddressW;
	int32 MipBias;
	/** Smallest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MinMipLevel;
	/** Largest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MaxMipLevel;
	int32 MaxAnisotropy;
	uint32 BorderColor;
	TEnumAsByte<EYSamplerCompareFunction> SamplerComparisonFunction;

	friend FArchive& operator<<(FArchive& Ar, YSamplerStateInitializerRHI& SamplerStateInitializer)
	{
		Ar << SamplerStateInitializer.Filter;
		Ar << SamplerStateInitializer.AddressU;
		Ar << SamplerStateInitializer.AddressV;
		Ar << SamplerStateInitializer.AddressW;
		Ar << SamplerStateInitializer.MipBias;
		Ar << SamplerStateInitializer.MinMipLevel;
		Ar << SamplerStateInitializer.MaxMipLevel;
		Ar << SamplerStateInitializer.MaxAnisotropy;
		Ar << SamplerStateInitializer.BorderColor;
		Ar << SamplerStateInitializer.SamplerComparisonFunction;
		return Ar;
	}
};

struct YRasterizerStateInitializerRHI
{
	TEnumAsByte<EYRasterizerFillMode> FillMode;
	TEnumAsByte<EYRasterizerCullMode> CullMode;
	float DepthBias;
	float SlopeScaleDepthBias;
	bool bAllowMSAA;
	bool bEnableLineAA;

	friend FArchive& operator<<(FArchive& Ar, YRasterizerStateInitializerRHI& RasterizerStateInitializer)
	{
		Ar << RasterizerStateInitializer.FillMode;
		Ar << RasterizerStateInitializer.CullMode;
		Ar << RasterizerStateInitializer.DepthBias;
		Ar << RasterizerStateInitializer.SlopeScaleDepthBias;
		Ar << RasterizerStateInitializer.bAllowMSAA;
		Ar << RasterizerStateInitializer.bEnableLineAA;
		return Ar;
	}
};

struct YDepthStencilStateInitializerRHI
{
	bool bEnableDepthWrite;
	TEnumAsByte<EYCompareFunction> DepthTest;

	bool bEnableFrontFaceStencil;
	TEnumAsByte<EYCompareFunction> FrontFaceStencilTest;
	TEnumAsByte<EYStencilOp> FrontFaceStencilFailStencilOp;
	TEnumAsByte<EYStencilOp> FrontFaceDepthFailStencilOp;
	TEnumAsByte<EYStencilOp> FrontFacePassStencilOp;
	bool bEnableBackFaceStencil;
	TEnumAsByte<EYCompareFunction> BackFaceStencilTest;
	TEnumAsByte<EYStencilOp> BackFaceStencilFailStencilOp;
	TEnumAsByte<EYStencilOp> BackFaceDepthFailStencilOp;
	TEnumAsByte<EYStencilOp> BackFacePassStencilOp;
	uint8 StencilReadMask;
	uint8 StencilWriteMask;

	YDepthStencilStateInitializerRHI(
		bool bInEnableDepthWrite = true,
		EYCompareFunction InDepthTest = YCF_LessEqual,
		bool bInEnableFrontFaceStencil = false,
		EYCompareFunction InFrontFaceStencilTest = YCF_Always,
		EYStencilOp InFrontFaceStencilFailStencilOp = YSO_Keep,
		EYStencilOp InFrontFaceDepthFailStencilOp = YSO_Keep,
		EYStencilOp InFrontFacePassStencilOp = YSO_Keep,
		bool bInEnableBackFaceStencil = false,
		EYCompareFunction InBackFaceStencilTest = YCF_Always,
		EYStencilOp InBackFaceStencilFailStencilOp = YSO_Keep,
		EYStencilOp InBackFaceDepthFailStencilOp = YSO_Keep,
		EYStencilOp InBackFacePassStencilOp = YSO_Keep,
		uint8 InStencilReadMask = 0xFF,
		uint8 InStencilWriteMask = 0xFF
	)
		: bEnableDepthWrite(bInEnableDepthWrite)
		, DepthTest(InDepthTest)
		, bEnableFrontFaceStencil(bInEnableFrontFaceStencil)
		, FrontFaceStencilTest(InFrontFaceStencilTest)
		, FrontFaceStencilFailStencilOp(InFrontFaceStencilFailStencilOp)
		, FrontFaceDepthFailStencilOp(InFrontFaceDepthFailStencilOp)
		, FrontFacePassStencilOp(InFrontFacePassStencilOp)
		, bEnableBackFaceStencil(bInEnableBackFaceStencil)
		, BackFaceStencilTest(InBackFaceStencilTest)
		, BackFaceStencilFailStencilOp(InBackFaceStencilFailStencilOp)
		, BackFaceDepthFailStencilOp(InBackFaceDepthFailStencilOp)
		, BackFacePassStencilOp(InBackFacePassStencilOp)
		, StencilReadMask(InStencilReadMask)
		, StencilWriteMask(InStencilWriteMask)
	{}

	friend FArchive& operator<<(FArchive& Ar, YDepthStencilStateInitializerRHI& DepthStencilStateInitializer)
	{
		Ar << DepthStencilStateInitializer.bEnableDepthWrite;
		Ar << DepthStencilStateInitializer.DepthTest;
		Ar << DepthStencilStateInitializer.bEnableFrontFaceStencil;
		Ar << DepthStencilStateInitializer.FrontFaceStencilTest;
		Ar << DepthStencilStateInitializer.FrontFaceStencilFailStencilOp;
		Ar << DepthStencilStateInitializer.FrontFaceDepthFailStencilOp;
		Ar << DepthStencilStateInitializer.FrontFacePassStencilOp;
		Ar << DepthStencilStateInitializer.bEnableBackFaceStencil;
		Ar << DepthStencilStateInitializer.BackFaceStencilTest;
		Ar << DepthStencilStateInitializer.BackFaceStencilFailStencilOp;
		Ar << DepthStencilStateInitializer.BackFaceDepthFailStencilOp;
		Ar << DepthStencilStateInitializer.BackFacePassStencilOp;
		Ar << DepthStencilStateInitializer.StencilReadMask;
		Ar << DepthStencilStateInitializer.StencilWriteMask;
		return Ar;
	}
};

class YBlendStateInitializerRHI
{
public:

	struct YRenderTarget
	{
		TEnumAsByte<EYBlendOperation> ColorBlendOp;
		TEnumAsByte<EYBlendFactor> ColorSrcBlend;
		TEnumAsByte<EYBlendFactor> ColorDestBlend;
		TEnumAsByte<EYBlendOperation> AlphaBlendOp;
		TEnumAsByte<EYBlendFactor> AlphaSrcBlend;
		TEnumAsByte<EYBlendFactor> AlphaDestBlend;
		TEnumAsByte<EYColorWriteMask> ColorWriteMask;

		YRenderTarget(
			EYBlendOperation InColorBlendOp = YBO_Add,
			EYBlendFactor InColorSrcBlend = YBF_One,
			EYBlendFactor InColorDestBlend = YBF_Zero,
			EYBlendOperation InAlphaBlendOp = YBO_Add,
			EYBlendFactor InAlphaSrcBlend = YBF_One,
			EYBlendFactor InAlphaDestBlend = YBF_Zero,
			EYColorWriteMask InColorWriteMask = YCW_RGBA
		)
			: ColorBlendOp(InColorBlendOp)
			, ColorSrcBlend(InColorSrcBlend)
			, ColorDestBlend(InColorDestBlend)
			, AlphaBlendOp(InAlphaBlendOp)
			, AlphaSrcBlend(InAlphaSrcBlend)
			, AlphaDestBlend(InAlphaDestBlend)
			, ColorWriteMask(InColorWriteMask)
		{}

		friend FArchive& operator<<(FArchive& Ar, YRenderTarget& RenderTarget)
		{
			Ar << RenderTarget.ColorBlendOp;
			Ar << RenderTarget.ColorSrcBlend;
			Ar << RenderTarget.ColorDestBlend;
			Ar << RenderTarget.AlphaBlendOp;
			Ar << RenderTarget.AlphaSrcBlend;
			Ar << RenderTarget.AlphaDestBlend;
			Ar << RenderTarget.ColorWriteMask;
			return Ar;
		}
	};

	YBlendStateInitializerRHI() {}

	YBlendStateInitializerRHI(const YRenderTarget& InRenderTargetBlendState)
		: bUseIndependentRenderTargetBlendStates(false)
	{
		RenderTargets[0] = InRenderTargetBlendState;
	}

	template<uint32 NumRenderTargets>
	YBlendStateInitializerRHI(const TStaticArray<YRenderTarget, NumRenderTargets>& InRenderTargetBlendStates)
		: bUseIndependentRenderTargetBlendStates(NumRenderTargets > 1)
	{
		static_assert(NumRenderTargets <= YMaxSimultaneousRenderTargets, "Too many render target blend states.");

		for (uint32 RenderTargetIndex = 0; RenderTargetIndex < NumRenderTargets; ++RenderTargetIndex)
		{
			RenderTargets[RenderTargetIndex] = InRenderTargetBlendStates[RenderTargetIndex];
		}
	}

	TStaticArray<YRenderTarget, YMaxSimultaneousRenderTargets> RenderTargets;
	bool bUseIndependentRenderTargetBlendStates;

	friend FArchive& operator<<(FArchive& Ar, YBlendStateInitializerRHI& BlendStateInitializer)
	{
		Ar << BlendStateInitializer.RenderTargets;
		Ar << BlendStateInitializer.bUseIndependentRenderTargetBlendStates;
		return Ar;
	}
};


/**
 *	Screen Resolution
 */
struct YScreenResolutionRHI
{
	uint32	Width;
	uint32	Height;
	uint32	RefreshRate;
};

/**
 *	Viewport bounds structure to set multiple view ports for the geometry shader
 *  (needs to be 1:1 to the D3D11 structure)
 */
struct YViewportBounds
{
	float	TopLeftX;
	float	TopLeftY;
	float	Width;
	float	Height;
	float	MinDepth;
	float	MaxDepth;

	YViewportBounds() {}

	YViewportBounds(float InTopLeftX, float InTopLeftY, float InWidth, float InHeight, float InMinDepth = 0.0f, float InMaxDepth = 1.0f)
		:TopLeftX(InTopLeftX), TopLeftY(InTopLeftY), Width(InWidth), Height(InHeight), MinDepth(InMinDepth), MaxDepth(InMaxDepth)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, YViewportBounds& ViewportBounds)
	{
		Ar << ViewportBounds.TopLeftX;
		Ar << ViewportBounds.TopLeftY;
		Ar << ViewportBounds.Width;
		Ar << ViewportBounds.Height;
		Ar << ViewportBounds.MinDepth;
		Ar << ViewportBounds.MaxDepth;
		return Ar;
	}
};

typedef TArray<YScreenResolutionRHI>	YScreenResolutionArray;

struct YVRamAllocation
{
	YVRamAllocation(uint32 InAllocationStart = 0, uint32 InAllocationSize = 0)
		: AllocationStart(InAllocationStart)
		, AllocationSize(InAllocationSize)
	{
	}

	bool IsValid() { return AllocationSize > 0; }

	// in bytes
	uint32 AllocationStart;
	// in bytes
	uint32 AllocationSize;
};

struct YRHIResourceInfo
{
	YVRamAllocation VRamAllocation;
};

enum class EYClearBinding
{
	YENoneBound, //no clear color associated with this target.  Target will not do hardware clears on most platforms
	YEColorBound, //target has a clear color bound.  Clears will use the bound color, and do hardware clears.
	YEDepthStencilBound, //target has a depthstencil value bound.  Clears will use the bound values and do hardware clears.
};

struct YClearValueBinding
{
	struct DSVAlue
	{
		float Depth;
		uint32 Stencil;
	};

	YClearValueBinding()
		: ColorBinding(EYClearBinding::YEColorBound)
	{
		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;
	}

	YClearValueBinding(EYClearBinding NoBinding)
		: ColorBinding(NoBinding)
	{
		check(ColorBinding == EYClearBinding::YENoneBound);
	}

	explicit YClearValueBinding(const FLinearColor& InClearColor)
		: ColorBinding(EYClearBinding::YEColorBound)
	{
		Value.Color[0] = InClearColor.R;
		Value.Color[1] = InClearColor.G;
		Value.Color[2] = InClearColor.B;
		Value.Color[3] = InClearColor.A;
	}

	explicit YClearValueBinding(float DepthClearValue, uint32 StencilClearValue = 0)
		: ColorBinding(EYClearBinding::YEDepthStencilBound)
	{
		Value.DSValue.Depth = DepthClearValue;
		Value.DSValue.Stencil = StencilClearValue;
	}

	FLinearColor GetClearColor() const
	{
		ensure(ColorBinding == EYClearBinding::YEColorBound);
		return FLinearColor(Value.Color[0], Value.Color[1], Value.Color[2], Value.Color[3]);
	}

	void GetDepthStencil(float& OutDepth, uint32& OutStencil) const
	{
		ensure(ColorBinding == EYClearBinding::YEDepthStencilBound);
		OutDepth = Value.DSValue.Depth;
		OutStencil = Value.DSValue.Stencil;
	}

	bool operator==(const YClearValueBinding& Other) const
	{
		if (ColorBinding == Other.ColorBinding)
		{
			if (ColorBinding == EYClearBinding::YEColorBound)
			{
				return
					Value.Color[0] == Other.Value.Color[0] &&
					Value.Color[1] == Other.Value.Color[1] &&
					Value.Color[2] == Other.Value.Color[2] &&
					Value.Color[3] == Other.Value.Color[3];

			}
			if (ColorBinding == EYClearBinding::YEDepthStencilBound)
			{
				return
					Value.DSValue.Depth == Other.Value.DSValue.Depth &&
					Value.DSValue.Stencil == Other.Value.DSValue.Stencil;
			}
			return true;
		}
		return false;
	}

	EYClearBinding ColorBinding;

	union ClearValueType
	{
		float Color[4];
		DSVAlue DSValue;
	} Value;

	// common clear values
	static  const YClearValueBinding None;
	static  const YClearValueBinding Black;
	static  const YClearValueBinding White;
	static  const YClearValueBinding Transparent;
	static  const YClearValueBinding DepthOne;
	static  const YClearValueBinding DepthZero;
	static  const YClearValueBinding DepthNear;
	static  const YClearValueBinding DepthFar;
	static  const YClearValueBinding Green;
	static  const YClearValueBinding MidGray;
};


struct YRHIResourceCreateInfo
{
	// for CreateVertexBuffer/CreateStructuredBuffer calls
	YRHIResourceCreateInfo(FResourceArrayInterface* InResourceArray) :ResourceArray(InResourceArray)
	{}

	// for CreateTexture calls
	YRHIResourceCreateInfo(FResourceBulkDataInterface* InBulkData)
		: BulkData(InBulkData)
		, ResourceArray(nullptr)
		, ClearValueBinding(FLinearColor::Transparent)
	{}
	// for CreateTexture calls
	FResourceBulkDataInterface* BulkData;
	// for CreateVertexBuffer/CreateStructuredBuffer calls
	FResourceArrayInterface* ResourceArray = nullptr;
	// for binding clear colors to rendertargets.
	YClearValueBinding ClearValueBinding;
};

enum class YResourceTransitionAccess
{
	EReadable, //transition from write-> read
	EWritable, //transition from read -> write	
	ERWBarrier, // Mostly for UAVs.  Transition to read/write state and always insert a resource barrier.
	ERWNoBarrier, //Mostly UAVs.  Indicates we want R/W access and do not require synchronization for the duration of the RW state.  The initial transition from writable->RWNoBarrier and readable->RWNoBarrier still requires a sync
	ERWSubResBarrier, //For special cases where read/write happens to different subresources of the same resource in the same call.  Inserts a barrier, but read validation will pass.  Temporary until we pass full subresource info to all transition calls.
	EMetaData,		  // For transitioning texture meta data, for example for making readable in shaders
	EMaxAccess,
};

