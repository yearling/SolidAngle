#include "YYUT.h"
#include "YYUTHelper.h"
#include "YYUTFont.h"
#include "YYUTDXManager.h"

void YYUTFont::Init()
{

	m_device = YYUTDXManager::GetInstance().GetD3DDevice();
	m_dc = YYUTDXManager::GetInstance().GetD3DDC();
	m_DSV = YYUTDXManager::GetInstance().GetDepthStencilView();
	m_RTV = YYUTDXManager::GetInstance().GetRenderTargetView();
	m_BackBufferWidth = YYUTDXManager::GetInstance().GetBackBufferWidth();
	m_BackBufferHeight = YYUTDXManager::GetInstance().GetBackBufferHeight();
	CreateSRVFromeFile( TEXT("media\\Font.dds"), m_FontSRV, TEXT("Font::m_FontSRV"));
	CreateVertexShader(TEXT("..\\..\\Source\\Experimental\\Private\\UI.hlsl"), TEXT("VSmain"), m_VS, TEXT("Font::m_FontVS"));
	CreatePixelShader(TEXT("..\\..\\Source\\Experimental\\Private\\UI.hlsl"), TEXT("PSmain"), m_PS, TEXT("Font::m_PS"));
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CreateInputLayout(TEXT("..\\..\\Source\\Experimental\\Private\\UI.hlsl"), TEXT("VSmain"), layout, _countof(layout), m_FontInputLayout, "Font::m_FontInputLayOut");
	CreateBlendState(m_BlendStateAlpha, false, "Font::BlendState");
	CreateDepthStencileState(m_DS, false, "Font::DS State");
	CreateSamplerLinearWrap( m_SampleWrap, "Font::SamepleWrap");
	//////////////////////////////////////////////////////////////////////////
	m_clr = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_pt.x = 0;
	m_pt.y = 0;
	m_nLineHeight = 15;
}

YYUTFont::YYUTFont() :m_FontBufferBytes(0),
m_nLineHeight(0),
m_BackBufferWidth(0),
m_BackBufferHeight(0)
{

}


void YYUTFont::EndFont()
{
	m_FontBufferBytes = 0;
}

void YYUTFont::BeginText()
{
	m_vecFontVertices.Empty();
	m_pt.x = 0.0f;
	m_pt.y = 0.0f;
}

