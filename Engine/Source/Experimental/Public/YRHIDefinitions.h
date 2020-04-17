#pragma once


enum EYShaderFrequency
{
	YSF_Vertex = 0,
	YSF_Hull = 1,
	YSF_Domain = 2,
	YSF_Pixel = 3,
	YSF_Geometry = 4,
	YSF_Compute = 5,

	YSF_NumFrequencies = 6,

	YSF_NumBits = 3,
};
static_assert(YSF_NumFrequencies <= (1 << YSF_NumBits), "SF_NumFrequencies will not fit on SF_NumBits");

enum EYRenderQueryType
{
	// e.g. WaitForFrameEventCompletion()
	YRQT_Undefined,
	// Result is the number of samples that are not culled (divide by MSAACount to get pixels)
	YRQT_Occlusion,
	// Result is time in micro seconds = 1/1000 ms = 1/1000000 sec
	YRQT_AbsoluteTime,
};

/** Maximum number of miplevels in a texture. */
enum { YMAX_TEXTURE_MIP_COUNT = 14 };

/** The maximum number of vertex elements which can be used by a vertex declaration. */
enum { YMaxVertexElementCount = 16 };

/** The alignment in bytes between elements of array shader parameters. */
enum { YShaderArrayElementAlignBytes = 16 };

/** The number of render-targets that may be simultaneously written to. */
enum { YMaxSimultaneousRenderTargets = 8 };

/** The number of UAVs that may be simultaneously bound to a shader. */
enum { YMaxSimultaneousUAVs = 8 };


enum class EYRHIZBuffer
{
	// Before changing this, make sure all math & shader assumptions are correct! Also wrap your C++ assumptions with
	//		static_assert(ERHIZBuffer::IsInvertedZBuffer(), ...);
	// Shader-wise, make sure to update Definitions.usf, HAS_INVERTED_Z_BUFFER
	YFarPlane = 0,
	YNearPlane = 1,

	// 'bool' for knowing if the API is using Inverted Z buffer
	YIsInverted = (int32)((int32)EYRHIZBuffer::YFarPlane < (int32)EYRHIZBuffer::YNearPlane),
};

/**
 * The RHI's feature level indicates what level of support can be relied upon.
 * Note: these are named after graphics API's like ES2 but a feature level can be used with a different API (eg ERHIFeatureLevel::ES2 on D3D11)
 * As long as the graphics API supports all the features of the feature level (eg no ERHIFeatureLevel::SM5 on OpenGL ES2)
 */
namespace EYRHIFeatureLevel
{
	enum Type
	{
		/** Feature level defined by the core capabilities of OpenGL ES2. */
		ES2,
		/** Feature level defined by the core capabilities of OpenGL ES3.1 & Metal/Vulkan. */
		ES3_1,
		/** Feature level defined by the capabilities of DX10 Shader Model 4. */
		SM4,
		/** Feature level defined by the capabilities of DX11 Shader Model 5. */
		SM5,
		Num
	};
};
enum EYSamplerFilter
{
	YSF_Point,
	YSF_Bilinear,
	YSF_Trilinear,
	YSF_AnisotropicPoint,
	YSF_AnisotropicLinear
};

enum EYSamplerAddressMode
{
	YAM_Wrap,
	YAM_Clamp,
	YAM_Mirror,
	/** Not supported on all platforms */
	YAM_Border
};

enum EYSamplerCompareFunction
{
	YSCF_Never,
	YSCF_Less
};

enum EYRasterizerFillMode
{
	YFM_Point,
	YFM_Wireframe,
	YFM_Solid
};

enum EYRasterizerCullMode
{
	YCM_None,
	YCM_CW,
	YCM_CCW
};

enum EYColorWriteMask
{
	YCW_RED = 0x01,
	YCW_GREEN = 0x02,
	YCW_BLUE = 0x04,
	YCW_ALPHA = 0x08,

