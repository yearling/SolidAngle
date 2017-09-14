#include "TestCategory.h"
DECLARE_LOG_CATEGORY_EXTERN(FbxCreateCubeLog, Log, All);
DEFINE_LOG_CATEGORY(FbxCreateCubeLog);
void TestCreateCube()
{
	UE_LOG(FbxCreateCubeLog, Log, TEXT("Test Create Cube"));
	FbxManager* pManager = InitFBXSDK();
	check(pManager);
	CreateAndFillIOSettings(pManager);
	FbxScene* pScene = CreateScene(pManager);
	check(pScene);
	FbxMesh* pCubeMesh = CreateCubeMeshTriangle(pScene, "CubeMesh");
	pCubeMesh->GetNode()->LclTranslation = FbxVector4(0, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeMesh->GetNode());
	SaveScene(pManager, pScene, "TestCrateCube.fbx");
	DestroySdkObjects(pManager);
}