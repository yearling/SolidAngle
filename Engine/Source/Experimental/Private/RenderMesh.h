#pragma once
#include "YYUT.h"
#include "YYUTCamera.h"
#include "StaticMesh.h"
#include "RenderInfo.h"
#include <memory>
#include "SkelImport.h"
#include "BonePose.h"
#include "YYUTFont.h"

struct RenderMeshCBuffer
{
	XMMATRIX  m_matView;
	XMMATRIX  m_matProj;
	XMMATRIX  m_matViewProj;
	XMMATRIX  m_matInvViewProj;
	XMFLOAT3  m_lightDir;
};
struct FRenderMeshCBuffer
{
	FMatrix  m_matView;
	FMatrix  m_matProj;
	FMatrix  m_matViewProj;
	FMatrix  m_matInvViewProj;
	FVector  m_lightDir;
};

class FSkeletalMeshRenderHelper
{
public:
	FSkeletalMeshRenderHelper(YSkeletalMesh* InSkeletalMesh, UAnimSequence* InAnimSequence);
	~FSkeletalMeshRenderHelper();
	void Init();
	void SetPos(FCompactPose& CompacePose);
	void Render(TSharedRef<FRenderInfo> RenderInfo);
private:
	YSkeletalMesh* SkeletalMesh;
	UAnimSequence* AnimSequence;
	TUniquePtr<YVSShader>		VSShader;
	TUniquePtr<YPSShader>		PSShader;
	TComPtr<ID3D11Buffer>       VB;
	TComPtr<ID3D11Buffer>       IB;
	TArray<FSoftSkinVertex> SkinVertex;
	FMultiSizeIndexContainerData IndexData;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TArray<FTransform>			CurrentPose;
};
class RenderScene
{
public:
	RenderScene(void);
	~RenderScene(void);
	virtual	void					Init();
	virtual void					Update(float ElpaseTime);
	virtual void					Render(TSharedRef<FRenderInfo> RenderInfo);
	virtual void					AllocResource();
	void							SetScreenWidthHeigth(int width, int height) { m_ScreenWidth = (float)width; m_ScreenHeight = (float)height; }
	void							RegisterSkeletalMesh(YSkeletalMesh* pSkeletalMesh,UAnimSequence* pAnimationSequence);
	void							PlayAnimation(UAnimSequence* pAnimationSequence) { AnimationSequence = pAnimationSequence; }
	void							CreateMeshResource();
	void							DrawGridAndCoordinates();
	void							DrawSkeletalMeshes(TSharedRef<FRenderInfo> RenderInfo);
	void							DrawSkeleton(YSkeletalMesh* pSkeletalMesh);
	void							DrawSkeleton2(YSkeletalMesh* pSkeletalMesh,const FCompactPose& CompacePose);
private:
	float							m_ScreenWidth;
	float							m_ScreenHeight;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
private:
	FSkeletalMeshImportData*		m_pSkeletalMeshData;
	TArray<YSkeletalMesh*>			SkeletalMeshes;
	TArray<UAnimSequence*>			AnimationSequences;
	TArray<TUniquePtr<FSkeletalMeshRenderHelper>> SkeletalMeshRenderHeplers;
	UAnimSequence*					AnimationSequence;
	TUniquePtr<YYUTFont>			ScreenLayout;
};
