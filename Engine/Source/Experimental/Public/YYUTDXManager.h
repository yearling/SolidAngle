#pragma once
#include "YYUT.h"
#include <d3d11.h>

typedef	TFunction<void()>	DX_RENDER_EVENT;

class YYUTDXManager
{
public:

	typedef TFunction<void(int)>   DX_KEYBOARD_MOUSE_EVENT;
public:

	virtual							~YYUTDXManager(void);
	void							Init(HWND hwnd);

	static YYUTDXManager &			GetInstance(void);
	TComPtr<ID3D11Device>&			GetD3DDevice(void);
	TComPtr<ID3D11DeviceContext>&	GetD3DDC(void);
	TComPtr<ID3D11DepthStencilView>& GetDepthStencilView(void);
	TComPtr<ID3D11RenderTargetView>& GetRenderTargetView(void);
	TComPtr<ID3D11ShaderResourceView>& GetDepthStecilSRV(void);
	D3D11_VIEWPORT*					GetDefaultViewPort() { return &m_ViewPort; }
	virtual void					Tick(DWORD elapse_time);
	virtual void					Render(void);
	void							ReSize(int width, int height);
	virtual	void					KeyboardMouseProc(void);
	void							PauseTime(bool pause);
	void							PauseRender(bool pause);
	void							PauseAll(bool pause);
	bool							IsInitialized(void);
	void							SetKeyboardMouseEvent(DX_KEYBOARD_MOUSE_EVENT key_event);
	void							AddRenderEvent(DX_RENDER_EVENT	render_event);
	UINT							GetBackBufferWidth();
	UINT							GetBackBufferHeight();
	UINT							GetFPS();
protected:
	TComPtr<ID3D11Device>			m_Device;
	TComPtr<ID3D11DeviceContext>	m_DC;
	TComPtr<IDXGISwapChain>			m_SwapChain;
	TComPtr<ID3D11RenderTargetView> m_RTV;
	TComPtr<ID3D11Texture2D>		m_DepthStencilTexture;
	TComPtr<ID3D11DepthStencilView>	m_DSV;
	TComPtr<ID3D11ShaderResourceView> m_SRVforDS;
	DXGI_SAMPLE_DESC				m_SampeDesc;
	D3D11_VIEWPORT					m_ViewPort;
	int								m_iWidth;
	int								m_iHeight;
	int								m_FPS;
	int								m_FPSCounts;
private:
	YYUTDXManager(void);
	YYUTDXManager(const YYUTDXManager &);
	YYUTDXManager &					operator=(const YYUTDXManager &);

private:
	static YYUTDXManager			m_hInstance;
	HWND							m_hWnd;
	bool							m_bPauseTime;
	bool							m_bInitialed;
	bool							m_bPauseRender;
	DX_KEYBOARD_MOUSE_EVENT			m_EventMouseKeyborad;
	TArray<DX_RENDER_EVENT>			m_ListRenderEvents;
	float							m_lastFPSTime;
};