void YYUTFont::DrawText(const FString strText,
	const RECT& rcScreen,
	FLinearColor vFontColor,
	float fBBWidth,
	float fBBHeight,
	bool bCenter)
{
	float fCharTexSizeX = 0.010526315f;
	//float fGlyphSizeX = 14.0f / fBBWidth;
	//float fGlyphSizeY = 32.0f / fBBHeight;
	float fGlyphSizeX = 15.0f / fBBWidth;
	float fGlyphSizeY = 42.0f / fBBHeight;


	float fRectLeft = rcScreen.left / fBBWidth;
	float fRectTop = 1.0f - rcScreen.top / fBBHeight;

	fRectLeft = fRectLeft * 2.0f - 1.0f;
	fRectTop = fRectTop * 2.0f - 1.0f;

	int NumChars = (int)strText.Len();
	if (bCenter) {
		float fRectRight = rcScreen.right / fBBWidth;
		fRectRight = fRectRight * 2.0f - 1.0f;
		float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
		fRectBottom = fRectBottom * 2.0f - 1.0f;
		float fcenterx = ((fRectRight - fRectLeft) - (float)NumChars*fGlyphSizeX) *0.5f;
		float fcentery = ((fRectTop - fRectBottom) - (float)1 * fGlyphSizeY) *0.5f;
		fRectLeft += fcenterx;
		fRectTop -= fcentery;
	}
	float fOriginalLeft = fRectLeft;
	float fTexTop = 0.0f;
	float fTexBottom = 1.0f;

	float fDepth = 0.5f;
	for (int i = 0; i < NumChars; i++)
	{
		if (strText[i] == '\n')
		{
			fRectLeft = fOriginalLeft;
			fRectTop -= fGlyphSizeY;

			continue;
		}
		else if (strText[i] < 32 || strText[i] > 126)
		{
			continue;
		}

		// Add 6 sprite vertices
		YYSpriteVertex SpriteVertex;
		float fRectRight = fRectLeft + fGlyphSizeX;
		float fRectBottom = fRectTop - fGlyphSizeY;
		float fTexLeft = (strText[i] - 32) * fCharTexSizeX;
		float fTexRight = fTexLeft + fCharTexSizeX;

		// tri1
		SpriteVertex.vPos = FVector(fRectLeft, fRectTop, fDepth);
		SpriteVertex.vTex = FVector2D(fTexLeft, fTexTop);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = FVector(fRectRight, fRectTop, fDepth);
		SpriteVertex.vTex = FVector2D(fTexRight, fTexTop);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = FVector(fRectLeft, fRectBottom, fDepth);
		SpriteVertex.vTex = FVector2D(fTexLeft, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		// tri2
		SpriteVertex.vPos = FVector(fRectRight, fRectTop, fDepth);
		SpriteVertex.vTex = FVector2D(fTexRight, fTexTop);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = FVector(fRectRight, fRectBottom, fDepth);
		SpriteVertex.vTex = FVector2D(fTexRight, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		SpriteVertex.vPos = FVector(fRectLeft, fRectBottom, fDepth);
		SpriteVertex.vTex = FVector2D(fTexLeft, fTexBottom);
		SpriteVertex.vColor = vFontColor;
		m_vecFontVertices.Add(SpriteVertex);

		fRectLeft += fGlyphSizeX;

	}

	// We have to end text after every line so that rendering order between sprites and fonts is preserved
	RenderText();
}

void YYUTFont::RenderText()
{
	if (!m_vecFontVertices.Num())
		return;

	// ensure our buffer size can hold our sprites
	uint32 FontDataBytes = static_cast<uint32>(m_vecFontVertices.Num() * sizeof(YYSpriteVertex));
	if (m_FontBufferBytes < FontDataBytes)
	{
		m_FontBuffer = NULL;
		m_FontBufferBytes = FontDataBytes;
		CreateVertexBufferDynamic( m_FontBufferBytes, NULL, m_FontBuffer, "m_FontBuffer");
	}

	// Copy the sprites over
	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = FontDataBytes;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (S_OK == m_dc->Map(m_FontBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource))
	{
		memcpy(MappedResource.pData, (void*)&m_vecFontVertices[0], FontDataBytes);
		m_dc->Unmap(m_FontBuffer, 0);
	}

	TComPtr<ID3D11ShaderResourceView> pOldTexture;
	m_dc->PSGetShaderResources(0, 1, &pOldTexture);
	m_dc->PSSetShaderResources(0, 1, &m_FontSRV);

	// Draw
	UINT Stride = sizeof(YYSpriteVertex);
	UINT Offset = 0;
	m_dc->IASetVertexBuffers(0, 1, &m_FontBuffer, &Stride, &Offset);
	m_dc->IASetInputLayout(m_FontInputLayout);
	m_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_dc->VSSetShader(m_VS, NULL, 0);
	m_dc->PSSetShader(m_PS, NULL, 0);
	m_dc->PSSetSamplers(0, 1, &m_SampleWrap);
	m_dc->OMSetDepthStencilState(m_DS, 0);
	float blend[] = { 0.0f };
	m_dc->OMSetBlendState(m_BlendStateAlpha, blend, 0xffffffff);
	m_dc->OMSetRenderTargets(1, &m_RTV, m_DSV);
	m_dc->Draw(static_cast<UINT>(m_vecFontVertices.Num()), 0);
	m_dc->PSSetShaderResources(0, 1, &pOldTexture);
	m_vecFontVertices.Empty();
}

void YYUTFont::SetInsertionPos(int x, int y)
{
	m_pt.x = x;
	m_pt.y = y;
}

void YYUTFont::SetForegroundColor(FLinearColor clr)
{
	m_clr = clr;
}

void YYUTFont::DrawTextLine(const FString& str)
{
	RECT rc;
	SetRect(&rc, m_pt.x, m_pt.y, 0, 0);
	DrawText(str, rc, m_clr, (float)m_BackBufferWidth, (float)m_BackBufferHeight, false);
	m_pt.y += m_nLineHeight;
}