	YCW_NONE = 0,
	YCW_RGB = YCW_RED | YCW_GREEN | YCW_BLUE,
	YCW_RGBA = YCW_RED | YCW_GREEN | YCW_BLUE | YCW_ALPHA,
	YCW_RG = YCW_RED | YCW_GREEN,
	YCW_BA = YCW_BLUE | YCW_ALPHA,
};

enum EYCompareFunction
{
	YCF_Less,
	YCF_LessEqual,
	YCF_Greater,
	YCF_GreaterEqual,
	YCF_Equal,
	YCF_NotEqual,
	YCF_Never,
	YCF_Always,

	// Utility enumerations
	YCF_DepthNearOrEqual = (((int32)EYRHIZBuffer::YIsInverted != 0) ? YCF_GreaterEqual : YCF_LessEqual),
	YCF_DepthNear = (((int32)EYRHIZBuffer::YIsInverted != 0) ? YCF_Greater : YCF_Less),
	YCF_DepthFartherOrEqual = (((int32)EYRHIZBuffer::YIsInverted != 0) ? YCF_LessEqual : YCF_GreaterEqual),
	YCF_DepthFarther = (((int32)EYRHIZBuffer::YIsInverted != 0) ? YCF_Less : YCF_Greater),
};

enum EYStencilOp
{
	YSO_Keep,
	YSO_Zero,
	YSO_Replace,
	YSO_SaturatedIncrement,
	YSO_SaturatedDecrement,
	YSO_Invert,
	YSO_Increment,
	YSO_Decrement
};

enum EYBlendOperation
{
	YBO_Add,
	YBO_Subtract,
	YBO_Min,
	YBO_Max,
	YBO_ReverseSubtract,
};

enum EYBlendFactor
{
	YBF_Zero,
	YBF_One,
	YBF_SourceColor,
	YBF_InverseSourceColor,
	YBF_SourceAlpha,
	YBF_InverseSourceAlpha,
	YBF_DestAlpha,
	YBF_InverseDestAlpha,
	YBF_DestColor,
	YBF_InverseDestColor,
	YBF_ConstantBlendFactor,
	YBF_InverseConstantBlendFactor
};

enum EYVertexElementType
{
	YVET_None,
	YVET_Float1,
	YVET_Float2,
	YVET_Float3,
	YVET_Float4,
	YVET_PackedNormal,	// FPackedNormal
	YVET_UByte4,
	YVET_UByte4N,
	YVET_Color,
	YVET_Short2,
	YVET_Short4,
	YVET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	YVET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	YVET_Half4,
	YVET_Short4N,		// 4 X 16 bit word, normalized 
	YVET_UShort2,
	YVET_UShort4,
	YVET_UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
	YVET_UShort4N,		// 4 X 16 bit word unsigned, normalized 
	YVET_URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
	YVET_MAX
};

enum EYCubeFace
{
	YCubeFace_PosX = 0,
	YCubeFace_NegX,
	YCubeFace_PosY,
	YCubeFace_NegY,
	YCubeFace_PosZ,
	YCubeFace_NegZ,
	YCubeFace_MAX
};


enum EYUniformBufferUsage
{
	// the uniform buffer is temporary, used for a single draw call then discarded
	YUniformBuffer_SingleDraw = 0,
	// the uniform buffer is used for multiple draw calls but only for the current frame
	YUniformBuffer_SingleFrame,
	// the uniform buffer is used for multiple draw calls, possibly across multiple frames
	YUniformBuffer_MultiFrame,
};

/** The base type of a value in a uniform buffer. */
enum EYUniformBufferBaseType
{
	YUBMT_INVALID,
	YUBMT_BOOL,
	YUBMT_INT32,
	YUBMT_UINT32,
	YUBMT_FLOAT32,
	YUBMT_STRUCT,
	YUBMT_SRV,
	YUBMT_UAV,
	YUBMT_SAMPLER,
	YUBMT_TEXTURE
};

