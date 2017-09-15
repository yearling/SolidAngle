#include "TestCategory.h"

DECLARE_LOG_CATEGORY_EXTERN(FbxPivotLog, Log, All);
DEFINE_LOG_CATEGORY(FbxPivotLog);
static void AnimateCubeRotateAxis(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
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

		lTime.SetSecondDouble(12.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 360, FbxAnimCurveDef::eInterpolationLinear);

		lCurve->KeyModifyEnd();
	}
}
void AnimateCubeRotateAxisWithPivot(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	AnimateCubeRotateAxis(pCube, pAnimLayer, Axis);
	pCube->SetRotationPivot(FbxNode::eSourcePivot,FbxVector4(0, 0, 50));
}
void AnimateCubeRotateAxisWithOffset(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	AnimateCubeRotateAxis(pCube, pAnimLayer, Axis);
	pCube->SetRotationPivot(FbxNode::eSourcePivot, FbxVector4(0, 0, 50));
	pCube->SetRotationOffset(FbxNode::eSourcePivot, FbxVector4(-70, 0, 0));
}

void AnimateCubePreRotate(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	AnimateCubeRotateAxis(pCube, pAnimLayer, Axis);
	pCube->SetPreRotation(FbxNode::eSourcePivot, FbxVector4(0,90,0));
}

void AnimateCubePostRotate(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	AnimateCubeRotateAxis(pCube, pAnimLayer, Axis);
	pCube->SetPostRotation(FbxNode::eSourcePivot, FbxVector4(0, 90, 0));
}
void AnimateCubeScalingAllAxis(FbxNode* pCube, FbxAnimLayer* pAnimLayer)
{
	FbxAnimCurve* pCurve[3];
	FbxTime lTime;
	int lKeyIndex = 0;

	pCube->LclScaling.GetCurveNode(pAnimLayer, true);
	pCurve[0] = pCube->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	pCurve[1]= pCube->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	pCurve[2] = pCube->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	for(int32 i=0;i<3;++i)
	{
		FbxAnimCurve* lCurve = pCurve[i];
		lCurve->KeyModifyBegin();

		lTime.SetSecondDouble(0.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 1.0, FbxAnimCurveDef::eInterpolationLinear);

		lTime.SetSecondDouble(100.0/30.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime,3.0f, FbxAnimCurveDef::eInterpolationLinear);
		lCurve->KeyModifyEnd();
	}
}

void AnimateCubeScalingWithScalingPivot(FbxNode* pCube, FbxAnimLayer* pAnimLayer)
{
	AnimateCubeScalingAllAxis(pCube, pAnimLayer);
	pCube->SetScalingPivot(FbxNode::eSourcePivot,FbxVector4(0, 0, 50));
}

void AnimateCubeScalingWithScalingPivotAndOffset(FbxNode* pCube, FbxAnimLayer* pAnimLayer)
{
	AnimateCubeScalingAllAxis(pCube, pAnimLayer);
	pCube->SetScalingPivot(FbxNode::eSourcePivot, FbxVector4(0, 0, 50));
	pCube->SetScalingOffset(FbxNode::eSourcePivot, FbxVector4(-210, 0, 0));
}

void AnimateCubeTranslate(FbxNode* pCube, FbxAnimLayer* pAnimLayer)
{
	FbxAnimCurve* pCurve[3];
	FbxTime lTime;
	int lKeyIndex = 0;

	pCube->LclTranslation.GetCurveNode(pAnimLayer, true);
	pCurve[0] = pCube->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	pCurve[1] = pCube->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	pCurve[2] = pCube->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	for (int32 i = 0; i < 3; ++i)
	{
		FbxAnimCurve* lCurve = pCurve[i];
		lCurve->KeyModifyBegin();

		lTime.SetSecondDouble(0.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 1.0, FbxAnimCurveDef::eInterpolationLinear);

		lTime.SetSecondDouble(100.0 / 30.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 300.0f, FbxAnimCurveDef::eInterpolationLinear);
		lCurve->KeyModifyEnd();
	}
}

void CubeGeometryRotation(FbxNode* pCube)
{
	pCube->SetGeometricRotation(FbxNode::eSourcePivot, FbxVector4(0, 0, 50));
}

void CubeGeometryTranslate(FbxNode* pCube)
{
	pCube->SetGeometricTranslation(FbxNode::eSourcePivot, FbxVector4(-70, 0, 0));
}

void CubeGeometryScaling(FbxNode* pCube)
{
	pCube->SetGeometricScaling(FbxNode::eSourcePivot, FbxVector4(0.5, 1, 1));
}
static FString GetFbxPropertyFlags(FbxPropertyFlags::EFlags EProp)
{
	FString PropertyFlagsName;
	if (EProp & FbxPropertyFlags::EFlags::eNone)
	{
		PropertyFlagsName+=(TEXT("eNone "));
	}

	if (EProp & FbxPropertyFlags::EFlags::eStatic)
	{
		PropertyFlagsName += (TEXT("eStatic "));
	}

	if (EProp & FbxPropertyFlags::EFlags::eAnimatable)
	{
		PropertyFlagsName += (TEXT("eAnimatable "));
	}

	if (EProp & FbxPropertyFlags::EFlags::eAnimated)
	{
		PropertyFlagsName += (TEXT("eAnimated "));
	}

	if (EProp & FbxPropertyFlags::EFlags::eImported)
	{
		PropertyFlagsName += (TEXT("eImported "));
	}
	return MoveTemp(PropertyFlagsName);
}

