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
int main(int argc,TCHAR* argv[])
{
	FCommandLine::Set(TEXT(""));
	GLogConsole = FPlatformOutputDevices::GetLogConsole();
	GLogConsole->Show(true);
	FPlatformOutputDevices::SetupOutputDevices();
	//GLog->AddOutputDevice(GLogConsole);
	FbxManager* pManger = nullptr;
	FbxScene* pScene = nullptr;
	if (InitializeSdkObjects(pManger, pScene))
	{

	}
	check(pManger);
	check(pScene);
	GLog->TearDown();
}