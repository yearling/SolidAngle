#include "YYUT.h"
#include "YYDX11.h"
#include "Modules\ModuleManager.h"
#include <iostream>
#include "HAL\MallocLeakDetection.h"

DX11Demo theApp;
class YYGameModel : public FDefaultModuleImpl
{
public:
	virtual bool SupportsDynamicReloading() override
	{
		// Core cannot be unloaded or reloaded
		return false;
	}
};
IMPLEMENT_MODULE(YYGameModel, YYGameModel);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	FPlatformMemory::Init();
	const FPlatformMemoryStats& StateReport = FPlatformMemory::GetStats();
	std::cout << "MemoryTotalPhysical: " << StateReport.TotalPhysicalGB << " GB" << std::endl;
	std::cout << "MemoryTotalVirtual: " << (StateReport.TotalVirtual / 1024 / 1024 / 1024) << " GB" << std::endl;
	std::cout << "MemoryPage: " << StateReport.PageSize / 1024 << " KB" << std::endl;

	FMallocLeakDetection::Get().SetAllocationCollection(true);
	int result=0;
	YYUTApplication* yy_app=GetApplication();
	yy_app->Initial();
	result=yy_app->Run();
	yy_app->Exit();
	FMallocLeakDetection::Get().SetAllocationCollection(false);
	FMallocLeakDetection::Get().DumpPotentialLeakers();
	FMallocLeakDetection::Get().DumpOpenCallstacks();
	return result;
}
