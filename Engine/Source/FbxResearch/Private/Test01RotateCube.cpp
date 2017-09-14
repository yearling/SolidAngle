#include "TestCategory.h"
DECLARE_LOG_CATEGORY_EXTERN(FbxRotateCubeLog, Log, All);
DEFINE_LOG_CATEGORY(FbxRotateCubeLog);

void AnimateCubeRotateAxis(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	FbxAnimCurve* lCurve = NULL;
	FbxTime lTime;
	int lKeyIndex = 0;

	pCube->LclRotation.GetCurveNode(pAnimLayer, true);
	if (Axis == 0)
	{
		lCurve = pCube->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	}
	else if (Axis == 1)
	{
		lCurve = pCube->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	}
	else if (Axis == 2)
	{
		lCurve = pCube->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	}

	if (lCurve)
	{
		lCurve->KeyModifyBegin();

		lTime.SetSecondDouble(0.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 0.0, FbxAnimCurveDef::eInterpolationLinear);

		lTime.SetSecondDouble(20.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, -3500, FbxAnimCurveDef::eInterpolationLinear);
		lCurve->KeyModifyEnd();
	}
}


void TestRotateCube()
{
	UE_LOG(FbxRotateCubeLog, Log, TEXT("Test Rotate Cube"));
	FbxManager* pManager = InitFBXSDK();
	check(pManager);
	CreateAndFillIOSettings(pManager);
	FbxScene* pScene = CreateScene(pManager);
	check(pScene);
	FbxAnimLayer* pAnimLayer = CreateAnimation(pScene);
	FbxMesh* pCubeMesh = CreateCubeMeshTriangle(pScene, "CubeMesh");
	pCubeMesh->GetNode()->LclTranslation = FbxVector4(0, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeMesh->GetNode());

	FbxMesh* pCubeNodeRotateX = CreateCubeMeshTriangle(pScene, "CubeMeshRotateX");
	AnimateCubeRotateAxis(pCubeNodeRotateX->GetNode(), pAnimLayer, 0);
	pCubeNodeRotateX->GetNode()->LclTranslation = FbxVector4(150, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeNodeRotateX->GetNode());

	FbxMesh* pCubeNodeRotateY = CreateCubeMeshTriangle(pScene, "CubeMeshRotateY");
	AnimateCubeRotateAxis(pCubeNodeRotateY->GetNode(), pAnimLayer, 1);
	pCubeNodeRotateY->GetNode()->LclTranslation = FbxVector4(300, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeNodeRotateY->GetNode());

	FbxMesh* pCubeNodeRotateZ = CreateCubeMeshTriangle(pScene, "CubeMeshRotateZ");
	AnimateCubeRotateAxis(pCubeNodeRotateZ->GetNode(), pAnimLayer, 2);
	pCubeNodeRotateZ->GetNode()->LclTranslation = FbxVector4(450, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeNodeRotateZ->GetNode());

	SaveScene(pManager, pScene, "TestRotateCube.fbx");
	DestroySdkObjects(pManager);
}
