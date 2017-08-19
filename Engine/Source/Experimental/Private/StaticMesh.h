#pragma once
#include "YYUT.h"
#include "VertexDef.h"
#include <vector>
#include <fbxsdk.h>
#include <memory>
#include "YYUTHelper.h"
#include "Skeleton.h"
#include "IShader.h"
#include <map>



struct MaterialBaseFbx
{
	std::string MaterialName;
	std::string DiffusePath;
};

class StaticMesh
{
public:
	StaticMesh();
	~StaticMesh();
	bool AllocResource(TComPtr<ID3D11Device> device, TComPtr<ID3D11DeviceContext> dc);
	void Clear();
	void Render(TComPtr<ID3D11DeviceContext> dc, TComPtr<ID3D11Buffer> cb);
	void UpdateVertexPosition(FbxMesh*pMesh, FbxVector4* pVertexArray);
	XMMATRIX  MatWorld;
	std::vector<LocalVertex> VertexArray;
	std::vector<int> IndexArray;
	int FaceNum;
	FbxNode* m_pNode;
	std::vector<int> MapTriangleIndexToMaterialIndex;
	std::vector<MaterialBaseFbx> MaterialArray;
	TComPtr<ID3D11Buffer>			m_VB;
	TComPtr<ID3D11Buffer>			m_IB;
	std::string					MeshName;
	YVSShader * m_VS;
	YPSShader * m_PS;
};

class MeshModel
{
public:
	MeshModel();
	void Init(TComPtr<ID3D11Device> device, TComPtr<ID3D11DeviceContext> dc);
	void Tick();
	void DrawNodeRecursive(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition);
	void DrawNode(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition);
	void ComputeShapeDeformation(FbxMesh* pMesh, FbxVector4* pVertexArray);
	void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix);
	void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	void DrawMesh(FbxNode* pNode, FbxAMatrix& pGlobalPosition);
	void Render(TComPtr<ID3D11DeviceContext> dc);
	bool SetCurrentAnimStack(int pIndex);
	FbxAMatrix GetGlobalPosition(FbxNode* pNode);
	void DrawSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition);
	void DrawSkeleton2(Bone* pBone, const XMMATRIX& MatParent);
	void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxVector4* pVertexArray);
	FbxTime mFrameTime, mStart, mStop, mCurrentTime;
	FbxNode* RootNode;
	FbxScene* Scene;
	std::vector<std::unique_ptr<StaticMesh>> MeshArrays;
	Skeleton MainSkeleton;
	FbxArray<FbxString*> mAnimStackNameArray;
	FbxAnimLayer * mCurrentAnimLayer;
	TComPtr<ID3D11Buffer>			m_cbPerMesh;
	TComPtr<ID3D11DeviceContext>    m_dc;
	std::map<FbxNode*, int>       mapFbxNodeToStaticMesh;
	std::unique_ptr<YVSShader>  m_VS;
	std::unique_ptr<YPSShader>  m_PS;
};