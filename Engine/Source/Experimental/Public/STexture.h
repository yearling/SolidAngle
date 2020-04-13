#pragma once
#include "Core.h"
#include "SObject.h"

enum  ETextureFilterType
{
	TFT_NEAREST = 0,
	TFT_BILINEAR = 1,
	TFT_TRILINEAR = 2,
	TFT_NUM
};

enum ETextureSampleType
{
	TST_WRAP = 0,
	TST_CLAMP,
	TST_MIRROR,
	TST_NUM
};

enum ETextureType
{
	ETT_2D = 0,
	ETT_3D = 1,
	ETT_CUBE = 2,
	ETT_Num
};
class STexture :public SObject
{
public:
	STexture();
	virtual ~STexture();
	static constexpr  bool IsInstance()
	{
		return true;
	};
	ETextureFilterType TextureFilterType = ETextureFilterType::TFT_TRILINEAR;
	ETextureSampleType TextureUSampleType = ETextureSampleType::TST_WRAP;
	ETextureSampleType TextureVSampleType = ETextureSampleType::TST_WRAP;
	ETextureType  TextureType = ETextureType::ETT_2D;
	int32 UVIndex;
	FName TexturePackagePath;
};

class STexture2D : public STexture
{
public:
	STexture2D();
	static constexpr  bool IsInstance()
	{
		return true;
	};
};
