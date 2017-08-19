#pragma once
#include "YYUT.h"
#include <templates/SharedPointer.h>
class YYUTWindow
{
public:
	virtual ~YYUTWindow() {};
	YYUTWindow() {};
	YYUTWindow(HINSTANCE hinstance) :m_hWnd(NULL), m_hMainInstance(hinstance), m_bIsWindowed(true) {};
	void Init(int _width, int Height, const TCHAR *);
	void SetInstance(HINSTANCE hIns) { m_hMainInstance = hIns; }
	operator HWND() { return m_hWnd; }
public:
	HINSTANCE m_hMainInstance;
	HWND m_hWnd;
	int m_iWidth;
	int m_iHeight;
	bool m_bIsWindowed;
};
class YYUTApplication
{
public:
	friend YYUTWindow;
	YYUTApplication(void);
	virtual ~YYUTApplication(void);
	virtual void WindowCreate(int width, int height, const TCHAR *, int = SW_SHOW);
	virtual void Update(float ElapseTime);
	virtual void Render();
	virtual void Initial();
	virtual int Run();
	virtual void Exit();
protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
private:
	void * operator new(size_t) {};
	void * operator new[](size_t) {};
	void operator delete(void *) {};
	void operator delete[](void *) {};

public:
	TSharedPtr<YYUTWindow> m_spMainWindow;
	HINSTANCE m_hMainInstance;

};

extern YYUTApplication *YYUT_application;
YYUTApplication *GetApplication();