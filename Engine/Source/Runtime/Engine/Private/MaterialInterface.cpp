#pragma once
#include "MaterialInterface.h"

UMaterialInterface::UMaterialInterface()
{

}

FString UMaterialInterface::GetPathName() const
{
	return PathName;
}

bool UMaterialInterface::IsValid() const
{
	return true;
}

FName UMaterialInterface::GetFName() const
{
	return MaterialName;
}

TextureSampler::TextureSampler()
	:SamplerType(TextureSamplerType::SAMPLERTYPE_Color)
	,Texture(nullptr)
	,ScalingU(1.0f)
	,ScalingV(1.0f)
	,UVIndex(0)
{

}