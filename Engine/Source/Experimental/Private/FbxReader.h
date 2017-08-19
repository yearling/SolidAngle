#pragma once
#include "YYUT.h"
#include <fbxsdk.h>
#include <string>
#include <memory>
#include <vector>
#include "StaticMesh.h"
#include "FbxUtility.h"

class FBXReader
{
public:
	FBXReader();
	~FBXReader();
	void InitialFBXSDK();
	bool ImportFile(std::string FileName);
	std::unique_ptr<MeshModel> GenerateModelMesh();

	FbxManager*   FBXSdkManager;
	FbxScene*	  MainScene;
	std::vector<std::unique_ptr<StaticMesh>>	StaticMeshArray;
	void ConvertScene();
	std::pair<bool,std::string> LoadNodes();
	std::pair<bool,std::string> LoadNodesRecursive(FbxNode* pNode);
	std::pair<bool,std::string> LoadMeshInfo(FbxMesh *pMesh, StaticMesh* pStaticMesh);
	std::pair<bool, std::string> LoadSkeletonInfo(FbxSkeleton* pSkeleton);
	void ReadVertex(FbxVector4* pControlPointsArray, int CtrlPointIndex, XMFLOAT3* pVertex);
	void ReadColor(FbxMesh* pMesh, int CtrlPointIndex, int vertexCounter, XMFLOAT4* pColor);
	void ReadUV(FbxMesh* pMesh, int CtrlPointIndex, int TextureUVIndex, int UVLayer, XMFLOAT2* pUV);
	void ReadNormal(FbxMesh* pMesh, int CtrlPointIndex, int VertexCounter, XMFLOAT3* pNormal);
	void ReadTangent(FbxMesh * pMesh, int CtrlPointIndex, int VertexCounter, XMFLOAT3 * pTangent);
	void ConnectMaterialToMesh(FbxMesh* pMesh, StaticMesh* pStaticMesh);
	void ReadMaterial(FbxMesh* pMesh, StaticMesh* pStaticMesh);
	MaterialBaseFbx LoadMaterialAttribute(FbxSurfaceMaterial* pSurfaceMaterial);
	void BuildSkeletonRelationship();
	void BuildSkeletonRecursive(FbxNode * pSkeleton);
	FbxSkeleton* GetParentSkeleton(FbxSkeleton* pSkeleton);
	std::vector<FbxVector4>		MeshVertex;
	std::vector<int>			MeshIndex;
	MeshModel*                  pMeshModel;
};
