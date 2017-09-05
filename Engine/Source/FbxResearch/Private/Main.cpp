#include "Core.h"
#include "Modules\ModuleManager.h"
#include <fbxsdk.h>
#include <iostream>
#include "FbxCommon.h"
class ResearchFbx : public FDefaultModuleImpl
{
public:
	virtual bool SupportsDynamicReloading() override
	{
		// Core cannot be unloaded or reloaded
		return false;
	}
};
IMPLEMENT_MODULE(ResearchFbx, TestModel)

FbxManager*   gSdkManager = nullptr;
FbxScene*     gScene = nullptr;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif
DECLARE_LOG_CATEGORY_EXTERN(Main, Log, All);
DEFINE_LOG_CATEGORY(Main);


FbxFileTexture*  gTexture = NULL;
FbxSurfacePhong* gMaterial = NULL;
void CreateTexture(FbxScene* pScene)
{
	gTexture = FbxFileTexture::Create(pScene, "Diffuse Texture");

	// Resource file must be in the application's directory.
	FbxString lTexPath = "\\Crate.jpg";

	// Set texture properties.
	gTexture->SetFileName(lTexPath.Buffer());
	gTexture->SetTextureUse(FbxTexture::eStandard);
	gTexture->SetMappingType(FbxTexture::eUV);
	gTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	gTexture->SetSwapUV(false);
	gTexture->SetTranslation(0.0, 0.0);
	gTexture->SetScale(1.0, 1.0);
	gTexture->SetRotation(0.0, 0.0);
}
int main(int argc,TCHAR* argv[])
{
	FCommandLine::Set(TEXT(""));
	GLogConsole = FPlatformOutputDevices::GetLogConsole();
	GLogConsole->Show(true);
	FPlatformOutputDevices::SetupOutputDevices();
	//GLog->AddOutputDevice(GLogConsole);
	FbxManager* pManger = nullptr;
	FbxScene* pScene = nullptr;
	if (!InitializeSdkObjects(pManger, pScene))
	{
		UE_LOG(Main, Error, TEXT("Create FbxScene Failed!"));
		return -1;
	}
	check(pManger);
	check(pScene);
	GLog->TearDown();
}