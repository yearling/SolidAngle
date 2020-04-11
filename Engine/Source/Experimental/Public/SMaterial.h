#pragma once
#include "Core.h"
#include "SObject.h"
class SMaterial :public SObject
{
public:
	SMaterial();
	static bool constexpr IsInstance() { return true; }
	friend class SObjectManager;
	FArchive& Serialize(FArchive& Ar);
	FName MaterialName;
protected:
	TMap<FName, FVariant> MaterialParameters;
};

 
