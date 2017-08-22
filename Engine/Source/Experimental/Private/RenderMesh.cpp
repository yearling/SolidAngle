#include "YYUT.h"
#include "RenderMesh.h"
#include "YYUTHelper.h"
#include "YYUTDXManager.h"
#include "StaticMesh.h"
#include "Canvas.h"
#include "IShader.h"




	RenderScene::RenderScene(void)
	{

	}

	RenderScene::~RenderScene(void)
	{

	}

	void RenderScene::Init()
	{
		CreateVertexShader(m_device,"..\\..\\Source\\Experimental\\Private\\RenderMesh.hlsl","VSMain",m_vs,"sdkmesh_vs");
		CreatePixelShader(m_device,"..\\..\\Source\\Experimental\\Private\\RenderMesh.hlsl","PSMain",m_ps,"sdkMesh_ps");
		CreateConstantBufferCPUWrite(m_device,sizeof(RenderMeshCBuffer),m_cbPerFrame,"cbPerframe");
		CreateSamplerLinearWrap(m_device,m_ss,"SampelStateWrap");
		CreateRasterState(m_device,m_rs);
		CreateBlendState(m_device,m_bs,true,"m_BlendOpaque");
		CreateDepthStencileState(m_device,m_ds,true,"m_DS_Test");
		m_MainRenderTargetView = YYUTDXManager::GetInstance().GetRenderTargetView();
		m_MainDepthStencilView = YYUTDXManager::GetInstance().GetDepthStencilView();
		//m_pMesh->Init(m_device,m_dc);

		//XMFLOAT3 eye(100.0f,5.0f,5.f);
		//XMFLOAT3 lookat(0.0f,0.0f,0.0f);
		//m_pViewCamera->SetViewParam(eye,lookat);
		m_pViewCamera->FrameMove(0);
		XMFLOAT3 lightEye(-320,300,-220.3f);
		XMFLOAT3 ligntLookat(0.0f,0.0f,0.0f);
		m_pLightCamera->SetViewParam(lightEye,ligntLookat);
		m_pLightCamera->SetProjParam(XM_PIDIV4,1.0f,0.1f,1000.0f);
		FVector lightEyeF(-320,300,-220.3f);
		FVector ligntLookatF(0.0f,0.0f,0.0f);
		//m_pLightCamera->SetViewParamF(lightEyeF, ligntLookatF);
		//m_pLightCamera->SetProjParamF(PI / 4, 1.0f, 0.1f, 1000.0f);
		m_pLightCamera->FrameMove(0);
		CreateMeshResource();
	}

	void RenderScene::Update(float ElpaseTime)
	{
		if (m_pMesh)
		{
			m_pMesh->Tick();
		}
	}

	void RenderScene::Render()
	{
		static bool bSetAniStack = true;
		if (bSetAniStack)
		{
			m_pMesh->SetCurrentAnimStack(0);
			bSetAniStack = false;
		}
		Update(0);
		HRESULT hr = S_OK;
		float backcolor[4]= { 0.0f,0.0f,0.0f,0.0f};
		m_dc->ClearRenderTargetView(m_MainRenderTargetView,backcolor);
		m_dc->ClearDepthStencilView(m_MainDepthStencilView,D3D11_CLEAR_DEPTH,1.0f,0);
		m_dc->RSSetState(m_rs);
		float BlendColor[4]= { 1.0f,1.0f,1.0f,1.0f};
		m_dc->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
		m_dc->OMSetDepthStencilState(m_ds, 0);
		m_dc->OMSetRenderTargets(1,&m_MainRenderTargetView,m_MainDepthStencilView);
		m_dc->RSSetViewports(1,YYUTDXManager::GetInstance().GetDefaultViewPort());

		XMMATRIX matCameraProj = m_pViewCamera->GetProject();
		XMMATRIX matCameraView = m_pViewCamera->GetView();
		XMMATRIX matCamaraViewProj = m_pViewCamera->GetViewProject();

		//更新CB
#pragma region 更新CB
		D3D11_MAPPED_SUBRESOURCE MapResource;
		hr=m_dc->Map(m_cbPerFrame,0,D3D11_MAP_WRITE_DISCARD,0,&MapResource);
		if (FAILED(hr))
		{
			return;
		}
		RenderMeshCBuffer &cbShadowData = (*(RenderMeshCBuffer*)MapResource.pData);
		cbShadowData.m_matView = XMMatrixTranspose(m_pViewCamera->GetView());
		cbShadowData.m_matProj = XMMatrixTranspose(m_pViewCamera->GetProject());
		cbShadowData.m_matViewProj = XMMatrixTranspose(m_pViewCamera->GetViewProject());
		XMVECTOR deter;
		cbShadowData.m_matInvViewProj =XMMatrixTranspose(XMMatrixInverse(&deter, cbShadowData.m_matViewProj));
		XMStoreFloat3(&cbShadowData.m_lightDir,-XMVector3Normalize(m_pLightCamera->GetDir()));
		m_dc->Unmap(m_cbPerFrame,0);

		m_dc->PSSetSamplers(0,1,&m_ss);
		m_dc->VSSetShader(m_vs,NULL,0);
		m_dc->PSSetShader(m_ps,nullptr,0);
		m_dc->VSSetConstantBuffers(0,1,&m_cbPerFrame);
		m_dc->PSSetConstantBuffers(0,1,&m_cbPerFrame);
		//m_pMesh->RenderMesh();
		m_dc->IASetInputLayout(m_InputLayout);
	
		/*m_dc->DrawIndexed(m_pMesh->IndexArray.size(), 0, 0);*/
		if (m_pMesh)
		{
			m_pMesh->Render(m_dc);
		}


		// Draw Coordinate
		GCanvas->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(5, 0, 0), XMFLOAT4(1, 0, 0, 1));
		GCanvas->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 5, 0), XMFLOAT4(0, 1, 0, 1));
		GCanvas->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 5), XMFLOAT4(0, 0, 1, 1));
	}

	void RenderScene::CreateMeshResource()
	{
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		CreateInputLayout(m_device, "..\\..\\Source\\Experimental\\Private\\RenderMesh.hlsl", "VSMain", layout, _countof(layout), m_InputLayout, "sdkmesh_inputlayout");
		
		m_pMesh->Init(m_device, m_dc);
	}



