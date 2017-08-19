#include "YYUTMutiScreen.h"
#include <tchar.h>
#include <cstdio>
#include <cstdlib>
#include <clocale>
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <iostream>
using namespace std;
HMONITOR CMutiScreen::m_hPrimior = NULL;
HMONITOR CMutiScreen::m_hOther = NULL;
int  CMutiScreen::m_nNumber = 0;
CMutiScreen::CMutiScreen()
{
	::EnumDisplayMonitors(NULL, NULL, EnumProc, 0);
	InitScreenInfo(m_hPrimior, m_vPrimerMonitor, m_strPrimiorName);
	if (m_hOther)
		InitScreenInfo(m_hOther, m_vOtherMointor, m_strOtherName);
	m_hCurrentMainHWND = NULL;
	m_hVSMainHWND = NULL;
}
BOOL CALLBACK CMutiScreen::EnumProc(
	HMONITOR hMonitor,
	HDC hdcMonitor,
	LPRECT lprcMonitor,
	LPARAM dwData
)
{
	MONITORINFOEX mi;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	if (mi.dwFlags&MONITORINFOF_PRIMARY)
	{
		CMutiScreen::m_hPrimior = hMonitor;
		CMutiScreen::m_nNumber++;

	}
	else
	{
		CMutiScreen::m_hOther = hMonitor;
		CMutiScreen::m_nNumber++;
	}
	return TRUE;
}

CMutiScreen::OtherPosition CMutiScreen::GetOtherPosition()
{
	MONITORINFOEX Pri;
	MONITORINFOEX Other;
	ZeroMemory(&Pri, sizeof(Pri));
	Pri.cbSize = sizeof(Pri);
	GetMonitorInfo(CMutiScreen::m_hPrimior, &Pri);
	ZeroMemory(&Other, sizeof(Other));
	Other.cbSize = sizeof(Other);
	GetMonitorInfo(CMutiScreen::m_hOther, &Other);
	if (Other.rcMonitor.right <= Pri.rcMonitor.left)
		return SCREEN_LEFT;
	if (Other.rcMonitor.bottom <= Pri.rcMonitor.top)
		return SCREEN_TOP;
	if (Other.rcMonitor.left >= Pri.rcMonitor.right)
		return SCREEN_RIGHT;
	if (Other.rcMonitor.top >= Pri.rcMonitor.bottom)
		return SCREEN_DOWN;
	return SCREEN_LEFT;
}

