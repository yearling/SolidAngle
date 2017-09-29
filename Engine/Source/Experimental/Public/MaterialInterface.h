#pragma once
#include "Core.h"
class UMaterialInterface
{
public:
	UMaterialInterface();
	FString GetPathName() const;
private:
	FString PathName;
};