enum EYResourceLockMode
{
	YRLM_ReadOnly,
	YRLM_WriteOnly,
	YRLM_Num
};

/** limited to 8 types in FReadSurfaceDataFlags */
enum EYRangeCompressionMode
{
	// 0 .. 1
	YRCM_UNorm,
	// -1 .. 1
	YRCM_SNorm,
	// 0 .. 1 unless there are smaller values than 0 or bigger values than 1, then the range is extended to the minimum or the maximum of the values
	YRCM_MinMaxNorm,
	// minimum .. maximum (each channel independent)
	YRCM_MinMax,
};

enum class EYPrimitiveTopologyType : uint8
{
	YTriangle,
	YPatch,
	YLine,
	YPoint,
	YQuad,
	YNum,
	YNumBits = 3
};

enum EYPrimitiveType
{
	YPT_TriangleList,
	YPT_TriangleStrip,
	YPT_LineList,
	YPT_QuadList,
	YPT_PointList,
	YPT_1_ControlPointPatchList,
	YPT_2_ControlPointPatchList,
	YPT_3_ControlPointPatchList,
	YPT_4_ControlPointPatchList,
	YPT_5_ControlPointPatchList,
	YPT_6_ControlPointPatchList,
	YPT_7_ControlPointPatchList,
	YPT_8_ControlPointPatchList,
	YPT_9_ControlPointPatchList,
	YPT_10_ControlPointPatchList,
	YPT_11_ControlPointPatchList,
	YPT_12_ControlPointPatchList,
	YPT_13_ControlPointPatchList,
	YPT_14_ControlPointPatchList,
	YPT_15_ControlPointPatchList,
	YPT_16_ControlPointPatchList,
	YPT_17_ControlPointPatchList,
	YPT_18_ControlPointPatchList,
	YPT_19_ControlPointPatchList,
	YPT_20_ControlPointPatchList,
	YPT_21_ControlPointPatchList,
	YPT_22_ControlPointPatchList,
	YPT_23_ControlPointPatchList,
	YPT_24_ControlPointPatchList,
	YPT_25_ControlPointPatchList,
	YPT_26_ControlPointPatchList,
	YPT_27_ControlPointPatchList,
	YPT_28_ControlPointPatchList,
	YPT_29_ControlPointPatchList,
	YPT_30_ControlPointPatchList,
	YPT_31_ControlPointPatchList,
	YPT_32_ControlPointPatchList,
	YPT_Num,
	YPT_NumBits = 6
};

static_assert(YPT_Num <= (1 << 8), "EPrimitiveType doesn't fit in a byte");
static_assert(YPT_Num <= (1 << YPT_NumBits), "PT_NumBits is too small");

enum EYBufferUsageFlags
{
	// Mutually exclusive write-frequency flags
	YBUF_Static = 0x0001, // The buffer will be written to once.
	YBUF_Dynamic = 0x0002, // The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed.
	YBUF_Volatile = 0x0004, // The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame.

	// Mutually exclusive bind flags.
	YBUF_UnorderedAccess = 0x0008, // Allows an unordered access view to be created for the buffer.

	/** Create a byte address buffer, which is basically a structured buffer with a uint32 type. */
	YBUF_ByteAddressBuffer = 0x0020,
	/** Create a structured buffer with an atomic UAV counter. */
	YBUF_UAVCounter = 0x0040,
	/** Create a buffer that can be bound as a stream output target. */
	YBUF_StreamOutput = 0x0080,
	/** Create a buffer which contains the arguments used by DispatchIndirect or DrawIndirect. */
	YBUF_DrawIndirect = 0x0100,
	/**
	 * Create a buffer that can be bound as a shader resource.
	 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
	 */
	YBUF_ShaderResource = 0x0200,

	/**
	 * Request that this buffer is directly CPU accessible
	 * (@todo josh: this is probably temporary and will go away in a few months)
	 */
	YBUF_KeepCPUAccessible = 0x0400,

