#pragma once
#include "YYUT.h"
#include "YYUTCamera.h"
#include "StaticMesh.h"
struct RenderMeshCBuffer
{
	XMMATRIX  m_matView;
	XMMATRIX  m_matProj;
	XMMATRIX  m_matViewProj;
	XMMATRIX  m_matInvViewProj;
	XMFLOAT3  m_lightDir;
};

class RenderScene
{
public:
	RenderScene(void);
	~RenderScene(void);
	virtual	void					Init();
	virtual void					Update(float ElpaseTime);
	virtual void					Render();
	void							SetCamera(ICamera* pCamera) { m_pViewCamera = pCamera; }
	void							SetLightCamera(ICamera* pCamera) { m_pLightCamera = pCamera; }
	void							SetDevice(TComPtr<ID3D11Device> device) { m_device = device; }
	void							SetDeviceContext(TComPtr<ID3D11DeviceContext>  dc) { m_dc = dc; }
	void							SetScreenWidthHeigth(int width, int height) { m_ScreenWidth = (float)width; m_ScreenHeight = (float)height; }
	void							SetMesh(std::unique_ptr<MeshModel> && pMesh) { m_pMesh = std::move(pMesh); }
	void							CreateMeshResource();

private:
	TComPtr<ID3D11Device>			m_device;
	TComPtr<ID3D11DeviceContext>	m_dc;
	TComPtr<ID3D11RenderTargetView>	m_MainRenderTargetView; //MainRenderTarget
	TComPtr<ID3D11DepthStencilView>	m_MainDepthStencilView; //MainDepthStencilView
	ICamera*					m_pViewCamera;
	ICamera*					m_pLightCamera;
	float							m_ScreenWidth;
	float							m_ScreenHeight;
	//std::shared_ptr<YYSDKMesh>		m_pMesh;			//³¡¾°mesh
	std::unique_ptr<MeshModel>     m_pMesh;
	TComPtr<ID3D11VertexShader>		m_vs;
	TComPtr<ID3D11PixelShader>		m_ps;
	TComPtr<ID3D11Buffer>			m_cbPerFrame;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
	TComPtr<ID3D11SamplerState>		m_ss;
	std::unique_ptr<class YVSShader> m_VertexShader;
	std::unique_ptr<class YPSShader> m_PixShader;
private:
	TComPtr<ID3D11InputLayout>		m_InputLayout;
};
