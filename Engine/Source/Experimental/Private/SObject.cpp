#include "SObject.h"

SObject::~SObject()
{

}

SObject::SObject()
{

}

bool SObject::LoadFromPackage(const FString & Path)
{
	return true;
}

void SObject::SaveToPackage(const FString & Path)
{

}

FArchive& SObject::Serialize(FArchive& Ar)
{
	return Ar;
}

bool SObject::LoadFromJson(const TSharedPtr<FJsonObject>&RootJson)
{
	return true;
}

bool SObject::PostLoadOp()
{
	return true;
}
