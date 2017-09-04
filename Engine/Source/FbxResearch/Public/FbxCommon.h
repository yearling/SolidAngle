#pragma once
#include "Core.h"
#include <fbxsdk.h>
DECLARE_LOG_CATEGORY_EXTERN(FbxSDKLog, Log, All);
bool InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
void CreateAndFillIOSettings(FbxManager* pManager);
bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);