#include "YYUTApplication.h"
YYUTApplication *YYUT_application;

YYUTApplication::YYUTApplication(void)
	:m_spMainWindow(new YYUTWindow())
{
	YYUT_application = this;
}


YYUTApplication::~YYUTApplication(void)
{
}

void YYUTApplication::WindowCreate(int width, int height, const TCHAR *title, int cmdshow)
{
	//assert(m_spMainWindow);
	m_spMainWindow->Init(width, height, title);
	ShowWindow(*m_spMainWindow, cmdshow);
	UpdateWindow(*m_spMainWindow);
}

void YYUTApplication::Initial()
{
	m_spMainWindow->SetInstance(m_hMainInstance);
}

int YYUTApplication::Run()
{
	MSG msg;
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				return static_cast<int>(msg.wParam);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}

void YYUTApplication::Exit()
{
}


LRESULT CALLBACK YYUTApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	YYUTApplication *pApp = GetApplication();
	return pApp->MyProc(hWnd, message, wParam, lParam);
}

LRESULT YYUTApplication::MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void YYUTApplication::Update(float ElapseTime)
{

}

void YYUTApplication::Render()
{

}

YYUTApplication * GetApplication()
{
	return YYUT_application;
}
//throw an exception 
void YYUTWindow::Init(int width, int Height, const TCHAR *window_name)
{
	DWORD $err_hr;
	m_iWidth = width;
	m_iHeight = Height;
	WNDCLASSEX wcex;
	TCHAR _Regstr[] = { _T("YYWINDOW") };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = YYUTApplication::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hMainInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = _Regstr;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	$err_hr = RegisterClassEx(&wcex);
	m_hWnd = CreateWindow(_Regstr, window_name, WS_CAPTION | WS_SIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, m_iWidth, m_iHeight, NULL, NULL, m_hMainInstance, NULL);
	$err_hr = GetLastError();
	if (m_hWnd)
	{
		if (m_bIsWindowed)
		{

			RECT window_rect = { 0,0,m_iWidth,m_iHeight };
			// make the call to adjust window_rect
			//::AdjustWindowRectEx(&window_rect, GetWindowStyle(m_hWnd), GetMenu(m_hWnd) != NULL, GetWindowExStyle(m_hWnd));
			MoveWindow(m_hWnd, 100, // x position
				50, // y position
				window_rect.right - window_rect.left, // width
				window_rect.bottom - window_rect.top, // height
				FALSE);
		}
	}
}

