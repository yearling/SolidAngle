#pragma once
#include "YYUT.h"
#include <vector>
#include <string>
#ifdef UNICODE
#define YYSetConsole YYSetConsoleW
#else
#define YYSetConsole YYSetConSoleA
#endif

class ScreenInfo
{
public:
	long							lPelsWidth;
	long							lPelsHeight;
	long							lBitPerPel;

	ScreenInfo() :
		lPelsWidth(0),
		lPelsHeight(lPelsWidth),
		lBitPerPel(0) {}

	ScreenInfo(long lWidth, long lHeight, long BitPerPel) :
		lPelsWidth(lWidth),
		lPelsHeight(lHeight),
		lBitPerPel(BitPerPel) {}
};


struct HwndANDPosition
{
	HWND							hwnd;
	RECT							rc;
	HwndANDPosition() :hwnd(NULL)
	{
		rc.bottom = 0;
		rc.left = 0;
		rc.right = 0;
		rc.top = 0;
	}
};


class CMutiScreen
{
public:
	CMutiScreen();
	static int						m_nNumber;//屏幕数量
	static BOOL __stdcall			EnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);//回调函数
	static HMONITOR					m_hPrimior;//主屏幕句柄
	static HMONITOR					m_hOther;//副屏幕句柄
#ifdef UNICODE
	std::wstring					m_strPrimiorName;
	std::wstring					m_strOtherName;
#else
	std::string						m_strPrimiorName;
	std::string						m_strOtherName;
#endif
	enum							OtherPosition { SCREEN_LEFT = 0, SCREEN_RIGHT, SCREEN_TOP, SCREEN_DOWN };//副屏幕相对于主屏幕的位置
	OtherPosition					GetOtherPosition();//得到副屏幕相对于主屏幕的位置
	RECT							GetRect(HMONITOR hMonitor);//得到窗口的虚拟坐标。

	std::vector<ScreenInfo>			m_vPrimerMonitor;//主屏幕的硬件参数
	std::vector<ScreenInfo>			m_vOtherMointor;//副屏幕的硬件参数
	std::vector<HwndANDPosition>	m_hpVSWindows;
	ScreenInfo						GetDefaultScreenInfo(bool PrimiorMonitor);//得到默认的显示设置，true为返回的是主屏幕
	virtual bool					Init();
	virtual void					Restore();
	virtual							~CMutiScreen();
private:
#ifdef UNICODE
	void							InitScreenInfo(HMONITOR hMonitor, std::vector<ScreenInfo> &vec, std::wstring &strName);//给指定的屏幕，对其设置排序
#else
	void							InitScreenInfo(HMONITOR hMonitor, std::vector<ScreenInfo> &vec, std::string &strName);//给指定的屏幕，对其设置排序
#endif
	HWND							m_hCurrentMainHWND;
	HWND							m_hVSMainHWND;
};
void								YYSetConsoleA(HWND = nullptr);//设置名为“DebugConsole”的位置为第二屏幕的控制台
void								YYSetConsoleW(HWND = nullptr);//设置名为“DebugConsole”的位置为第二屏幕的控制台
void								YYSetConsoleA(std::string file_name);//把输入定义到某文件，用来打log
void								YYSetConsoleW(std::wstring file_name);
CMutiScreen*						GetMutiScreen();//单例模式，得到CMutiScrren
