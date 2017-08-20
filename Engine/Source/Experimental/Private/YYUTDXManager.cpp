#include "YYUTDXManager.h"
#include "YYUTHelper.h"
#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;
YYUTDXManager::YYUTDXManager(void) : m_iWidth(0),
m_iHeight(0),
m_bPauseTime(true),
m_bInitialed(false),
m_bPauseRender(false),
m_FPS(0),
m_lastFPSTime(0.0f),
m_FPSCounts(0)
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_bPauseTime = true;
	m_bInitialed = false;
	m_bPauseRender = true;
}


YYUTDXManager::~YYUTDXManager(void)
{
}

void YYUTDXManager::Init(HWND hwnd)
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL d3d_feature_levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};
	D3D_FEATURE_LEVEL pCreate_level;
	int feature_counts = _countof(d3d_feature_levels);
	assert(feature_counts != 0 && "D3D_FEATURE_LEVEL should not be empty!!");
	if (FAILED(hr = D3D11CreateDevice(NULL/*default adapter*/,
		D3D_DRIVER_TYPE_HARDWARE/*硬件加速*/,
		NULL/*not software*/,
		0,/*not flag*/
		d3d_feature_levels,/*feature levels counts*/
		feature_counts,
		D3D11_SDK_VERSION,
		&m_Device,
		&pCreate_level,
		&m_DC
	)))
	{
		std::string str_err = "D3D11CreaeteDevice failed";
	}
	if (pCreate_level != D3D_FEATURE_LEVEL_11_0)
	{
	}
	UINT MSAA4x = 0;
	m_Device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &MSAA4x);
	assert(MSAA4x > 0);
	cout << "max of MSAA: " << MSAA4x << endl;
	m_SampeDesc.Count = 1;
	m_SampeDesc.Quality = 0;
	m_hWnd = hwnd;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	cout << "rc.left: " << rc.left << "	rc.right: " << rc.right << "	width is:" << rc.right - rc.left << endl;
	cout << "rc.top: " << rc.top << "		rc.bottom: " << rc.bottom << "	height is:" << rc.bottom - rc.top << endl;
	m_iWidth = rc.right - rc.left;
	m_iHeight = rc.bottom - rc.top;
	DXGI_SWAP_CHAIN_DESC chain_des;
	chain_des.BufferDesc.Width = m_iWidth;
	chain_des.BufferDesc.Height = m_iHeight;
	chain_des.BufferDesc.RefreshRate.Numerator = 60;
	chain_des.BufferDesc.RefreshRate.Denominator = 1;
	chain_des.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	chain_des.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	chain_des.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	chain_des.SampleDesc = m_SampeDesc;
	chain_des.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chain_des.BufferCount = 1;
	chain_des.OutputWindow = m_hWnd;
	chain_des.Windowed = TRUE;
	chain_des.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//chain_des.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE ;
	chain_des.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	TComPtr<IDXGIFactory> dxgi_factory;
	TComPtr<IDXGIDevice> dxgi_device;
	TComPtr<IDXGIAdapter> dxgi_adapter;
	if (FAILED(hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device)))
	{
	}
	if (FAILED(hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter)))
	{
	}
	if (FAILED(hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory)))
	{
	}

	if (FAILED(hr = dxgi_factory->CreateSwapChain(m_Device, &chain_des, &m_SwapChain)))
	{
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	m_bInitialed = true;
}

YYUTDXManager & YYUTDXManager::GetInstance()
{
	return m_hInstance;
}

TComPtr<ID3D11Device> YYUTDXManager::GetD3DDevice()
{
	return m_Device;
}

TComPtr<ID3D11DeviceContext> YYUTDXManager::GetD3DDC()
{
	return m_DC;
}

void YYUTDXManager::ReSize(int width, int height)
{
	m_iWidth = width;
	m_iHeight = height;
	assert(m_Device);
	assert(m_DC);
	assert(m_SwapChain);

	m_RTV = nullptr;
	m_DSV = nullptr;
	m_DepthStencilTexture = nullptr;

	HRESULT hr = S_OK;
	if (FAILED(hr = m_SwapChain->ResizeBuffers(1, m_iWidth, m_iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
	{
	}
	TComPtr<ID3D11Texture2D> back_buffer;
	if (FAILED(hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer))))
	{
	}
	if (FAILED(hr = m_Device->CreateRenderTargetView(back_buffer, 0, &m_RTV)))
	{
	}


	Create2DTextureDSV_SRV(m_Device, m_iWidth, m_iHeight, DXGI_FORMAT_R32_TYPELESS, m_DepthStencilTexture, "m_DepthStencilTexture");
	CreateDSVForTexture2D(m_Device, DXGI_FORMAT_D32_FLOAT, m_DepthStencilTexture, m_DSV, "depth_stencil_dsv");
	CreateSRVForTexture2D(m_Device, DXGI_FORMAT_R32_FLOAT, m_DepthStencilTexture, m_SRVforDS, "depth_stencil_srv");
	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	m_ViewPort.Width = (float)m_iWidth;
	m_ViewPort.Height = (float)m_iHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_DC->RSSetViewports(1, &m_ViewPort);
}

void YYUTDXManager::KeyboardMouseProc(void)
{

}

void YYUTDXManager::Render(void)
{
	for(auto &Func:m_ListRenderEvents)
		(Func)();
	m_SwapChain->Present(0, 0);
	m_FPSCounts++;
}

void YYUTDXManager::Tick(DWORD elapse_time)
{

}

void YYUTDXManager::PauseTime(bool pause)
{
}

void YYUTDXManager::PauseRender(bool pause)
{
}

bool YYUTDXManager::IsInitialized(void)
{
	return m_bInitialed;
}

void YYUTDXManager::PauseAll(bool pause)
{
	PauseTime(pause);
	PauseRender(pause);
}

void YYUTDXManager::SetKeyboardMouseEvent(DX_KEYBOARD_MOUSE_EVENT key_event)
{
	m_EventMouseKeyborad = key_event;
}

void YYUTDXManager::AddRenderEvent(DX_RENDER_EVENT render_event)
{
	m_ListRenderEvents.Add(render_event);
}

TComPtr<ID3D11DepthStencilView> YYUTDXManager::GetDepthStencilView(void)
{
	return m_DSV;
}

TComPtr<ID3D11RenderTargetView> YYUTDXManager::GetRenderTargetView(void)
{
	return m_RTV;
}

TComPtr<ID3D11ShaderResourceView> YYUTDXManager::GetDepthStecilSRV(void)
{
	return m_SRVforDS;
}

UINT YYUTDXManager::GetBackBufferWidth()
{
	return m_iWidth;
}

UINT YYUTDXManager::GetBackBufferHeight()
{
	return m_iHeight;
}

UINT YYUTDXManager::GetFPS()
{
	return m_FPS;
}

YYUTDXManager YYUTDXManager::m_hInstance;
