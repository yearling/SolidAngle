#pragma once
#include "YYUT.h"

struct FontNode
{
	TCHAR strFace[MAX_PATH];
	LONG nHeight;
	LONG nWeight;
};

struct YYSpriteVertex
{
	FVector vPos;
	FLinearColor vColor;
	FVector2D vTex;
};

class YYUTFont
{
public:
	YYUTFont();
	void							Init();

	void							EndFont();
	void							BeginText();

	void DrawText(const FString strText,const RECT& rcScreen,FLinearColor vFontColor,float fBBWidth,float fBBHeight,bool bCenter);

	void							SetInsertionPos(int x, int y);
	void							SetForegroundColor(FLinearColor clr);
	void							DrawTextLine(const FString&);
protected:
	void							RenderText();
	TComPtr<ID3D11Buffer>			m_FontBuffer;
	uint32							m_FontBufferBytes;
	TArray<YYSpriteVertex>			m_vecFontVertices;
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
	FLinearColor					m_clr;
	int								m_BackBufferWidth;
	int								m_BackBufferHeight;
};