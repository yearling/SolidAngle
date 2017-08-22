#pragma once
#include "YYUT.h"
#include "VertexDef.h"
#include <fbxsdk.h>
#include "YYUTHelper.h"
#include "Skeleton.h"
#include "IShader.h"
#include "RenderInfo.h"




struct MaterialBaseFbx
{
	FString MaterialName;
	FString DiffusePath;
};
class MeshModel;
class StaticMesh
{
public:
	StaticMesh();
	~StaticMesh();
	bool AllocResource();
	void Clear();
	void Render(TComPtr<ID3D11Buffer> cb);
	void UpdateVertexPosition(FbxMesh*pMesh, FbxVector4* pVertexArray);
	FMatrix  MatWorld;
	TArray<LocalVertex> VertexArray;
	TArray<int> IndexArray;
	int FaceNum;
	FbxNode* m_pNode;
	TArray<int> MapTriangleIndexToMaterialIndex;
	TArray<MaterialBaseFbx> MaterialArray;
	TComPtr<ID3D11Buffer>			m_VB;
	TComPtr<ID3D11Buffer>			m_IB;
	FString					MeshName;
	TUniquePtr<YVSShader>		m_VSShader;
	TUniquePtr<YPSShader>		m_PSShader;
	MeshModel*    pMeshModel;
};

class MeshModel
{
public:
	MeshModel();
	void Init();
	void Tick();
	void DrawNodeRecursive(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition);
	void DrawNode(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition);
	void ComputeShapeDeformation(FbxMesh* pMesh, FbxVector4* pVertexArray);
	void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix);
	void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	void DrawMesh(FbxNode* pNode, FbxAMatrix& pGlobalPosition);
	void Render(TSharedRef<FRenderInfo> RenderInfo);
	bool SetCurrentAnimStack(int pIndex);
	FbxAMatrix GetGlobalPosition(FbxNode* pNode);
	void DrawSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition);
	void DrawSkeleton2(Bone* pBone, const XMMATRIX& MatParent);
	void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	FbxTime mFrameTime, mStart, mStop, mCurrentTime;
	FbxNode* RootNode;
	FbxScene* Scene;
	TArray<TUniquePtr<StaticMesh>> MeshArrays;
	Skeleton MainSkeleton;
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxAnimLayer * mCurrentAnimLayer;
	TComPtr<ID3D11Buffer>			m_cbPerMesh;
	TMap<FbxNode*, int>       mapFbxNodeToStaticMesh;
	TUniquePtr<YVSShader>  m_VS;
	TUniquePtr<YPSShader>  m_PS;
	FRenderInfo					m_RenderInfo;
};