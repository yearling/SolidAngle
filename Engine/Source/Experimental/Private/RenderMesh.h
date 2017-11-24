#pragma once
#include "YYUT.h"
#include "YYUTCamera.h"
#include "StaticMesh.h"
#include "RenderInfo.h"
#include <memory>
#include "SkelImport.h"
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
	FSkeletalMeshRenderHelper(YSkeletalMesh* InSkeletalMesh);
	~FSkeletalMeshRenderHelper();
	void Init();
	void Render();
private:
	YSkeletalMesh* SkeletalMesh;
	TUniquePtr<YVSShader>		m_VSShader;
	TUniquePtr<YPSShader>		m_PSShader;
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
	void							SetMesh(std::unique_ptr<MeshModel> && pMesh) { m_pMesh = std::move(pMesh); }
	void							RegisterSkeletalMesh(YSkeletalMesh* pSkeletalMesh);
	void							CreateMeshResource();
	void							DrawGridAndCoordinates();
	void							DrawSkeletalMeshes();
	void							DrawSkeleton(YSkeletalMesh* pSkeletalMesh);
	void							DrawMesh(YSkeletalMesh* pSkeletalMesh);
private:
	float							m_ScreenWidth;
	float							m_ScreenHeight;
	std::unique_ptr<MeshModel>     m_pMesh;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
private:
	FSkeletalMeshImportData*		m_pSkeletalMeshData;
	TArray<YSkeletalMesh*>			SkeletalMeshes;
	TArray<TUniquePtr<FSkeletalMeshRenderHelper>> SkeletalMeshRenderHeplers;
};
