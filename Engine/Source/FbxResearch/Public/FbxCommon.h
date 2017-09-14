#pragma once
#include "Core.h"
#include <fbxsdk.h>
DECLARE_LOG_CATEGORY_EXTERN(FbxSDKLog, Log, All);

bool InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
FbxManager* InitFBXSDK();
FbxScene* CreateScene(FbxManager* pManager);
void DestroySdkObjects(FbxManager* pManager);
void CreateAndFillIOSettings(FbxManager* pManager);
FbxAnimLayer*  CreateAnimation(FbxScene* pScene);
FbxFileTexture*  CreateTexture(FbxScene* pScene);
FbxSurfacePhong* CreateMaterial(FbxScene* pScene);
void AddMaterials(FbxMesh* pMesh, FbxSurfacePhong* pMaterial);
FbxMesh* CreateCubeMeshPolygon(FbxScene* pScene, char* pName);
FbxMesh* CreateCubeMeshTriangle(FbxScene* pScene, char* pName);
FbxMesh* CreatecuboidMeshTriangle(FbxScene* pScene, char* pName);
bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);