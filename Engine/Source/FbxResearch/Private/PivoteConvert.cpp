#include "TestCategory.h"
DECLARE_LOG_CATEGORY_EXTERN(FbxPivotConvertLog, Log, All);
DEFINE_LOG_CATEGORY(FbxPivotConvertLog)
void TestPivotConvert()
{
	UE_LOG(FbxPivotConvertLog, Log, TEXT("Test Pivot Convert"));
	FbxManager* pManager = InitFBXSDK();
	check(pManager);
	CreateAndFillIOSettings(pManager);
	FbxScene* pScene = CreateScene(pManager);
	check(pScene);
	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, "Self Animation Stack");
	FbxAnimLayer*  pAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
	lAnimStack->AddMember(pAnimLayer);
	FbxMesh* pCubeMesh = CreatecuboidMeshTriangle(pScene, "CuboidMesh");
	FbxNode* pCubeNode = pCubeMesh->GetNode();
	pScene->GetRootNode()->AddChild(pCubeNode);
	// Enable pivot
	pCubeNode->SetRotationActive(true);
	// Set the rotation pivot at the center of the pyramid
	pCubeNode->SetRotationPivot(FbxNode::eSourcePivot, FbxVector4(0, 2, 0));
	// Add a post rotation for the pyramid
	pCubeNode->SetPostRotation(FbxNode::eSourcePivot, FbxVector4(0, 0, -90));

	const double KEY_TIME[] = { 0.0, 0.5, 1.0 };
	const float KEY_VALUE[] = { 0.0, 90.0, 180.0 };
	// Animate the Y channel of the local rotation
	FbxAnimCurve * lAnimCurve = pCubeNode->LclRotation.GetCurve(pAnimLayer, "Y", true);
	if (lAnimCurve)
	{
		const int lKeyCount = sizeof(KEY_TIME) / sizeof(double);
		for (int lKeyIndex = 0; lKeyIndex < lKeyCount; ++lKeyIndex)
		{
			FbxTime lTime;
			FbxAnimCurveKey lKey;
			lTime.SetSecondDouble(KEY_TIME[lKeyIndex]);
			lKey.Set(lTime, KEY_VALUE[lKeyIndex]);
			lAnimCurve->KeyAdd(lTime, lKey);
		}
	}

	// Query the local transform and global transform of the pyramid node at 0.5 second
	FbxTime lTime;
	lTime.SetSecondDouble(0.5);
	FbxAMatrix lLocalTransform = pCubeNode->EvaluateLocalTransform(lTime);
	FbxAMatrix lGlobalTransform = pCubeNode->EvaluateGlobalTransform(lTime);

	// Save the scene before pivot converting
	bool lResult = SaveScene(pManager, pScene, "PivotConvertBefore");

	// Set the target of pivot converting
	// Reset the rotation pivot and post rotation, and maintain the animation
	pCubeNode->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);
	pCubeNode->SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive);
	pCubeNode->SetPostRotation(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0));
	pCubeNode->SetRotationPivot(FbxNode::eDestinationPivot, FbxVector4(0, 0, 0));

	// Convert the animation between source pivot set and destination pivot set with a frame rate of 30 per second
	pScene->GetRootNode()->ConvertPivotAnimationRecursive(lAnimStack, FbxNode::eDestinationPivot, 30.0);

	// Copy the rotation pivot and post rotation from destination set to source set in order to save them in file
	pCubeNode->SetRotationPivot(FbxNode::eSourcePivot, FbxVector4(0, 0, 0));
	pCubeNode->SetPostRotation(FbxNode::eSourcePivot, FbxVector4(0, 0, 0));

	// Save the scene after pivot converting
	lResult = SaveScene(pManager, pScene, "PivotConvertAfter");

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(pManager);
}
