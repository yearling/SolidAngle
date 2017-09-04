#include "fbxsdk.h"
#include "FbxCommon.h"
DEFINE_LOG_CATEGORY(FbxSDKLog);
bool InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		UE_LOG(FbxSDKLog, Warning, TEXT("Create AbonusGameModeBase Successful"));
		return false;
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());
	UE_LOG(FbxSDKLog, Log, TEXT("Log system Successful"));
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
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
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