static void FbxPropertyStudy(FbxNode* pCube)
{
	if (!pCube)
		return;
	auto flags = pCube->GeometricTranslation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("GeometricTranslation's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->GeometricRotation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("GeometricRotation's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->GeometricScaling.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("GeometricScaling's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->LclScaling.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("LclScaling's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->LclRotation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("LclRotation's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->LclTranslation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("LclTranslation's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->Visibility.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("Visibility's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->RotationOffset.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("RotationOffset's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->RotationPivot.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("RotationPivot's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->ScalingOffset.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("ScalingOffset's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->ScalingPivot.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("ScalingPivot's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->RotationOrder.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("RotationOrder's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->PreRotation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("PreRotation's Property is %s"), *GetFbxPropertyFlags(flags));
	flags = pCube->PostRotation.GetFlags();
	UE_LOG(FbxPivotLog, Warning, TEXT("PostRotation's Property is %s"), *GetFbxPropertyFlags(flags));
}
static void AnimateGeometryRotateAxis(FbxNode* pCube, FbxAnimLayer* pAnimLayer, int32 Axis)
{
	FbxAnimCurve* lCurve = NULL;
	FbxTime lTime;
	int lKeyIndex = 0;
	pCube->GeometricTranslation.GetCurveNode(pAnimLayer, true);
	auto pCurveAlraeadyHave = pCube->LclRotation.GetCurveNode(pAnimLayer, false);
	auto pCurveLclScaling = pCube->LclScaling.GetCurveNode(pAnimLayer, false);
	auto flags = pCube->GeometricTranslation.GetFlags();
	flags = pCube->GeometricRotation.GetFlags();
	flags = pCube->GeometricScaling.GetFlags();
	flags = pCube->LclScaling.GetFlags();
	flags = pCube->LclRotation.GetFlags();
	flags = pCube->LclTranslation.GetFlags();
	flags = pCube->Visibility.GetFlags();
	flags = pCube->RotationOffset.GetFlags();
	flags = pCube->RotationPivot.GetFlags();
	flags = pCube->ScalingOffset.GetFlags();
	flags = pCube->ScalingPivot.GetFlags();
	flags = pCube->RotationOrder.GetFlags();
	flags = pCube->PreRotation.GetFlags();
	flags = pCube->PostRotation.GetFlags();
	if (Axis == 0)
	{
		lCurve = pCube->GeometricTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
	}
	else if (Axis == 1)
	{
		lCurve = pCube->GeometricTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
	}
	else if (Axis == 2)
	{
		lCurve = pCube->GeometricTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
	}

	if (lCurve)
	{
		lCurve->KeyModifyBegin();

		lTime.SetSecondDouble(0.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 0.0, FbxAnimCurveDef::eInterpolationLinear);

		lTime.SetSecondDouble(12.0);
		lKeyIndex = lCurve->KeyAdd(lTime);
		lCurve->KeySet(lKeyIndex, lTime, 300.0, FbxAnimCurveDef::eInterpolationLinear);

		lCurve->KeyModifyEnd();
	}
}
void TestPivot()
{
	UE_LOG(FbxPivotLog, Log, TEXT("Test Pivot"));
	FbxManager* pManager = InitFBXSDK();
	check(pManager);
	CreateAndFillIOSettings(pManager);
	FbxScene* pScene = CreateScene(pManager);
	check(pScene);
	FbxAnimLayer* pAnimLayer = CreateAnimation(pScene);
	FbxMesh* pCubeMesh = CreatecuboidMeshTriangle(pScene, "CuboidMesh");
	//AnimateCubeRotateAxis(pCubeMesh->GetNode(), pAnimLayer, 0);
	//AnimateGeometryRotateAxis(pCubeMesh->GetNode(), pAnimLayer, 0);
	//AnimateCubeScalingAllAxis(pCubeMesh->GetNode(), pAnimLayer);
	//FbxPropertyStudy(pCubeMesh->GetNode());
	//pCubeMesh->GetNode()->SetRotationActive(false);
	//pCubeMesh->GetNode()->SetRotationPivot(FbxNode::eSourcePivot, FbxVector4(0.0, 0.0, 50.0));
	//pCubeMesh->GetNode()->SetGeometricTranslation(FbxNode::eSourcePivot, FbxVector4(0, 0, 50));
	//AnimateCubeScalingWithScalingPivotAndOffset(pCubeMesh->GetNode(), pAnimLayer);
	//AnimateCubeScalingWithScalingPivot(pCubeMesh->GetNode(), pAnimLayer);
	//AnimateCubeRotateAxisWithPivot(pCubeMesh->GetNode(), pAnimLayer,0);
	//AnimateCubeRotateAxisWithOffset(pCubeMesh->GetNode(), pAnimLayer,0);
	//AnimateCubePreRotate(pCubeMesh->GetNode(), pAnimLayer,0);
	pCubeMesh->GetNode()->SetRotationActive(true);
	//AnimateCubePostRotate(pCubeMesh->GetNode(), pAnimLayer,0);
	//AnimateCubePreRotate(pCubeMesh->GetNode(), pAnimLayer, 0);
	//AnimateCubeTranslate(pCubeMesh->GetNode(), pAnimLayer);
	//CubeGeometryRotation(pCubeMesh->GetNode());
	//CubeGeometryTranslate(pCubeMesh->GetNode());
	CubeGeometryScaling(pCubeMesh->GetNode());
	//pCubeMesh->GetNode()->LclTranslation = FbxVector4(0, 0, 0);
	//pCubeMesh->GetNode()->ScalingPivot = FbxVector4(0, 0, 0);
	pScene->GetRootNode()->AddChild(pCubeMesh->GetNode());


	SaveScene(pManager, pScene, "TestPivotCube.fbx");
	DestroySdkObjects(pManager);
}
