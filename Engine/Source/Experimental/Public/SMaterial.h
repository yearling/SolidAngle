#pragma once
#include "Core.h"
#include "SObject.h"
#include "STexture.h"
#include "YMaterialInterface.h"


DECLARE_LOG_CATEGORY_EXTERN(LogSMaterial, Log, All);

class SMaterial :public SObject, public YMaterialInterface
{
public:
	SMaterial();
	virtual ~SMaterial();
	static bool constexpr IsInstance() { return false; }
	friend class SObjectManager;
	FArchive& Serialize(FArchive& Ar);
	FName MaterialName;

	virtual bool LoadFromPackage(const FString & Path) override;


	virtual void SaveToPackage(const FString & Path) override;
	void AddMaterialTextureParameter(FName ParameterName, TRefCountPtr<STexture> Texture);
protected:
	TMap<FName, FVariant> MaterialParameters;
	TMap<FName, TRefCountPtr<STexture>> TextureParameters;
};


class SMaterialInstance : public SMaterial
{
public:
	SMaterialInstance();
	virtual ~SMaterialInstance();
	static bool constexpr IsInstance() { return false; }
};
