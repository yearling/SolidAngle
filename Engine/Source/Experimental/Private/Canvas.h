#pragma once
#include "YYUT.h"
#include "VertexDef.h"
#include "IShader.h"
#include "YYUTCamera.h"
#include "RenderInfo.h"

class Y3DCanvas
{
public:
	Y3DCanvas();
	void DrawLine(FVector StartPos, FVector EndPos, FLinearColor Color);
	void						Render(TSharedRef<FRenderInfo> RenderInf);
	void						Init();
private:
	TComPtr<ID3D11Buffer>			m_VB;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
	TArray<LocalVertex>		LineDatas;
	TUniquePtr<YVSShader>		m_VSShader;
	TUniquePtr<YPSShader>		m_PSShader;
	const int						m_MAXVertex = 1024;
};

extern Y3DCanvas* GCanvas;

