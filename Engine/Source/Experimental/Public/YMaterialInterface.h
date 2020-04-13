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
	float ScalingU=1.0;
	float ScalingV=1.0;
	int32 UVIndex=0;
};

class YMaterialInterface
{
public:
	YMaterialInterface();
	virtual ~YMaterialInterface() {};
	FString GetPathName() const;
	bool IsValid() const;
	FName GetFName() const;
	FString PathName;
	FName MaterialName;
	YTextureSampler DiffuseTexture;
	YTextureSampler NormalTexture;
};

class YMeshMaterial :public YMaterialInterface
{
public:
	YMeshMaterial() {}
};