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
	FCommandLine::Set(lpCmdLine);
	GLogConsole = FPlatformOutputDevices::GetLogConsole();
	GLogConsole->Show(true);
	FPlatformOutputDevices::SetupOutputDevices();
	FPlatformMemory::Init();
	FMallocLeakDetection::Get().SetAllocationCollection(true);
	GLogConsole->Logf__VA(TEXT("log's name %s"), *FPlatformOutputDevices::GetAbsoluteLogFilename());

	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "w+t", stdin);
	freopen_s(&stream, "CONOUT$", "w+t", stderr);
	setlocale(LC_ALL, "chs");
	std::ios::sync_with_stdio();
	std::cout.clear();//防止在AllocConsole之前调用cout，这样子的话，cout.badbit set会被设置，然后就什么也不能输出了。
	//见http://stackoverflow.com/questions/18914506/doing-a-stdendl-before-allocconsole-causes-no-display-of-stdcout
	std::wcout.clear();
	int result=0;
	YYUTApplication* yy_app=GetApplication();
	yy_app->Initial();
	//std::wcout << "hello" << std::endl;
	result=yy_app->Run();
	yy_app->Exit();
	FMallocLeakDetection::Get().SetAllocationCollection(false);
	FMallocLeakDetection::Get().DumpPotentialLeakers();
	FMallocLeakDetection::Get().DumpOpenCallstacks();
	if (GLog)
	{
		GLog->TearDown();
	}
	return result;
}
