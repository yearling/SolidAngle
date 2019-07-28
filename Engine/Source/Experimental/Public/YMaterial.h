# pragma once
#include "Core.h"
class YTexture;

enum class YTextureSampleType
{
	YTEXUTRESAMPLE_NORMAL,
	YTEXTURESAMPLE_COLOR
};

class YTextureSampler
{
public:
	YTextureSampler();
	YTextureSampleType SamplerType;
	YTexture* Texture;
	float ScalingU;
	float ScalingV;
	int32 UVIndex;
};

class YMaterialInterface
{
public:
	YMaterialInterface();
	FString GetPathName() const;
	bool IsValid() const;
	FName GetFName() const;
	FString PathName;
	FName MaterialName;
	YTextureSampler DiffuseTexture;
	YTextureSampler NormalTexture;
};