	/**
	 * Provide information that this buffer will contain only one vertex, which should be delivered to every primitive drawn.
	 * This is necessary for OpenGL implementations, which need to handle this case very differently (and can't handle GL_HALF_FLOAT in such vertices at all).
	 */
	YBUF_ZeroStride = 0x0800,

	/** Buffer should go in fast vram (hint only) */
	YBUF_FastVRAM = 0x1000,

	// Helper bit-masks
	YBUF_AnyDynamic = (YBUF_Dynamic | YBUF_Volatile),
};

/** An enumeration of the different RHI reference types. */
enum EYRHIResourceType
{
	YRRT_None,

	YRRT_SamplerState,
	YRRT_RasterizerState,
	YRRT_DepthStencilState,
	YRRT_BlendState,
	YRRT_VertexDeclaration,
	YRRT_VertexShader,
	YRRT_HullShader,
	YRRT_DomainShader,
	YRRT_PixelShader,
	YRRT_GeometryShader,
	YRRT_ComputeShader,
	YRRT_BoundShaderState,
	YRRT_UniformBuffer,
	YRRT_IndexBuffer,
	YRRT_VertexBuffer,
	YRRT_StructuredBuffer,
	YRRT_Texture,
	YRRT_Texture2D,
	YRRT_Texture2DArray,
	YRRT_Texture3D,
	YRRT_TextureCube,
	YRRT_TextureReference,
	YRRT_RenderQuery,
	YRRT_Viewport,
	YRRT_UnorderedAccessView,
	YRRT_ShaderResourceView,

	YRRT_Num
};


/** Flags used for texture creation */
enum EYTextureCreateFlags
{
	YTexCreate_None = 0,

	// Texture can be used as a render target
	YTexCreate_RenderTargetable = 1 << 0,
	// Texture can be used as a resolve target, Copy a multisampled resource into a non-multisampled resource. eg. ResolveSubresource
	YTexCreate_ResolveTargetable = 1 << 1,
	// Texture can be used as a depth-stencil target.
	YTexCreate_DepthStencilTargetable = 1 << 2,
	// Texture can be used as a shader resource.
	YTexCreate_ShaderResource = 1 << 3,

	// Texture is encoded in sRGB gamma space
	YTexCreate_SRGB = 1 << 4,
	// Texture will be created without a packed miptail
	YTexCreate_NoMipTail = 1 << 5,
	// Texture will be created with an un-tiled format
	YTexCreate_NoTiling = 1 << 6,
	// Texture that may be updated every frame
	YTexCreate_Dynamic = 1 << 8,
	// Allow silent texture creation failure
	YTexCreate_AllowFailure = 1 << 9,
	// Disable automatic defragmentation if the initial texture memory allocation fails.
	YTexCreate_DisableAutoDefrag = 1 << 10,
	// Create the texture with automatic -1..1 biasing
	YTexCreate_BiasNormalMap = 1 << 11,
	// Create the texture with the flag that allows mip generation later, only applicable to D3D11
	YTexCreate_GenerateMipCapable = 1 << 12,
	// UnorderedAccessView (DX11 only)
	// Warning: Causes additional synchronization between draw calls when using a render target allocated with this flag, use sparingly
	// See: GCNPerformanceTweets.pdf Tip 37
	YTexCreate_UAV = 1 << 16,
	// Render target texture that will be displayed on screen (back buffer)
	YTexCreate_Presentable = 1 << 17,
	// Texture data is accessible by the CPU
	YTexCreate_CPUReadback = 1 << 18,
	// Texture was processed offline (via a texture conversion process for the current platform)
	YTexCreate_OfflineProcessed = 1 << 19,
	// Texture needs to go in fast VRAM if available (HINT only)
	YTexCreate_FastVRAM = 1 << 20,
	// by default the texture is not showing up in the list - this is to reduce clutter, using the FULL option this can be ignored
	YTexCreate_HideInVisualizeTexture = 1 << 21,
	// Texture should be created in virtual memory, with no physical memory allocation made
	// You must make further calls to RHIVirtualTextureSetFirstMipInMemory to allocate physical memory
	// and RHIVirtualTextureSetFirstMipVisible to map the first mip visible to the GPU
	YTexCreate_Virtual = 1 << 22,
	// Creates a RenderTargetView for each array slice of the texture
	// Warning: if this was specified when the resource was created, you can't use SV_RenderTargetArrayIndex to route to other slices!
	YTexCreate_TargetArraySlicesIndependently = 1 << 23,
	// Texture that may be shared with DX9 or other devices
	YTexCreate_Shared = 1 << 24,
	// RenderTarget will not use full-texture fast clear functionality.
	YTexCreate_NoFastClear = 1 << 25,
	// Texture is a depth stencil resolve target
	YTexCreate_DepthStencilResolveTarget = 1 << 26,
	// Render target will not FinalizeFastClear; Caches and meta data will be flushed, but clearing will be skipped (avoids potentially trashing metadata)
	YTexCreate_NoFastClearFinalize = 1 << 28,
	// Hint to the driver that this resource is managed properly by the engine for Alternate-Frame-Rendering in mGPU usage.
	YTexCreate_AFRManual = 1 << 29,
};

