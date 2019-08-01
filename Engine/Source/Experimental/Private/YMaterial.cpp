#include "YMaterial.h"

YTextureSampler::YTextureSampler()
{

}

YMaterialInterface::YMaterialInterface()
{

}

FString YMaterialInterface::GetPathName() const
{
	return PathName;
}

bool YMaterialInterface::IsValid() const
{
	return true;
}

FName YMaterialInterface::GetFName() const
{
	return MaterialName;
}
