#include "fbxsdk.h"
#include "FbxCommon.h"
DEFINE_LOG_CATEGORY(FbxSDKLog);
bool InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		UE_LOG(FbxSDKLog, Error, TEXT("Error: Unable to create FBX Manager!\n"));
		return false;
	}
	else
	{
		UE_LOG(FbxSDKLog, Log, TEXT("Autodesk FBX SDK version %ls"), ANSI_TO_TCHAR(pManager->GetVersion()));
	}
	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		UE_LOG(FbxSDKLog,Error,TEXT("Error: Unable to create FBX scene!\n"));
		return false;
	}
	return true;
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

void CreateAndFillIOSettings(FbxManager* pManager)
{

}
