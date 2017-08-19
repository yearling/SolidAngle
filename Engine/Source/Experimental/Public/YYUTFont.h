#pragma once
#include "YYUT.h"
#include <vector>

struct FontNode
{
	WCHAR strFace[MAX_PATH];
	LONG nHeight;
	LONG nWeight;
};

struct YYSpriteVertex
{
	XMFLOAT3 vPos;
	XMFLOAT4 vColor;
	XMFLOAT2 vTex;
};

class YYUTFont
{
public:
	YYUTFont();
	void							Init();

	void							EndFont();
	void							BeginText();

	void							DrawText(std::string strText,
		const RECT& rcScreen,
		XMFLOAT4 vFontColor,
		float fBBWidth,
		float fBBHeight,
		bool bCenter);


	void							SetInsertionPos(int x, int y);
	void							SetForegroundColor(XMFLOAT4 clr);
	void							DrawTextLine(std::string);
protected:
	void							RenderText();
	TComPtr<ID3D11Buffer>			m_FontBuffer;
	UINT							m_FontBufferBytes;
	std::vector<YYSpriteVertex>		m_vecFontVertices;
	TComPtr<ID3D11ShaderResourceView> m_FontSRV;
	TComPtr<ID3D11InputLayout>		m_FontInputLayout;
	TComPtr<ID3D11Device>			m_device;
	TComPtr<ID3D11DeviceContext>    m_dc;
	TComPtr<ID3D11VertexShader>		m_VS;
	TComPtr<ID3D11PixelShader>		m_PS;
	TComPtr<ID3D11BlendState>		m_BlendStateAlpha;
	TComPtr<ID3D11RasterizerState>	m_RS;
	TComPtr<ID3D11DepthStencilState>m_DS;
	TComPtr<ID3D11RenderTargetView> m_RTV;
	TComPtr<ID3D11DepthStencilView> m_DSV;
	TComPtr<ID3D11SamplerState>		m_SampleWrap;

	POINT							m_pt;
	int								m_nLineHeight;
	XMFLOAT4						m_clr;
	int								m_BackBufferWidth;
	int								m_BackBufferHeight;
};