#pragma once
#include "Core.h"
class UTexture;
enum class TextureSamplerType
{
	SAMPLERTYPE_Normal,
	SAMPLERTYPE_Color,
};
class ENGINE_API TextureSampler
{
public:
	TextureSampler();
	TextureSamplerType SamplerType;
	UTexture* Texture;
	float ScalingU;
	float ScalingV;
	int32 UVIndex;
};
class ENGINE_API UMaterialInterface
{
public:
	UMaterialInterface();
	FString GetPathName() const;
	bool IsValid() const;
	FName GetFName() const;
	FString PathName;
	FName	MaterialName;
	TextureSampler DiffuseTexture;
	TextureSampler NormalTexture;
	TextureSampler BumpTexture;

};