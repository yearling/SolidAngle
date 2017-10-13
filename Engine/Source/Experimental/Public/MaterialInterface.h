#pragma once
#include "Core.h"
class UTexture;
enum class TextureSamplerType
{
	SAMPLERTYPE_Normal,
	SAMPLERTYPE_Color,
};
class TextureSampler
{
public:
	TextureSampler();
	TextureSamplerType SamplerType;
	UTexture* Texture;
	float ScalingU;
	float ScalingV;
	int32 UVIndex;
};
class UMaterialInterface
{
public:
	UMaterialInterface();
	FString GetPathName() const;
	FString PathName;
	TextureSampler DiffuseTexture;
	TextureSampler NormalTexture;
	TextureSampler BumpTexture;

};