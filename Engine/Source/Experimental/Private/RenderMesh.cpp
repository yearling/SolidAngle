#include "YYUT.h"
#include "RenderMesh.h"
#include "YYUTHelper.h"
#include "YYUTDXManager.h"
#include "StaticMesh.h"
#include "Canvas.h"
#include "IShader.h"
#include "YSkeletalMesh.h"
#include "BonePose.h"
#include "Templates/UniquePtr.h"




RenderScene::RenderScene(void):
	m_pSkeletalMeshData(nullptr)
{
	ScreenLayout = MakeUnique<YYUTFont>();
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
	ScreenLayout->Init();
}

void RenderScene::Update(float ElpaseTime)
{
}

void RenderScene::Render(TSharedRef<FRenderInfo> RenderInfo)
{
	TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	static bool bSetAniStack = true;
	if (bSetAniStack)
	{
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

	DrawSkeletalMeshes(RenderInfo);
	DrawGridAndCoordinates();
	for (TUniquePtr<FSkeletalMeshRenderHelper>& RenderHelper : SkeletalMeshRenderHeplers)
	{
		RenderHelper->Render(RenderInfo);
	}
	GCanvas->Render(RenderInfo);
	ScreenLayout->BeginText();
	ScreenLayout->DrawTextLine(FString::Printf(TEXT("FPS: %f"), RenderInfo->FPS));
}


void RenderScene::AllocResource()
{
	for (TUniquePtr<FSkeletalMeshRenderHelper>& RenderHelper : SkeletalMeshRenderHeplers)
	{
		RenderHelper->Init();
	}

	for (UStaticMesh* pStaticMesh : StaticMeshes)
	{
		pStaticMesh->InitResource();
	}
}

void RenderScene::CreateMeshResource()
{
	//m_pMesh->Init();
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

void RenderScene::DrawSkeletalMeshes(TSharedRef<FRenderInfo> RenderInfo)
{
	
	for(int32 i=0;i<SkeletalMeshes.Num();++i)
	{
		YSkeletalMesh* pMesh = SkeletalMeshes[i];
		//DrawSkeleton(pMesh);
		UAnimSequence* pAnimSequence = AnimationSequences[i];
		TArray<FBoneIndexType> RequiredBone = pMesh->GetResourceForRendering()->LODModels[0].RequiredBones;
		FBoneContainer BoneContainer;
		BoneContainer.AssetSkeletalMesh = pMesh;
		BoneContainer.InitializeTo(RequiredBone);
		FCompactPose CompactPose;
		FMemMark Mark(FMemStack::Get());
		CompactPose.SetBoneContainer(&BoneContainer);
		CompactPose.ResetToRefPose();
		FBlendedCurve BlendCurve;
		static float AnimationTime = 0.0;
		AnimationTime += RenderInfo->TickTime;
		AnimationTime = FMath::Fmod(AnimationTime, pAnimSequence->GetPlayLength());
		pAnimSequence->GetAnimationPose(CompactPose, BlendCurve, FAnimExtractContext(AnimationTime, pAnimSequence->bEnableRootMotion));
		DrawSkeleton2(pMesh,CompactPose);
		SkeletalMeshRenderHeplers[i]->SetPos(CompactPose);
	}
}
template<class TAlloc>
FMatrix GetParentMatrix(const TArray<FMeshBoneInfo>& MeshBoneInfoes, const TArray<FTransform,TAlloc> & BonePoses, int32 iIndex)
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


void RenderScene::DrawSkeleton2(YSkeletalMesh* pSkeletalMesh,const FCompactPose& CompacePose)
{
	if (!pSkeletalMesh)
		return;
	const FReferenceSkeleton& Skeleton = pSkeletalMesh->RefSkeleton;
	const TArray<FTransform,FAnimStackAllocator> & BonePoses = CompacePose.GetBones();

	TArray<FMatrix> CurrentBonePos;
	CurrentBonePos.Empty(pSkeletalMesh->RefSkeleton.GetRawBoneNum());
	const TArray<FMeshBoneInfo> &MeshBoneInfos = pSkeletalMesh->RefSkeleton.GetRefBoneInfo();
	for (int32 BoneID = 0; BoneID < pSkeletalMesh->RefSkeleton.GetRawBoneNum(); ++BoneID)
	{
		if (BoneID == 0)
		{
			check(MeshBoneInfos[BoneID].ParentIndex == INDEX_NONE);
			CurrentBonePos.Add(CompacePose[(FCompactPoseBoneIndex)BoneID].ToMatrixWithScale());
		}
		else
		{
			int32 ParentIndex = MeshBoneInfos[BoneID].ParentIndex;
			CurrentBonePos.Add(CompacePose[(FCompactPoseBoneIndex)BoneID].ToMatrixWithScale()*CurrentBonePos[ParentIndex]);
		}
	}


	for (int32 i = 0; i < MeshBoneInfos.Num(); ++i)
	{
		const FMeshBoneInfo & BoneInfo = MeshBoneInfos[i];
		if (BoneInfo.ParentIndex != INDEX_NONE)
		{
			//FMatrix ParentMatrix = GetParentMatrix(MeshBoneInfos, BonePoses, i);
			FMatrix ParentMatrix = CurrentBonePos[BoneInfo.ParentIndex];
			FMatrix LocalMatrix = CurrentBonePos[i];
			FVector ParentPos = ParentMatrix.TransformPosition(FVector(0, 0, 0));
			FVector LocalPos = LocalMatrix.TransformPosition(FVector(0, 0, 0));
			GCanvas->DrawLine(ParentPos, LocalPos, FLinearColor(1, 1, 0, 1));
			GCanvas->DrawBall(LocalPos, FLinearColor(1, 0, 0, 1));
		}
		else
		{
			GCanvas->DrawBall(BonePoses[i].GetTranslation(), FLinearColor(0, 1, 0, 1));
		}
	}
}

void RenderScene::RegisterSkeletalMesh(YSkeletalMesh* pSkeletalMesh, UAnimSequence* pAnimationSequence)
{
	SkeletalMeshes.Add(pSkeletalMesh);
	AnimationSequences.Add(pAnimationSequence);
	SkeletalMeshRenderHeplers.Emplace(MakeUnique<FSkeletalMeshRenderHelper>(pSkeletalMesh,pAnimationSequence));
}

void RenderScene::RegisterStaticMesh(UStaticMesh* pStaticMesh)
{
	StaticMeshes.Add(pStaticMesh);
}

FSkeletalMeshRenderHelper::FSkeletalMeshRenderHelper(YSkeletalMesh* InSkeletalMesh, UAnimSequence* InAnimSequence)
	:SkeletalMesh(InSkeletalMesh)
	,AnimSequence(InAnimSequence)
	, IsCPURender(false)
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
		{ "ATTRIBUTE",  8, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  9, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  10, DXGI_FORMAT_R8G8B8A8_UINT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  11, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 68, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  12, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	VSShader->BindInputLayout(Layout);
	if (!VSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\SkeletalMesh.hlsl"), TEXT("VSMain")))
	{
		check(0);
	}
	VSShaderGPU = MakeUnique<YVSShader>();
	VSShaderGPU->BindInputLayout(Layout);
	if (!VSShaderGPU->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\GPUSKin.hlsl"), TEXT("VSMain")))
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

		CreateVertexBuffer((UINT)SkinVertex.Num() * sizeof(FSoftSkinVertex), &SkinVertex[0], VBGPU);
		TArray<TComPtr<ID3D11Buffer>> BoneMatrix;
		TArray<TComPtr<ID3D11ShaderResourceView>> BoneMatrixBufferSRV;
		for (int32 SectionIndex = 0; SectionIndex < StaticLodModel.Sections.Num(); ++SectionIndex)
		{
			TComPtr<ID3D11Buffer> BoneMatrixBuffer;
			CreateTBuffer<true, false, true, false>(StaticLodModel.Sections[SectionIndex].BoneMap.Num() * 3* 16, BoneMatrixBuffer);
			TComPtr<ID3D11ShaderResourceView> SRV;
			CreateSRVForTBuffer(DXGI_FORMAT_R32G32B32A32_FLOAT,StaticLodModel.Sections[SectionIndex].BoneMap.Num() * 3, BoneMatrixBuffer,SRV);
			BoneMatrix.Emplace(MoveTemp(BoneMatrixBuffer));
			BoneMatrixBufferSRV.Emplace(MoveTemp(SRV));
		}
		FinalBoneMatrix.Emplace(MoveTemp(BoneMatrix));
		FinalBoneMatrixSRV.Emplace(MoveTemp(BoneMatrixBufferSRV));
	}
}
MS_ALIGN(16) struct YSkinMatrix3x4
{
	float M[3][4];
};
void FSkeletalMeshRenderHelper::SetPos(FCompactPose& CompacePose)
{
	CompacePose.CopyBonesTo(CurrentPose);
	//CurrentPose = SkeletalMesh->RefSkeleton.GetRawRefBonePose();
	TArray<FMatrix> CurrentBonePose;
	CurrentBonePose.Empty(SkeletalMesh->RefSkeleton.GetRawBoneNum());
	const TArray<FMeshBoneInfo> &MeshBoneInfos = SkeletalMesh->RefSkeleton.GetRefBoneInfo();
	for (int32 BoneID = 0; BoneID < SkeletalMesh->RefSkeleton.GetRawBoneNum(); ++BoneID)
	{
		if (BoneID==0)
		{
			check(MeshBoneInfos[BoneID].ParentIndex == INDEX_NONE);
			CurrentBonePose.Add(CurrentPose[BoneID].ToMatrixWithScale());
		}
		else
		{
			int32 ParentIndex = MeshBoneInfos[BoneID].ParentIndex;
			CurrentBonePose.Add(CurrentPose[BoneID].ToMatrixWithScale()*CurrentBonePose[ParentIndex]);
		}
	}

	FSkeletalMeshResource* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	if (IsCPURender)
	{
		TArray<FSoftSkinVertex> TransformSkinVertex;
		for (int32 LodIndex = 0; LodIndex < SkeletalMeshResource->LODModels.Num(); ++LodIndex)
		{
			FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[LodIndex];
			TransformSkinVertex.Empty(SkinVertex.Num());
			const float normalUint8 = 1 / 255.0f;
			for (int32 SectionIndex = 0; SectionIndex < StaticLodModel.Sections.Num(); SectionIndex++)
			{
				FSkelMeshSection& Section = StaticLodModel.Sections[SectionIndex];
				for (int32 i = 0; i < Section.SoftVertices.Num(); i++)
				{
					FSoftSkinVertex* SoftVert = &Section.SoftVertices[i];
					FSoftSkinVertex DesVertex = *SoftVert;
					FMatrix TranformMatrix;
					memset(&TranformMatrix, 0, sizeof(FMatrix));
					for (int32 j = 0; j < MAX_TOTAL_INFLUENCES; j++)
					{
						if (SoftVert->InfluenceWeights[j] > 0)
						{

							int32 BoneIndex = Section.BoneMap[SoftVert->InfluenceBones[j]];
							TranformMatrix += SkeletalMesh->RefBasesInvMatrix[BoneIndex] * CurrentBonePose[BoneIndex] * (((float)SoftVert->InfluenceWeights[j])* normalUint8);
						}
					}
					DesVertex.Position = FTransform(TranformMatrix).TransformPosition(SoftVert->Position);
					TransformSkinVertex.Emplace(DesVertex);
				}
			}
			check(TransformSkinVertex.Num() == SkinVertex.Num());
		}
		D3D11_MAPPED_SUBRESOURCE MapResource;
		TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
		HRESULT hr = DeviceContext->Map(VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
		{
			memcpy(MapResource.pData, &TransformSkinVertex[0], sizeof(FSoftSkinVertex)* TransformSkinVertex.Num());
		}
		DeviceContext->Unmap(VB, 0);
	}
	else
	{
		for (int32 LodIndex = 0; LodIndex < SkeletalMeshResource->LODModels.Num(); ++LodIndex)
		{
			FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[LodIndex];
			for (int32 SectionIndex = 0; SectionIndex < StaticLodModel.Sections.Num(); SectionIndex++)
			{
				FSkelMeshSection &CurrentSection = StaticLodModel.Sections[SectionIndex];
				TArray<YSkinMatrix3x4> RefToLocalMatrixRows;
				RefToLocalMatrixRows.AddUninitialized(CurrentSection.BoneMap.Num());
				for (int32 iSectionBone = 0; iSectionBone < CurrentSection.BoneMap.Num(); ++iSectionBone)
				{
					int32 BoneIndex = CurrentSection.BoneMap[iSectionBone];
					FMatrix BoneMatrix = (SkeletalMesh->RefBasesInvMatrix[BoneIndex] * CurrentBonePose[BoneIndex]);
					BoneMatrix.To3x4MatrixTranspose((float*)RefToLocalMatrixRows[iSectionBone].M);
				}
				TComPtr<ID3D11Buffer>& CurrentBoneBuffer = FinalBoneMatrix[LodIndex][SectionIndex];
				TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
				D3D11_MAPPED_SUBRESOURCE MapResource;
				HRESULT hr = DeviceContext->Map(CurrentBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
				if(SUCCEEDED(hr))
				{
					memcpy(MapResource.pData, &RefToLocalMatrixRows[0], sizeof(YSkinMatrix3x4)* RefToLocalMatrixRows.Num());
				}
				DeviceContext->Unmap(CurrentBoneBuffer, 0);
			}
		}
	}
}

void FSkeletalMeshRenderHelper::Render(TSharedRef<FRenderInfo> RenderInfo)
{
	
	FSkeletalMeshResource* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	for (int32 i = 0; i < SkeletalMeshResource->LODModels.Num(); ++i)
	{
		FStaticLODModel& StaticLodModel = SkeletalMeshResource->LODModels[i];
		for(int32 MeshSectionID = 0;MeshSectionID<StaticLodModel.Sections.Num();++MeshSectionID)
		{
			FSkelMeshSection& MeshSection = StaticLodModel.Sections[MeshSectionID];
			if (MeshSection.NumTriangles == 0)
				return;
			
			TComPtr<ID3D11DeviceContext> dc = YYUTDXManager::GetInstance().GetD3DDC();
			float BlendColor[4] = { 1.0f,1.0f,1.0f,1.0f };
			dc->OMSetBlendState(m_bs, BlendColor, 0xffffffff);
			dc->RSSetState(m_rs);
			dc->OMSetDepthStencilState(m_ds, 0);
			UINT strid = sizeof(FSoftSkinVertex);
			UINT offset = 0;
			if (IsCPURender)
			{
				VSShader->BindResource(TEXT("g_view"), RenderInfo->RenderCameraInfo.View);
				VSShader->BindResource(TEXT("g_projection"), RenderInfo->RenderCameraInfo.Projection);
				VSShader->BindResource(TEXT("g_VP"), RenderInfo->RenderCameraInfo.ViewProjection);
				VSShader->BindResource(TEXT("g_InvVP"), RenderInfo->RenderCameraInfo.ViewProjectionInv);
				VSShader->BindResource(TEXT("g_world"), FMatrix::Identity);
				VSShader->Update();
				dc->IASetVertexBuffers(0, 1, &(VB), &strid, &offset);
			}
			else
			{
				VSShaderGPU->BindResource(TEXT("g_view"), RenderInfo->RenderCameraInfo.View);
				VSShaderGPU->BindResource(TEXT("g_projection"), RenderInfo->RenderCameraInfo.Projection);
				VSShaderGPU->BindResource(TEXT("g_VP"), RenderInfo->RenderCameraInfo.ViewProjection);
				VSShaderGPU->BindResource(TEXT("g_InvVP"), RenderInfo->RenderCameraInfo.ViewProjectionInv);
				VSShaderGPU->BindResource(TEXT("g_world"), FMatrix::Identity);
				VSShaderGPU->BindSRV(TEXT("BoneMatrices"), FinalBoneMatrixSRV[i][MeshSectionID]);
				VSShaderGPU->Update();
				dc->IASetVertexBuffers(0, 1, &(VBGPU), &strid, &offset);
			}
			
			PSShader->BindResource(TEXT("g_lightDir"), RenderInfo->SceneInfo.MainLightDir.GetSafeNormal());
			PSShader->Update();
		/*	if(IndexData.DataTypeSize == sizeof(uint16))
			{ 
				dc->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
			}
			else if (IndexData.DataTypeSize == sizeof(uint32))*/
			{
				dc->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);
			}
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			dc->DrawIndexed(MeshSection.NumTriangles*3, MeshSection.BaseIndex, 0);
		}
	}
}

FStaticMeshRenderHelper::FStaticMeshRenderHelper(UStaticMesh* InMesh)
:StaticMesh(InMesh)
{

}

FStaticMeshRenderHelper::~FStaticMeshRenderHelper()
{

}

void FStaticMeshRenderHelper::Init()
{
	if (!StaticMesh)
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
		{ "ATTRIBUTE",  8, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  9, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  10, DXGI_FORMAT_R8G8B8A8_UINT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  11, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 68, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ATTRIBUTE",  12, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	VSShader->BindInputLayout(Layout);
	if (!VSShader->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\SkeletalMesh.hlsl"), TEXT("VSMain")))
	{
		check(0);
	}
	VSShaderGPU = MakeUnique<YVSShader>();
	VSShaderGPU->BindInputLayout(Layout);
	if (!VSShaderGPU->CreateShader(TEXT("..\\..\\Source\\Experimental\\Private\\GPUSKin.hlsl"), TEXT("VSMain")))
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
	}
}

void FStaticMeshRenderHelper::Render(TSharedRef<FRenderInfo> RenderInfo)
{

}
