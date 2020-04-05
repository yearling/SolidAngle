#include "Canvas.h"
#include "YYUTHelper.h"
#include "YYUTDXManager.h"

Y3DCanvas gCanvas;
Y3DCanvas* GCanvas = &gCanvas;

Y3DCanvas::Y3DCanvas()
{
	m_VSShader = MakeUnique<YVSShader>();
	m_PSShader = MakeUnique<YPSShader>();
}

void Y3DCanvas::Init()
{
	LineDatas.Empty();
	TArray<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_VSShader->BindInputLayout(layout);
	m_VSShader->CreateShader("..\\..\\Source\\Experimental\\Private\\Canvas.hlsl", "VSMain");
	m_PSShader->CreateShader("..\\..\\Source\\Experimental\\Private\\Canvas.hlsl", "PSColor");
	CreateRasterStateNonCull(m_rs);
	CreateBlendState(m_bs, false, "m_BlendOpaque");
	CreateDepthStencileStateNoWriteNoTest(m_ds, "m_DS_Test");
	CreateVertexBufferDynamic( sizeof(LocalVertex) * m_MAXVertex, nullptr, m_VB, "line vb");
}

void Y3DCanvas::DrawLine(FVector StartPos, FVector EndPos, FLinearColor Color)
{
	LocalVertex Vertex;
	Vertex.Position = StartPos;
	Vertex.Color = Color;
	LineDatas.Add(Vertex);
	Vertex.Position = EndPos;
	Vertex.Color = Color;
	LineDatas.Add(Vertex);
}

void Y3DCanvas::DrawBall(FVector Pos, FLinearColor Color, float Length /*= 0.1f*/)
{
	FVector Point0(-1, 1, -1);
	FVector Point1(1, 1, -1);
	FVector Point2(1, -1, -1);
	FVector Point3(-1, -1, -1);
	FVector Point4(-1, 1, 1);
	FVector Point5(1, 1, 1);
	FVector Point6(1, -1, 1);
	FVector Point7(-1, -1, 1);

	DrawLine(Point0*Length+ Pos, Point1*Length+Pos, Color);
	DrawLine(Point0*Length+Pos, Point3*Length+Pos, Color);
	DrawLine(Point0*Length+Pos, Point4*Length+Pos, Color);
	DrawLine(Point1*Length+Pos, Point2*Length+Pos, Color);
	DrawLine(Point1*Length+Pos, Point5*Length+Pos, Color);
	DrawLine(Point2*Length+Pos, Point3*Length+Pos, Color);
	DrawLine(Point2*Length+Pos, Point6*Length+Pos, Color);
	DrawLine(Point3*Length+Pos, Point7*Length+Pos, Color);
	DrawLine(Point4*Length+Pos, Point5*Length+Pos, Color);
	DrawLine(Point4*Length+Pos, Point7*Length+Pos, Color);
	DrawLine(Point5*Length+Pos, Point6*Length+Pos, Color);
	DrawLine(Point6*Length+Pos, Point7*Length+Pos, Color);
}

void Y3DCanvas::Render(TSharedRef<YRenderInfo> RenderInf)
{
	TComPtr<ID3D11DeviceContext>& DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	DeviceContext->RSSetState(m_rs);
	DeviceContext->OMSetDepthStencilState(m_ds, 0);
	float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	DeviceContext->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
	D3D11_MAPPED_SUBRESOURCE MapResource;
	HRESULT hr = DeviceContext->Map(m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
	if (LineDatas.Num())
	{
		memcpy(MapResource.pData, &LineDatas[0], sizeof(LocalVertex)* FMath::Min((int)LineDatas.Num(),m_MAXVertex));
	}
	DeviceContext->Unmap(m_VB, 0);

	m_VSShader->BindResource("g_view", RenderInf->RenderCameraInfo.View);
	m_VSShader->BindResource("g_projection", RenderInf->RenderCameraInfo.Projection);
	m_VSShader->BindResource("g_VP", RenderInf->RenderCameraInfo.ViewProjection);
	m_VSShader->BindResource("g_InvVP", RenderInf->RenderCameraInfo.ViewProjectionInv);
	m_VSShader->BindResource("g_world", FMatrix::Identity);
	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	UINT tride = (UINT)sizeof(LocalVertex);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &tride, &offset);

	m_VSShader->Update();
	m_PSShader->Update();
	DeviceContext->Draw(LineDatas.Num(), 0);
	LineDatas.Empty();
}
