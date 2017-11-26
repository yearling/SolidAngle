#include "YYUT.h"
#include "RenderMesh.h"
#include "YYUTHelper.h"
#include "YYUTDXManager.h"
#include "StaticMesh.h"
#include "Canvas.h"
#include "IShader.h"
#include "YSkeletalMesh.h"




RenderScene::RenderScene(void):
	m_pSkeletalMeshData(nullptr)
{

}

RenderScene::~RenderScene(void)
{

}

void RenderScene::Init()
{
	TComPtr<ID3D11Device> Device = YYUTDXManager::GetInstance().GetD3DDevice();
	CreateRasterState(m_rs);
	CreateBlendState(m_bs, true, "m_BlendOpaque");
	CreateDepthStencileState(m_ds, true, "m_DS_Test");
	CreateMeshResource();
}

void RenderScene::Update(float ElpaseTime)
{
	if (m_pMesh)
	{
		m_pMesh->Tick();
	}
}

void RenderScene::Render(TSharedRef<FRenderInfo> RenderInfo)
{
	TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	static bool bSetAniStack = true;
	if (bSetAniStack)
	{
		m_pMesh->SetCurrentAnimStack(0);
		bSetAniStack = false;
	}
	Update(0);
	HRESULT hr = S_OK;
	float backcolor[4] = { 0.0f,0.0f,0.0f,0.0f };
	DeviceContext->ClearRenderTargetView(YYUTDXManager::GetInstance().GetRenderTargetView(), backcolor);
	DeviceContext->ClearDepthStencilView(YYUTDXManager::GetInstance().GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	DeviceContext->RSSetState(m_rs);
	float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	DeviceContext->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
	DeviceContext->OMSetDepthStencilState(m_ds, 0);
	DeviceContext->OMSetRenderTargets(1, &YYUTDXManager::GetInstance().GetRenderTargetView(), YYUTDXManager::GetInstance().GetDepthStencilView());
	DeviceContext->RSSetViewports(1, YYUTDXManager::GetInstance().GetDefaultViewPort());

	if (m_pMesh)
	{
		//m_pMesh->Render(RenderInfo);
	}
	DrawSkeletalMeshes();
	DrawGridAndCoordinates();
	for (TUniquePtr<FSkeletalMeshRenderHelper>& RenderHelper : SkeletalMeshRenderHeplers)
	{
		RenderHelper->Render(RenderInfo);
	}
	GCanvas->Render(RenderInfo);
}


void RenderScene::AllocResource()
{
	for (TUniquePtr<FSkeletalMeshRenderHelper>& RenderHelper : SkeletalMeshRenderHeplers)
	{
		RenderHelper->Init();
	}
}

void RenderScene::CreateMeshResource()
{
	m_pMesh->Init();
}

void RenderScene::DrawGridAndCoordinates()
{
	// DrawGrids
	float xStart = -100.0f;
	float xEnd = 100.0f;
	float zStart = -100.0f;
	float zEnd = 100.0f;
	float Grid = 10.0f;

	for (float xCurrent = xStart; xCurrent < xEnd + 1.0f; xCurrent += Grid)
	{
		GCanvas->DrawLine(FVector(xCurrent, 0, zStart), FVector(xCurrent, 0, zEnd), FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
	}
	for (float zCurrent = zStart; zCurrent < zEnd + 1.0f; zCurrent += Grid)
	{
		GCanvas->DrawLine(FVector(xStart, 0, zCurrent), FVector(xEnd, 0, zCurrent), FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
	}

	// Draw Coordinate
	GCanvas->DrawLine(FVector(0, 0, 0), FVector(5, 0, 0), FLinearColor(1, 0, 0, 1));
	GCanvas->DrawLine(FVector(0, 0, 0), FVector(0, 5, 0), FLinearColor(0, 1, 0, 1));
	GCanvas->DrawLine(FVector(0, 0, 0), FVector(0, 0, 5), FLinearColor(0, 0, 1, 1));
}

void RenderScene::DrawSkeletalMeshes()
{
	for (YSkeletalMesh* pMesh : SkeletalMeshes)
	{
		DrawSkeleton(pMesh);
		
	}
}

FMatrix GetParentMatrix(const TArray<FMeshBoneInfo>& MeshBoneInfoes, const TArray<FTransform> & BonePoses, int32 iIndex)
{
	FMatrix ParentMatrix = FMatrix::Identity;
	int32 ParentBoneIndex = MeshBoneInfoes[iIndex].ParentIndex;
	while (ParentBoneIndex != INDEX_NONE)
	{
		ParentMatrix = ParentMatrix* BonePoses[ParentBoneIndex].ToMatrixWithScale();
		ParentBoneIndex = MeshBoneInfoes[ParentBoneIndex].ParentIndex;
	}
	return ParentMatrix;
}

void RenderScene::DrawSkeleton(YSkeletalMesh* pSkeletalMesh)
{
	if (!pSkeletalMesh)
		return;
	const FReferenceSkeleton& Skeleton= pSkeletalMesh->RefSkeleton;
	const TArray<FMeshBoneInfo> &MeshBoneInfos = Skeleton.GetRefBoneInfo();
	const TArray<FTransform> & BonePoses = Skeleton.GetRawRefBonePose();
	for (int32 i = 0; i < MeshBoneInfos.Num(); ++i)
	{
		const FMeshBoneInfo & BoneInfo = MeshBoneInfos[i];
		if (BoneInfo.ParentIndex != INDEX_NONE)
		{
			FMatrix ParentMatrix = GetParentMatrix(MeshBoneInfos, BonePoses, i);
			FMatrix LocalMatrix = BonePoses[i].ToMatrixWithScale()*ParentMatrix;
			FVector ParentPos = ParentMatrix.TransformPosition(FVector(0, 0, 0));
			FVector LocalPos = LocalMatrix.TransformPosition(FVector(0, 0, 0));
			GCanvas->DrawLine(ParentPos, LocalPos, FLinearColor(1, 1, 0, 1));
			GCanvas->DrawBall(LocalPos,FLinearColor(1,0,0,1));
		}
		else
		{
			GCanvas->DrawBall(BonePoses[i].GetTranslation(), FLinearColor(1, 0, 0, 1));
		}
	}
}

void RenderScene::DrawMesh(YSkeletalMesh* pSkeletalMesh)
{
	if (!pSkeletalMesh)
		return;
	
}

void RenderScene::RegisterSkeletalMesh(YSkeletalMesh* pSkeletalMesh)
{
	SkeletalMeshes.AddUnique(pSkeletalMesh);
	SkeletalMeshRenderHeplers.Emplace(MakeUnique<FSkeletalMeshRenderHelper>(pSkeletalMesh));
}

FSkeletalMeshRenderHelper::FSkeletalMeshRenderHelper(YSkeletalMesh* InSkeletalMesh)
	:SkeletalMesh(InSkeletalMesh)
{

}

FSkeletalMeshRenderHelper::~FSkeletalMeshRenderHelper()
{

}

void FSkeletalMeshRenderHelper::Init()
{
	if (!SkeletalMesh)
		return;
	CreateRasterStateNonCull(m_rs);
	CreateBlendState(m_bs, true, "m_BlendOpaque");
	CreateDepthStencileState(m_ds, true, "m_DS_Test");
	VSShader = MakeUnique<YVSShader>();
	TArray<D3D11_INPUT_ELEMENT_DESC> Layout =
	{
		{ "ATTRIBUTE",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  1, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  2, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  3, DXGI_FORMAT_R8G8B8A8_UNORM,  0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  4, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  5, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  6, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  7, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  8, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  9, DXGI_FORMAT_R32G32B32A32_UINT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  10, DXGI_FORMAT_R32G32B32A32_UINT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  11, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 104, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  12, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 120, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	VSShader->BindInputLayout(Layout);
	if (!VSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\SkeletalMesh.hlsl"), TEXT("VSMain")))
	{
		check(0);
	}
	PSShader = MakeUnique<YPSShader>();
	if (!PSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\SkeletalMesh.hlsl"), TEXT("PSMain")))
	{
		check(0);
	}

	FSkeletalMeshResource* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	for (int32 i = 0; i < SkeletalMeshResource->LODModels.Num(); ++i)
	{
		FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[i];
		//TArray<FSoftSkinVertex> SkinVertex;
		StaticLodModel.GetVertices(SkinVertex);
		CreateVertexBufferDynamic((UINT)SkinVertex.Num() * sizeof(FSoftSkinVertex), &SkinVertex[0], VB);

		//FMultiSizeIndexContainerData IndexData;
		StaticLodModel.MultiSizeIndexContainer.GetIndexBufferData(IndexData);
		CreateIndexBuffer((UINT)IndexData.Indices.Num() * sizeof(uint32), &IndexData.Indices[0], IB);

	}
}

void FSkeletalMeshRenderHelper::Render(TSharedRef<FRenderInfo> RenderInfo)
{
	
	FSkeletalMeshResource* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	for (int32 i = 0; i < SkeletalMeshResource->LODModels.Num(); ++i)
	{
		FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[i];
		for (FSkelMeshSection& MeshSection : StaticLodModel.Sections)
		{
			if (MeshSection.NumTriangles == 0)
				return;
			VSShader->BindResource(TEXT("g_view"), RenderInfo->RenderCameraInfo.View);
			VSShader->BindResource(TEXT("g_projection"), RenderInfo->RenderCameraInfo.Projection);
			VSShader->BindResource(TEXT("g_VP"), RenderInfo->RenderCameraInfo.ViewProjection);
			VSShader->BindResource(TEXT("g_InvVP"), RenderInfo->RenderCameraInfo.ViewProjectionInv);
			VSShader->BindResource(TEXT("g_world"), FMatrix::Identity);
			PSShader->BindResource(TEXT("g_lightDir"), RenderInfo->SceneInfo.MainLightDir.GetSafeNormal());
			TComPtr<ID3D11DeviceContext> dc = YYUTDXManager::GetInstance().GetD3DDC();
			{
				D3D11_MAPPED_SUBRESOURCE VBMapResource;
				HRESULT hr = dc->Map(VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBMapResource);
				FSoftSkinVertex *pLocalVertexArray = (FSoftSkinVertex *)VBMapResource.pData;
				memcpy(pLocalVertexArray, &SkinVertex[0], sizeof(FSoftSkinVertex)*SkinVertex.Num());
				dc->Unmap(VB, 0);
			}

			float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
			dc->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
			dc->RSSetState(m_rs);
			dc->OMSetDepthStencilState(m_ds, 0);
			UINT strid = sizeof(FSoftSkinVertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &(VB), &strid, &offset);
			
		/*	if(IndexData.DataTypeSize == sizeof(uint16))
			{ 
				dc->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
			}
			else if (IndexData.DataTypeSize == sizeof(uint32))*/
			{
				dc->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);
			}
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			VSShader->Update();
			PSShader->Update();
			dc->DrawIndexed(MeshSection.NumTriangles*3, MeshSection.BaseIndex, 0);
		}
	}
}