RECT CMutiScreen::GetRect(HMONITOR hMonitor)
{
	MONITORINFOEX Pri;
	ZeroMemory(&Pri, sizeof(Pri));
	Pri.cbSize = sizeof(Pri);
	GetMonitorInfo(hMonitor, &Pri);
	return Pri.rcMonitor;
}
static inline bool BetterScreen(ScreenInfo a, ScreenInfo b)
{
	return a.lPelsHeight*a.lPelsWidth > b.lPelsHeight*b.lPelsWidth;
}
#ifdef UNICODE
void CMutiScreen::InitScreenInfo(HMONITOR hMonitor, std::vector<ScreenInfo> &vec, std::wstring &strName)
{
	MONITORINFOEX Pri;
	ZeroMemory(&Pri, sizeof(Pri));
	Pri.cbSize = sizeof(Pri);
	GetMonitorInfo(hMonitor, &Pri);
	strName = Pri.szDevice;
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	dm.dmDriverExtra = 0;
	for (int i = 0; ::EnumDisplaySettings(strName.c_str(), i, &dm); i++)
	{
		vec.push_back(ScreenInfo(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel));
	}
	std::sort(vec.begin(), vec.end(), BetterScreen);
}
#else
void CMutiScreen::InitScreenInfo(HMONITOR hMonitor, std::vector<ScreenInfo> &vec, std::string &strName)
{
	MONITORINFOEX Pri;
	ZeroMemory(&Pri, sizeof(Pri));
	Pri.cbSize = sizeof(Pri);
	GetMonitorInfo(hMonitor, &Pri);
	strName = Pri.szDevice;
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);
	dm.dmDriverExtra = 0;
	for (int i = 0; ::EnumDisplaySettings(strName.c_str(), i, &dm); i++)
	{
		vec.push_back(ScreenInfo(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel));
	}
	std::sort(vec.begin(), vec.end(), BetterScreen);
}
#endif
ScreenInfo CMutiScreen::GetDefaultScreenInfo(bool PrimiorMonitor)
{
	if (PrimiorMonitor)
		return m_vPrimerMonitor[0];
	else
		return m_vOtherMointor[0];
}
#ifdef UNICODE
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	wstring *pstr = (wstring*)lParam;
	CMutiScreen *pMs = GetMutiScreen();
	TCHAR tmp[255];
	::GetWindowText(hwnd, tmp, 255);
	wstring testName(tmp);
	if (pstr)
	{
		if (string::npos != testName.find(*pstr))
		{
			//找到了。
			if (testName == *pstr)
				return TRUE;
			HwndANDPosition hp;
			hp.hwnd = hwnd;
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			hp.rc = rc;
			pMs->m_hpVSWindows.push_back(hp);
		}
	}
	return TRUE;
}
#else
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	string *pstr = (string*)lParam;
	CMutiScreen *pMs = GetMutiScreen();
	TCHAR tmp[255];
	::GetWindowText(hwnd, tmp, 255);
	string testName(tmp);
	if (pstr)
	{
		if (string::npos != testName.find(*pstr))
		{
			//找到了。
			if (testName == *pstr)
				return TRUE;
			HwndANDPosition hp;
			hp.hwnd = hwnd;
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			hp.rc = rc;
			pMs->m_hpVSWindows.push_back(hp);
		}
	}
	return TRUE;
}
#endif
bool CMutiScreen::Init()
{
	if (m_nNumber != 1)
	{

		m_hCurrentMainHWND = ::GetActiveWindow();
#ifdef UNICODE
		wstring vsName(_T("Microsoft Visual Studio"));
#else
		string vsName(("Microsoft Visual Studio"));
#endif
		::EnumWindows(EnumWindowsProc, (LPARAM)&vsName);
		RECT rc = GetRect(m_hOther);
		for (auto it = m_hpVSWindows.begin(); it != m_hpVSWindows.end(); it++)
		{
			::SetWindowPos(it->hwnd, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
		}
	}
	return true;
}

void CMutiScreen::Restore()
{
	if (m_nNumber != 1)
	{

		for (auto it = m_hpVSWindows.begin(); it != m_hpVSWindows.end(); it++)
		{
			RECT rc = it->rc;
			::SetWindowPos(it->hwnd, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW | SWP_NOZORDER);
			::SendMessageW(it->hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
	}
}

CMutiScreen::~CMutiScreen()
{
	Restore();
}

//////////////////////////////////////////////////////////////////////////
CMutiScreen g_MutiScreen;
CMutiScreen* GetMutiScreen()
{
	return &g_MutiScreen;
}
//////////////////////////////////////////////////////////////////////////


void YYSetConsoleA(HWND after)
{
	AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "w+t", stdin);
	freopen_s(&stream, "CONOUT$", "w+t", stderr);
	setlocale(LC_ALL, "chs");
	std::ios::sync_with_stdio();
	cout.clear();//防止在AllocConsole之前调用cout，这样子的话，cout.badbit set会被设置，然后就什么也不能输出了。
	//见http://stackoverflow.com/questions/18914506/doing-a-stdendl-before-allocconsole-causes-no-display-of-stdcout
	wcout.clear();
	CMutiScreen *pMS = GetMutiScreen();
	//pMS->Init();
	if (pMS == NULL)
		return;

	RECT rc = pMS->GetRect(pMS->m_hOther);
	LPCSTR title = "DebugConsole";
	SetConsoleTitleA(title);
	HWND hWnd = FindWindowA(NULL, title);
	hWnd = GetConsoleWindow();
	if (hWnd != NULL)
	{
		if (pMS->m_nNumber >= 2)
		{
			::SetWindowPos(hWnd, HWND_TOP, rc.left + 200, rc.top + 200, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}
		else
		{
			if (after)
				::SetWindowPos(hWnd, after, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
			else
				::SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		}
	}
	//找到visual studio的窗口并把它设到副窗口

}
void YYSetConsoleW(HWND after)
{
	AllocConsole();
	FILE *stream;
	_tfreopen_s(&stream, _T("CONOUT$"), _T("w+t"), stdout);
	_tfreopen_s(&stream, _T("CONIN$"), _T("w+t"), stdin);
	_tfreopen_s(&stream, _T("CONOUT$"), _T("w+t"), stderr);
	_tsetlocale(LC_ALL, _T("chs"));
	std::ios::sync_with_stdio();
	cout.clear();
	wcout.clear();
	CMutiScreen *pMS = GetMutiScreen();
	pMS->Init();
	if (pMS == NULL)
		return;

	RECT rc = pMS->GetRect(pMS->m_hOther);
	LPCTSTR title = _T("DebugConsole");
	SetConsoleTitle(title);
	HWND hWnd = FindWindowW(NULL, title);
	hWnd = GetConsoleWindow();
	if (hWnd != NULL)
	{
		if (pMS->m_nNumber >= 2)
		{
			::SetWindowPos(hWnd, HWND_NOTOPMOST, rc.left + 200, rc.top + 200, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		else
		{
			if (after)
				::SetWindowPos(hWnd, after, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			else
				::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
	}
	//找到visual studio的窗口并把它设到副窗口

}

void YYSetConsoleA(string file_name)
{
	AllocConsole();
	FILE *stream;
	FILE *pStdout = nullptr;
	freopen_s(&pStdout, file_name.c_str(), "w+t", stdout);
	freopen_s(&stream, "CONIN$", "w+t", stdin);
	freopen_s(&stream, "CONOUT$", "w+t", stderr);
	setlocale(LC_ALL, "chs");
	std::ios::sync_with_stdio();
	cout.clear();
	wcout.clear();

	CMutiScreen *pMS = GetMutiScreen();
	pMS->Init();
	if (pMS == NULL)
		return;
	if (pMS->m_nNumber >= 2)
	{
		RECT rc = pMS->GetRect(pMS->m_hOther);
		LPCTSTR title = _T("DebugConsole");
		SetConsoleTitle(title);
		HWND hWnd = FindWindowW(NULL, title);
		hWnd = GetConsoleWindow();
		if (hWnd != NULL)
		{
			::SetWindowPos(hWnd, HWND_TOP, rc.left + 200, rc.top + 200, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		//找到visual studio的窗口并把它设到副窗口
	}
}

void YYSetConsoleW(wstring file_name)
{
	AllocConsole();
	FILE *stream;
	FILE *pStdout = nullptr;
	_wfreopen_s(&pStdout, file_name.c_str(), L"w+t", stdout);
	_wfreopen_s(&stream, L"CONIN$", L"w+t", stdin);
	_wfreopen_s(&stream, L"CONOUT$", L"w+t", stderr);
	_wsetlocale(LC_ALL, L"chs");
	std::ios::sync_with_stdio();
	cout.clear();
	wcout.clear();

	CMutiScreen *pMS = GetMutiScreen();
	pMS->Init();
	if (pMS == NULL)
		return;
	if (pMS->m_nNumber >= 2)
	{
		RECT rc = pMS->GetRect(pMS->m_hOther);
		LPCTSTR title = _T("DebugConsole");
		SetConsoleTitle(title);
		HWND hWnd = FindWindowW(NULL, title);
		hWnd = GetConsoleWindow();
		if (hWnd != NULL)
		{
			::SetWindowPos(hWnd, HWND_TOP, rc.left + 200, rc.top + 200, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		//找到visual studio的窗口并把它设到副窗口
	}
}

