#pragma once
#include "Core.h"
#include <fbxsdk.h>
#include "fbxsdk\fileio\fbxiosettings.h"
DECLARE_LOG_CATEGORY_EXTERN(LogYFbxConverter, Log, All);



struct YFbxMeshInfo
{
	FString Name;
	uint64 UniqueId;
	int32 FaceNum;
	int32 VertexNum;
	bool bTriangulated;
	int32 MaterialNum;
	bool bIsSkelMesh;
	FString SkeletonRoot;
	int32 SkeletonElemNum;
	FString LODGroup;
	int32 LODLevel;
	int32 MorphNum;
};

//Node use to store the scene hierarchy transform will be relative to the parent
struct YFbxNodeInfo
{
	const char* ObjectName;
	uint64 UniqueId;
	FbxAMatrix Transform;
	FbxVector4 RotationPivot;
	FbxVector4 ScalePivot;

	const char* AttributeName;
	uint64 AttributeUniqueId;
	const char* AttributeType;

	const char* ParentName;
	uint64 ParentUniqueId;
};


struct YFbxSceneInfo
{
	// data for static mesh
	int32 NonSkinnedMeshNum;

	//data for skeletal mesh
	int32 SkinnedMeshNum;

	// common data
	int32 TotalGeometryNum;
	int32 TotalMaterialNum;
	int32 TotalTextureNum;

	TArray<YFbxMeshInfo> MeshInfo;
	TArray<YFbxNodeInfo> HierarchyInfo;

	/* true if it has animation */
	bool bHasAnimation;
	double FrameRate;
	double TotalTime;

	void Reset()
	{
		NonSkinnedMeshNum = 0;
		SkinnedMeshNum = 0;
		TotalGeometryNum = 0;
		TotalMaterialNum = 0;
		TotalTextureNum = 0;
		MeshInfo.Empty();
		HierarchyInfo.Empty();
		bHasAnimation = false;
		FrameRate = 0.0;
		TotalTime = 0.0;
	}
};


class YFbxConverter
{
public:
	YFbxConverter();
	~YFbxConverter();
	bool Init(const FString &Filename);
	bool Import();
protected:
	bool GetSceneInfo(const FString& FileName, YFbxSceneInfo& SceneInfo);
	// 找到第一个LOD Group Node
	FbxNode* RecursiveFindParentLodGroup(FbxNode* pParentNode);
	// 找到第LodIndex级的mesh
	FbxNode* FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind = nullptr);
	FbxNode *RecursiveGetFirstMeshNode(FbxNode* Node, FbxNode* NodeToFind = nullptr);
	void TraverseHierarchyNodeRecursively(YFbxSceneInfo& SceneInfo, FbxNode *ParentNode, YFbxNodeInfo &ParentInfo);
	ANSICHAR* MakeName(const ANSICHAR* name);
	FString MakeString(const ANSICHAR* Name);
	bool ConvertScene();
private:
	FbxManager* SdkManager = nullptr;
	FbxGeometryConverter* GeometryConverter = nullptr;
	FbxImporter* Importer = nullptr;
	FbxScene* Scene;
	FString FileToImport;
};

