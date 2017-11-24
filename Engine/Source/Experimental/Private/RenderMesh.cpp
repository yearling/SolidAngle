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

	FSkeletalMeshResource* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	for (int32 i = 0; i < SkeletalMeshResource->LODModels.Num(); ++i)
	{
		FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[i];
		TArray<FSoftSkinVertex> SkinVertex;
		StaticLodModel.GetVertices(SkinVertex);

	}
}

void FSkeletalMeshRenderHelper::Render()
{

}
