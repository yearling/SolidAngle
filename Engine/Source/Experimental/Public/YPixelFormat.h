#pragma once

#include "CoreMinimal.h"

#if defined(YPF_MAX)
#undef YPF_MAX
#endif

enum EYPixelFormat
{
	YPF_Unknown              =0,
	YPF_A32B32G32R32F        =1,
	YPF_B8G8R8A8             =2,
	YPF_G8                   =3,
	YPF_G16                  =4,
	YPF_DXT1                 =5,
	YPF_DXT3                 =6,
	YPF_DXT5                 =7,
	YPF_UYVY                 =8,
	YPF_FloatRGB             =9,
	YPF_FloatRGBA            =10,
	YPF_DepthStencil         =11,
	YPF_ShadowDepth          =12,
	YPF_R32_FLOAT            =13,
	YPF_G16R16               =14,
	YPF_G16R16F              =15,
	YPF_G16R16F_FILTER       =16,
	YPF_G32R32F              =17,
	YPF_A2B10G10R10          =18,
	YPF_A16B16G16R16         =19,
	YPF_D24                  =20,
	YPF_R16F                 =21,
	YPF_R16F_FILTER          =22,
	YPF_BC5                  =23,
	YPF_V8U8                 =24,
	YPF_A1                   =25,
	YPF_FloatR11G11B10       =26,
	YPF_A8                   =27,
	YPF_R32_UINT             =28,
	YPF_R32_SINT             =29,
	YPF_PVRTC2               =30,
	YPF_PVRTC4               =31,
	YPF_R16_UINT             =32,
	YPF_R16_SINT             =33,
	YPF_R16G16B16A16_UINT    =34,
	YPF_R16G16B16A16_SINT    =35,
	YPF_R5G6B5_UNORM         =36,
	YPF_R8G8B8A8             =37,
	YPF_A8R8G8B8				=38,	// Only used for legacy loading; do NOT use!
	YPF_BC4					=39,
	YPF_R8G8                 =40,
	YPF_ATC_RGB				=41,
	YPF_ATC_RGBA_E			=42,
	YPF_ATC_RGBA_I			=43,
	YPF_X24_G8				=44,	// Used for creating SRVs to alias a DepthStencil buffer to read Stencil. Don't use for creating textures.
	YPF_ETC1					=45,
	YPF_ETC2_RGB				=46,
	YPF_ETC2_RGBA			=47,
	YPF_R32G32B32A32_UINT	=48,
	YPF_R16G16_UINT			=49,
	YPF_ASTC_4x4             =50,	// 8.00 bpp
	YPF_ASTC_6x6             =51,	// 3.56 bpp
	YPF_ASTC_8x8             =52,	// 2.00 bpp
	YPF_ASTC_10x10           =53,	// 1.28 bpp
	YPF_ASTC_12x12           =54,	// 0.89 bpp
	YPF_BC6H					=55,
	YPF_BC7					=56,
	YPF_R8_UINT				=57,
	YPF_MAX                  =58,
};
#define YFOREACH_ENUM_EPIXELFORMAT(op) \
	op(YPF_Unknown) \
	op(YPF_A32B32G32R32F) \
	op(YPF_B8G8R8A8) \
	op(YPF_G8) \
	op(YPF_G16) \
	op(YPF_DXT1) \
	op(YPF_DXT3) \
	op(YPF_DXT5) \
	op(YPF_UYVY) \
	op(YPF_FloatRGB) \
	op(YPF_FloatRGBA) \
	op(YPF_DepthStencil) \
	op(YPF_ShadowDepth) \
	op(YPF_R32_FLOAT) \
	op(YPF_G16R16) \
	op(YPF_G16R16F) \
	op(YPF_G16R16F_FILTER) \
	op(YPF_G32R32F) \
	op(YPF_A2B10G10R10) \
	op(YPF_A16B16G16R16) \
	op(YPF_D24) \
	op(YPF_R16F) \
	op(YPF_R16F_FILTER) \
	op(YPF_BC5) \
	op(YPF_V8U8) \
	op(YPF_A1) \
	op(YPF_FloatR11G11B10) \
	op(YPF_A8) \
	op(YPF_R32_UINT) \
	op(YPF_R32_SINT) \
	op(YPF_PVRTC2) \
	op(YPF_PVRTC4) \
	op(YPF_R16_UINT) \
	op(YPF_R16_SINT) \
	op(YPF_R16G16B16A16_UINT) \
	op(YPF_R16G16B16A16_SINT) \
	op(YPF_R5G6B5_UNORM) \
	op(YPF_R8G8B8A8) \
	op(YPF_A8R8G8B8) \
	op(YPF_BC4) \
	op(YPF_R8G8) \
	op(YPF_ATC_RGB) \
	op(YPF_ATC_RGBA_E) \
	op(YPF_X24_G8) \
	op(YPF_ETC1) \
	op(YPF_ETC2_RGB) \
	op(YPF_ETC2_RGBA) \
	op(YPF_R32G32B32A32_UINT) \
	op(YPF_R16G16_UINT) \
	op(YPF_ASTC_4x4) \
	op(YPF_ASTC_6x6) \
	op(YPF_ASTC_8x8) \
	op(YPF_ASTC_10x10) \
	op(YPF_ASTC_12x12) \
	op(YPF_BC6H) \
	op(YPF_BC7) \
	op(YPF_R8_UINT)