enum EYAsyncComputePriority
{
	YAsyncComputePriority_Default = 0,
	YAsyncComputePriority_High,
};
/**
 * Async texture reallocation status, returned by RHIGetReallocateTexture2DStatus().
 */
enum EYTextureReallocationStatus
{
	YTexRealloc_Succeeded = 0,
	YTexRealloc_Failed,
	YTexRealloc_InProgress,
};

/**
 * Action to take when a rendertarget is set.
 */
enum class EYRenderTargetLoadAction
{
	YENoAction,
	YELoad,
	YEClear,
};

/**
 * Action to take when a rendertarget is unset or at the end of a pass.
 */
enum class EYRenderTargetStoreAction
{
	YENoAction,
	YEStore,
	YEMultisampleResolve,
};

/**
 * Common render target use cases
 */
enum class EYSimpleRenderTargetMode
{
	// These will all store out color and depth
	YEExistingColorAndDepth,							// Color = Existing, Depth = Existing
	YEUninitializedColorAndDepth,					// Color = ????, Depth = ????
	YEUninitializedColorExistingDepth,				// Color = ????, Depth = Existing
	YEUninitializedColorClearDepth,					// Color = ????, Depth = Default
	YEClearColorExistingDepth,						// Clear Color = whatever was bound to the rendertarget at creation time. Depth = Existing
	YEClearColorAndDepth,							// Clear color and depth to bound clear values.
	YEExistingContents_NoDepthStore,					// Load existing contents, but don't store depth out.  depth can be written.
	YEExistingColorAndClearDepth,					// Color = Existing, Depth = clear value
	YEExistingColorAndDepthAndClearStencil,			// Color = Existing, Depth = Existing, Stencil = clear

	// If you add an item here, make sure to add it to DecodeRenderTargetMode() as well!
};

enum class EYClearDepthStencil
{
	YDepth,
	YStencil,
	YDepthStencil,
};

/**
 * Hint to the driver on how to load balance async compute work.  On some platforms this may be a priority, on others actually masking out parts of the GPU for types of work.
 */
enum class EYAsyncComputeBudget
{
	YELeast_0,			//Least amount of GPU allocated to AsyncCompute that still gets 'some' done.
	YEGfxHeavy_1,		//Gfx gets most of the GPU.
	YEBalanced_2,		//Async compute and Gfx share GPU equally.
	YEComputeHeavy_3,	//Async compute can use most of the GPU
	YEAll_4,				//Async compute can use the entire GPU.
};
