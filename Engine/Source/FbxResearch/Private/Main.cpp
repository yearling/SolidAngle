#include "Core.h"
#include "Modules\ModuleManager.h"
#include <fbxsdk.h>
#include <iostream>
#include "FbxCommon.h"
#include "fbxsdk\scene\geometry\fbxlayer.h"
#include "fbxsdk\scene\animation\fbxanimlayer.h"
#include "fbxsdk\scene\animation\fbxanimcurve.h"
#include "TestCategory.h"

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

FbxAnimLayer* gAnimLayer = NULL;  // holder of animation curves

DECLARE_LOG_CATEGORY_EXTERN(Main, Log, All);
DEFINE_LOG_CATEGORY(Main);

int main(int argc,TCHAR* argv[])
{
	FCommandLine::Set(TEXT(""));
	GLogConsole = FPlatformOutputDevices::GetLogConsole();
	GLogConsole->Show(true);
	FPlatformOutputDevices::SetupOutputDevices();
	TestCreateCube();
	TestRotateCube();
	TestPivot();
	TestPivotConvert();
	GLog->TearDown();
}