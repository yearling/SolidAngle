//#include "YYUT.h"
#include "YYDX11.h"
DX11Demo theApp;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	int result=0;
	YYUTApplication* yy_app=GetApplication();
		yy_app->Initial();
		result=yy_app->Run();
		yy_app->Exit();
	return result;
}
