#pragma once
#include "Core.h"
#include <fbxsdk.h>
#include "fbxsdk\fileio\fbxiosettings.h"
#include "YMaterial.h"
#include "YStaticMesh.h"
#include "YRawMesh.h"
#include "FbxImporter.h"
#include "YFbxImportOptions.h"
class UStaticMesh;

DECLARE_LOG_CATEGORY_EXTERN(LogYFbxConverter, Log, All);

class YFbxConverter
{
public:
	YFbxConverter();
	~YFbxConverter();
	bool Init(const FString &Filename);
	TRefCountPtr<YStaticMesh>  Import(TUniquePtr<YFBXImportOptions> ImportOptionsIn);
	bool Save(const FString& DesDir);
public:
	struct YFbxMaterial
	{
		FbxSurfaceMaterial* FbxMaterial;
		YMaterialInterface* Material;
		FString GetName() const { return FbxMaterial ? ANSI_TO_TCHAR(FbxMaterial->GetName()) : TEXT("None"); }
	};

protected:
	TRefCountPtr<YStaticMesh> ImportStaticMeshAsSingle( TArray<FbxNode*>& MeshNodeArray, const FName InName, TRefCountPtr<YStaticMesh>InStaticMesh, int LODIndex, void *ExistMeshDataPtr);
	bool BuildStaticMeshFromGeometry(FbxNode* Node, TRefCountPtr<YStaticMesh> StaticMesh, TArray<YFbxMaterial>& MeshMaterials, int32 LODIndex, YRawMesh& RawMesh, EYVertexColorImportOption::Type VertexColorImportOption, const FColor& VertexOverrideColor);
	void ConvertScene();

	ANSICHAR* MakeName(const ANSICHAR* name);
	FString MakeString(const ANSICHAR* Name);
	int32 GetFbxMeshCount(FbxNode* Node, bool bCountLODs, int32& OutNumLODGroups);
	void FillFbxMeshArray(FbxNode* Node, TArray<FbxNode*>& outMeshArray);

	int32 CreateNodeMaterials(FbxNode* FbxNode, TArray<YMaterialInterface*>& outMaterials, TArray<FString>& UVSets);
	void CreateMaterial(FbxSurfaceMaterial& FbxMaterial, TArray<YMaterialInterface*>& OutMaterials, TArray<FString>& UVSets);
	bool CreateMaterialProperty(FbxSurfaceMaterial& FbxMaterial,YMaterialInterface* UnrealMaterial,const char* MaterialProperty,bool bSetupAsNormalMap,TArray<FString>& UVSet);
	YTexture* ImportTexture(FbxFileTexture* FbxTexture, bool bSetupAsNormalMap);
	FbxAMatrix ComputeTotalMatrix(FbxNode* Node);
	/**
	* Check if there are negative scale in the transform matrix and its number is odd.
	 * @return bool True if there are negative scale and its number is 1 or 3.
	 */
	bool IsOddNegativeScale(FbxAMatrix& TotalMatrix);
	void AlignMaterialSection(TRefCountPtr<YStaticMesh>& InMesh);
protected:
	bool GetSceneInfo(const FString& FileName, YFbxSceneInfo& SceneInfo);
	// 找到第一个LOD Group Node
	FbxNode* RecursiveFindParentLodGroup(FbxNode* pParentNode);
	// 找到第LodIndex级的mesh
	FbxNode* FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind = nullptr);
	FbxNode *RecursiveGetFirstMeshNode(FbxNode* Node, FbxNode* NodeToFind = nullptr);
	void TraverseHierarchyNodeRecursively(YFbxSceneInfo& SceneInfo, FbxNode *ParentNode, YFbxNodeInfo &ParentInfo);
private:
	FbxManager* SdkManager = nullptr;
	TUniquePtr<FbxGeometryConverter> GeometryConverter;
	FbxImporter* Importer = nullptr;
	FbxScene* Scene;
	FString FileToImport; //import path
	FString FileBasePath;
	bool bHasSkin = false;
	bool bHasStaticMesh = false;
	bool bHasAnimation = false;
	TUniquePtr<YFBXImportOptions> ImportOptions;
};

