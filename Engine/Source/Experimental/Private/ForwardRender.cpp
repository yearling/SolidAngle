#include "ForwardRender.h"
#include "SStaticMesh.h"
#include "YMeshBatch.h"
#include "YYUTHelper.h"
#include "WICTextureLoader.h"
DEFINE_LOG_CATEGORY(LogForwardRender)

struct YDirectionalLightDefaultPolicy :public IYRenderPolicy
{
public:
	YDirectionalLightDefaultPolicy() {};
	~YDirectionalLightDefaultPolicy() override
	{

	}
	void Render(TArray<YMeshBatch>& MeshBatchArray, TSharedRef<YRenderInfo> RenderInfo) override
	{
		check(Initialized);
		TComPtr<ID3D11DeviceContext>& dc = YYUTDXManager::GetInstance().GetD3DDC();
		float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
		dc->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
		dc->RSSetState(m_rs);
		dc->OMSetDepthStencilState(m_ds, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (YMeshBatch& MeshBatch : MeshBatchArray)
		{
			UINT stridPosition = 12;
			uint32 StridTangeUV = 16;
			UINT offset = 0;
			VSShader->BindResource(TEXT("g_view"), RenderInfo->RenderCameraInfo.View);
			VSShader->BindResource(TEXT("g_projection"), RenderInfo->RenderCameraInfo.Projection);
			VSShader->BindResource(TEXT("g_VP"), RenderInfo->RenderCameraInfo.ViewProjection);
			VSShader->BindResource(TEXT("g_InvVP"), RenderInfo->RenderCameraInfo.ViewProjectionInv);
			VSShader->BindResource(TEXT("g_world"), FMatrix::Identity);
			VSShader->Update();

			PSShader->BindResource(TEXT("g_lightDir"), RenderInfo->SceneInfo.MainLightDir.GetSafeNormal());
			PSShader->BindSRV(TEXT("txDiffuse"), ShaderResourceView);
			PSShader->Update();

			// RHI Ã»Ð´Íê
			ID3D11Buffer* VBPosition = MeshBatch.VertexPositionBuffer->VertexBufferRHI->VertexBuffer.GetReference();
			dc->IASetVertexBuffers(0, 1, &(VBPosition), &stridPosition, &offset);
			ID3D11Buffer* VBTangentUV = MeshBatch.VertexBuffer->VertexBufferRHI->VertexBuffer.GetReference();
			dc->IASetVertexBuffers(1, 1, &(VBTangentUV), &StridTangeUV, &offset);
			dc->PSSetSamplers(0, 1, &SamplerState);
			for (YMeshBatchElement& MeshBatchElement : MeshBatch.Elements)
			{
				TRefCountPtr<YRHIIndexBuffer> IndexBuffer = MeshBatch.Elements[0].IndexBuffer->IndexBufferRHI;
				if (IndexBuffer->Stride == 4)
				{
					dc->IASetIndexBuffer(IndexBuffer->IndexBuffer.GetReference(), DXGI_FORMAT_R32_UINT, 0);
				}
				else
				{
					dc->IASetIndexBuffer(IndexBuffer->IndexBuffer.GetReference(), DXGI_FORMAT_R16_UINT, 0);
				}
				{
					uint32 nIndexStart = MeshBatchElement.FirstIndex;
					uint32 nTriangleCount = MeshBatchElement.NumPrimitives;
					dc->DrawIndexed(nTriangleCount * 3, nIndexStart, 0);
				}
			}
		}
	}
	void InitRenderReosurce() override
	{
		VSShader = MakeUnique<YVSShader>();
		TArray<D3D11_INPUT_ELEMENT_DESC> Layout =
		{
			{ "ATTRIBUTE",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ATTRIBUTE",  1, DXGI_FORMAT_R8G8B8A8_SNORM,  1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ATTRIBUTE",  2, DXGI_FORMAT_R8G8B8A8_SNORM,  1, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ATTRIBUTE",  3, DXGI_FORMAT_R16G16_FLOAT,    1, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ATTRIBUTE",  4, DXGI_FORMAT_R16G16_FLOAT,    1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		VSShader->BindInputLayout(Layout);
		if (!VSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\StaticMesh2.hlsl"), TEXT("VSMain")))
		{
			check(0);
		}
		PSShader = MakeUnique<YPSShader>();
		if (!PSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\StaticMesh2.hlsl"), TEXT("PSMain")))
		{
			check(0);
		}
		CreateRasterState(m_rs);
		CreateBlendState(m_bs, true, "m_BlendOpaque");
		CreateDepthStencileState(m_ds, true, "m_DS_Test");

		//read texture
		TComPtr<ID3D11Device>& device = YYUTDXManager::GetInstance().GetD3DDevice();
		TComPtr<ID3D11DeviceContext>& dc = YYUTDXManager::GetInstance().GetD3DDC();
		ID3D11Texture2D* TextureResource = nullptr;
		ID3D11ShaderResourceView* SRV = nullptr;
		DirectX::CreateWICTextureFromFile((ID3D11Device*)device, (ID3D11DeviceContext*)dc, TEXT("mirror_nija_no_seam_default_Normal.png"), (ID3D11Resource**)&TextureResource, &SRV, 0);
		NormalTexture.Attach((ID3D11Texture2D*)TextureResource);
		ShaderResourceView.Attach(SRV);
		CreateSamplerPointWrap(SamplerState);
		Initialized = true;
	}
	void ReleaseRenderResource() override
	{
		VSShader = nullptr;
		PSShader = nullptr;
		m_bs = nullptr;
		m_ds = nullptr;
		m_rs = nullptr;
		Initialized = false;
	}
	TUniquePtr<YVSShader>		VSShader;
	TUniquePtr<YPSShader>		PSShader;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11SamplerState> SamplerState;
	TComPtr<ID3D11Texture2D> NormalTexture;
	TComPtr<ID3D11ShaderResourceView> ShaderResourceView;
	bool Initialized = false;
};

YForwardRender::YForwardRender()
{

}

YForwardRender::~YForwardRender()
{

}

void YForwardRender::InitRenders()
{
	DirectionalLightRenderPolicy = MakeUnique<YDirectionalLightDefaultPolicy>();
	DirectionalLightRenderPolicy->InitRenderReosurce();
	CreateRasterState(m_rs);
	CreateBlendState(m_bs, true, "m_BlendOpaque");
	CreateDepthStencileState(m_ds, true, "m_DS_Test");
}

void YForwardRender::RenderScene(TRefCountPtr<YScene>& Scene, TSharedRef<YRenderInfo> RenderInfo)
{
	HRESULT hr = S_OK;
	float backcolor[4] = { 0.0f,0.0f,0.0f,0.0f };

	TComPtr<ID3D11DeviceContext>& DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	DeviceContext->ClearRenderTargetView(YYUTDXManager::GetInstance().GetRenderTargetView(), backcolor);
	DeviceContext->ClearDepthStencilView(YYUTDXManager::GetInstance().GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	DeviceContext->RSSetState(m_rs);
	float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	DeviceContext->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
	DeviceContext->OMSetDepthStencilState(m_ds, 0);
	DeviceContext->OMSetRenderTargets(1, &YYUTDXManager::GetInstance().GetRenderTargetView(), YYUTDXManager::GetInstance().GetDepthStencilView());
	DeviceContext->RSSetViewports(1, YYUTDXManager::GetInstance().GetDefaultViewPort());

	TArray<YMeshBatch> & PrimitiveArray = Scene->StaticMeshOpaquePrimitives;
	DirectionalLightRenderPolicy->Render(PrimitiveArray, RenderInfo);
}

