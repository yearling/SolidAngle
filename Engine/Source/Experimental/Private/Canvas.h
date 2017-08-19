#pragma once
#include "YYUT.h"
#include "VertexDef.h"
#include "IShader.h"
#include "YYUTCamera.h"


class Y3DCanvas
{
public:
	Y3DCanvas();
	void							SetCamera(ICamera* pCamera) { m_pViewCamera = pCamera; }
	void DrawLine(XMFLOAT3 StartPos, XMFLOAT3 EndPos, XMFLOAT4 Color);
	void Render();
	void Init();
private:
	TComPtr<ID3D11Buffer>			m_VB;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
	TArray<LocalVertex>		LineDatas;
	TUniquePtr<YVSShader>		m_VSShader;
	TUniquePtr<YPSShader>		m_PSShader;
	const int						m_MAXVertex = 1024;
	ICamera*					m_pViewCamera;
	
};

extern Y3DCanvas* GCanvas;

