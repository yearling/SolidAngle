#pragma once
#include "MaterialInterface.h"

UMaterialInterface::UMaterialInterface()
{

}

FString UMaterialInterface::GetPathName() const
{
	return PathName;
}
