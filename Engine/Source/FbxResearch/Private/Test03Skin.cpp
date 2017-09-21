#include "TestCategory.h"
DECLARE_LOG_CATEGORY_EXTERN(LogTestFbxSkin, Log, All);
DEFINE_LOG_CATEGORY(LogTestFbxSkin);

FbxMesh* CreateTrianglePatch(FbxScene* pScene,const FString& MeshName)
{
	FbxMesh* pMesh = FbxMesh::Create(pScene, TCHAR_TO_UTF8(*MeshName));
	const float BaseUnitSize = 10.0f;
	FbxVector4 LControlPoint0(-BaseUnitSize / 2, 0, 0);
	FbxVector4 LControlPoint1(BaseUnitSize / 2, 0, 0);
	FbxVector4 LControlPoint2(-BaseUnitSize / 2, BaseUnitSize, 0);
	FbxVector4 LControlPoint3(BaseUnitSize / 2, BaseUnitSize, 0);
	pMesh->InitControlPoints(4);
	FbxVector4* pControlpoints = pMesh->GetControlPoints();
	pControlpoints[0] = LControlPoint0;
	pControlpoints[1] = LControlPoint1;
	pControlpoints[2] = LControlPoint2;
	pControlpoints[3] = LControlPoint3;

	int lPolygonVertices[] = { 0, 1, 2, 3 };
	FbxGeometryElementNormal* lGeometryElementNormal = pMesh->CreateElementNormal();
	lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	lGeometryElementNormal->GetDirectArray().Add(FbxVector4(0, 0, 1));
	lGeometryElementNormal->GetIndexArray().Add(0);
	lGeometryElementNormal->GetIndexArray().Add(0);
	lGeometryElementNormal->GetIndexArray().Add(0);
	lGeometryElementNormal->GetIndexArray().Add(0);
	lGeometryElementNormal->GetIndexArray().Add(0);
	lGeometryElementNormal->GetIndexArray().Add(0);

	FbxGeometryElementUV* lUVDiffuseElement = pMesh->CreateElementUV("DiffuseUV");
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	lUVDiffuseElement->GetDirectArray().Add(FbxVector2(0,0));
	lUVDiffuseElement->GetDirectArray().Add(FbxVector2(1,0));
	lUVDiffuseElement->GetDirectArray().Add(FbxVector2(0,1));
	lUVDiffuseElement->GetDirectArray().Add(FbxVector2(1,1));
	lUVDiffuseElement->GetIndexArray().Add(0);
	lUVDiffuseElement->GetIndexArray().Add(1);
	lUVDiffuseElement->GetIndexArray().Add(3);
	lUVDiffuseElement->GetIndexArray().Add(0);
	lUVDiffuseElement->GetIndexArray().Add(3);
	lUVDiffuseElement->GetIndexArray().Add(2);

	

	FbxNode* lNode = FbxNode::Create(pScene, TCHAR_TO_UTF8(*(MeshName+TEXT("_Node"))));
	lNode->SetNodeAttribute(pMesh);
	FbxSurfacePhong* pMaterial = CreateMaterial(pScene);
	lNode->AddMaterial(pMaterial);
	lNode->SetShadingMode(FbxNode::eTextureShading);
	
	pMesh->BeginPolygon(-1, -1, -1, false);
	pMesh->AddPolygon(0);
	pMesh->AddPolygon(1);
	pMesh->AddPolygon(3);
	pMesh->EndPolygon();

	pMesh->BeginPolygon(-1, -1, -1, false);
	pMesh->AddPolygon(0);
	pMesh->AddPolygon(3);
	pMesh->AddPolygon(2);
	pMesh->EndPolygon();

	FbxGeometryElementMaterial* lMaterialElement = pMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	lMaterialElement->GetIndexArray().Add(0);
	lMaterialElement->GetIndexArray().Add(0);
	return pMesh;
}

FbxNode* CreateSkeleton(FbxScene* pScene, const char* pName)
{
	// Create skeleton root. 
	FbxString lRootName(pName);
	lRootName += "Root";
	FbxSkeleton* lSkeletonRootAttribute = FbxSkeleton::Create(pScene, pName);
	lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eRoot);
	FbxNode* lSkeletonRoot = FbxNode::Create(pScene, lRootName.Buffer());
	lSkeletonRoot->SetNodeAttribute(lSkeletonRootAttribute);
	lSkeletonRoot->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));

	// Create skeleton first limb node. 
	FbxString lLimbNodeName1(pName);
	lLimbNodeName1 += "LimbNode1";
	FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, lLimbNodeName1);
	lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);
	FbxNode* lSkeletonLimbNode1 = FbxNode::Create(pScene, lLimbNodeName1.Buffer());
	lSkeletonLimbNode1->SetNodeAttribute(lSkeletonLimbNodeAttribute1);
	lSkeletonLimbNode1->LclTranslation.Set(FbxVector4(0.0, 10.0, 0.0));

	// Create skeleton second limb node. 
	FbxString lLimbNodeName2(pName);
	lLimbNodeName2 += "LimbNode2";
	FbxSkeleton* lSkeletonLimbNodeAttribute2 = FbxSkeleton::Create(pScene, lLimbNodeName2);
	lSkeletonLimbNodeAttribute2->SetSkeletonType(FbxSkeleton::eLimbNode);
	FbxNode* lSkeletonLimbNode2 = FbxNode::Create(pScene, lLimbNodeName2.Buffer());
	lSkeletonLimbNode2->SetNodeAttribute(lSkeletonLimbNodeAttribute2);
	lSkeletonLimbNode2->LclTranslation.Set(FbxVector4(0.0, 10.0, 0.0));

	FbxString lLimbNodeName3(pName);
	lLimbNodeName3 += "LimbNode3";
	FbxSkeleton* lSkeletonLimbNodeAttribute3 = FbxSkeleton::Create(pScene, lLimbNodeName3);
	lSkeletonLimbNodeAttribute3->SetSkeletonType(FbxSkeleton::eLimbNode);
	FbxNode* lSkeletonLimbNode3 = FbxNode::Create(pScene, lLimbNodeName3.Buffer());
	lSkeletonLimbNode3->SetNodeAttribute(lSkeletonLimbNodeAttribute3);
	lSkeletonLimbNode3->LclTranslation.Set(FbxVector4(0.0, 10.0, 0.0));

	FbxString lLimbNodeName4(pName);
	lLimbNodeName4 += "LimbNode4";
	FbxSkeleton* lSkeletonLimbNodeAttribute4 = FbxSkeleton::Create(pScene, lLimbNodeName4);
	lSkeletonLimbNodeAttribute4->SetSkeletonType(FbxSkeleton::eLimbNode);
	FbxNode* lSkeletonLimbNode4 = FbxNode::Create(pScene, lLimbNodeName4.Buffer());
	lSkeletonLimbNode4->SetNodeAttribute(lSkeletonLimbNodeAttribute4);
	lSkeletonLimbNode4->LclTranslation.Set(FbxVector4(0.0, 10.0, 0.0));

	FbxString lLimbNodeName5(pName);
	lLimbNodeName5 += "LimbNode5";
	FbxSkeleton* lSkeletonLimbNodeAttribute5 = FbxSkeleton::Create(pScene, lLimbNodeName5);
	lSkeletonLimbNodeAttribute5->SetSkeletonType(FbxSkeleton::eLimbNode);
	FbxNode* lSkeletonLimbNode5 = FbxNode::Create(pScene, lLimbNodeName5.Buffer());
	lSkeletonLimbNode5->SetNodeAttribute(lSkeletonLimbNodeAttribute5);
	lSkeletonLimbNode5->LclTranslation.Set(FbxVector4(0.0, 10.0, 0.0));

	// Build skeleton node hierarchy. 
	lSkeletonRoot->AddChild(lSkeletonLimbNode1);
	lSkeletonLimbNode1->AddChild(lSkeletonLimbNode2);
	lSkeletonLimbNode2->AddChild(lSkeletonLimbNode3);
	lSkeletonLimbNode3->AddChild(lSkeletonLimbNode4);
	lSkeletonLimbNode4->AddChild(lSkeletonLimbNode5);

	return lSkeletonRoot;
}
void LinkPatchToSkeleton(FbxScene* pScene, FbxNode** pPatch, FbxNode* pSkeletonRoot)
{
	FbxAMatrix lXMatrix;

	FbxNode* SkeletonChain[6];
	SkeletonChain[0] = pSkeletonRoot;
	SkeletonChain[1] = pSkeletonRoot->GetChild(0);
	SkeletonChain[2] = SkeletonChain[1]->GetChild(0);
	SkeletonChain[3] = SkeletonChain[2]->GetChild(0);
	SkeletonChain[4] = SkeletonChain[3]->GetChild(0);
	SkeletonChain[5] = SkeletonChain[4]->GetChild(0);

	FbxNode* lLimbNode1 = pSkeletonRoot->GetChild(0);
	FbxNode* lLimbNode2 = lLimbNode1->GetChild(0);

	// Now we have the Patch and the skeleton correctly positioned,
	// set the Transform and TransformLink matrix accordingly.
	
	for (int32 i = 0; i < 5; ++i)
	{
		FbxSkin* lSkin = FbxSkin::Create(pScene, "SelfSkin");
		lXMatrix = pPatch[i]->EvaluateGlobalTransform();
		FbxCluster* Cluster = FbxCluster::Create(pScene, "");
		Cluster->SetLink(SkeletonChain[i]);
		Cluster->SetLinkMode(FbxCluster::eNormalize);
		
		Cluster->SetTransformMatrix(lXMatrix);

		lXMatrix = SkeletonChain[i]->EvaluateGlobalTransform();
		Cluster->SetTransformLinkMatrix(lXMatrix);
			Cluster->AddControlPointIndex(0, 0.5);
			Cluster->AddControlPointIndex(1, 0.5);
			Cluster->AddControlPointIndex(2, 0.5);
			Cluster->AddControlPointIndex(3, 0.5);
		lSkin->AddCluster(Cluster);

		Cluster = FbxCluster::Create(pScene, "");
		Cluster->SetLink(SkeletonChain[i+1]);
		Cluster->SetLinkMode(FbxCluster::eNormalize);
		lXMatrix = pPatch[i]->EvaluateGlobalTransform();
		Cluster->SetTransformMatrix(lXMatrix);

		lXMatrix = SkeletonChain[i+1]->EvaluateGlobalTransform();
		Cluster->SetTransformLinkMatrix(lXMatrix);
			Cluster->AddControlPointIndex(0, 0.5);
			Cluster->AddControlPointIndex(1, 0.5);
			Cluster->AddControlPointIndex(2, 0.5);
			Cluster->AddControlPointIndex(3, 0.5);

		lSkin->AddCluster(Cluster);
		FbxGeometry* lPatchAttribute = (FbxGeometry*)(pPatch[i]->GetNodeAttribute());
		lPatchAttribute->AddDeformer(lSkin);
	}
}

void AddNodeRecursively(FbxArray<FbxNode*>& pNodeArray, FbxNode* pNode)
{
	if (pNode)
	{
		AddNodeRecursively(pNodeArray, pNode->GetParent());

		if (pNodeArray.Find(pNode) == -1)
		{
			// Node not in the list, add it
			pNodeArray.Add(pNode);
		}
	}
}
// Store the Bind Pose
void StoreBindPose(FbxScene* pScene, TArray<FbxNode*> Patchs)
{
	// In the bind pose, we must store all the link's global matrix at the time of the bind.
	// Plus, we must store all the parent(s) global matrix of a link, even if they are not
	// themselves deforming any model.

	// In this example, since there is only one model deformed, we don't need walk through 
	// the scene
	//

	// Now list the all the link involve in the patch deformation
	FbxArray<FbxNode*> lClusteredFbxNodes;
	int                       i, j;

	for (FbxNode* pPatch : Patchs)
	{
		if (pPatch && pPatch->GetNodeAttribute())
		{
			int lSkinCount = 0;
			int lClusterCount = 0;
			switch (pPatch->GetNodeAttribute()->GetAttributeType())
			{
			default:
				break;
			case FbxNodeAttribute::eMesh:
			case FbxNodeAttribute::eNurbs:
			case FbxNodeAttribute::ePatch:

				lSkinCount = ((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
				//Go through all the skins and count them
				//then go through each skin and get their cluster count
				for (i = 0; i < lSkinCount; ++i)
				{
					FbxSkin *lSkin = (FbxSkin*)((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
					lClusterCount += lSkin->GetClusterCount();
				}
				break;
			}
			//if we found some clusters we must add the node
			if (lClusterCount)
			{
				//Again, go through all the skins get each cluster link and add them
				for (i = 0; i < lSkinCount; ++i)
				{
					FbxSkin *lSkin = (FbxSkin*)((FbxGeometry*)pPatch->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
					lClusterCount = lSkin->GetClusterCount();
					for (j = 0; j < lClusterCount; ++j)
					{
						FbxNode* lClusterNode = lSkin->GetCluster(j)->GetLink();
						AddNodeRecursively(lClusteredFbxNodes, lClusterNode);
					}

				}

				// Add the patch to the pose
				lClusteredFbxNodes.Add(pPatch);
			}
		}
	}

	// Now create a bind pose with the link list
	if (lClusteredFbxNodes.GetCount())
	{
		// A pose must be named. Arbitrarily use the name of the patch node.
		FbxPose* lPose = FbxPose::Create(pScene, "SelfPose");

		// default pose type is rest pose, so we need to set the type as bind pose
		lPose->SetIsBindPose(true);

		for (i = 0; i < lClusteredFbxNodes.GetCount(); i++)
		{
			FbxNode*  lKFbxNode = lClusteredFbxNodes.GetAt(i);
			FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();

			lPose->Add(lKFbxNode, lBindMatrix);
		}

		// Add the pose to the scene
		pScene->AddPose(lPose);
	}
}
void TestSkin()
{
	UE_LOG(LogTestFbxSkin, Log, TEXT("Start Test Fbx Skin"));
	FbxManager* pManager = InitFBXSDK();
	check(pManager);
	CreateAndFillIOSettings(pManager);
	FbxScene* pScene = CreateScene(pManager);
	FbxMesh* pMesh0 = CreateTrianglePatch(pScene, TEXT("Mesh0"));
	FbxMesh* pMesh1 = CreateTrianglePatch(pScene, TEXT("Mesh1"));
	FbxMesh* pMesh2 = CreateTrianglePatch(pScene, TEXT("Mesh2"));
	FbxMesh* pMesh3 = CreateTrianglePatch(pScene, TEXT("Mesh3"));
	FbxMesh* pMesh4 = CreateTrianglePatch(pScene, TEXT("Mesh4"));
	FbxNode* pRootNode = pScene->GetRootNode();
	pRootNode->AddChild(pMesh0->GetNode());
	pRootNode->AddChild(pMesh1->GetNode());
	pMesh1->GetNode()->LclTranslation.Set(FbxVector4(0, 10, 0));
	pMesh2->GetNode()->LclTranslation.Set(FbxVector4(0, 20, 0));
	pMesh3->GetNode()->LclTranslation.Set(FbxVector4(0, 30, 0));
	pMesh4->GetNode()->LclTranslation.Set(FbxVector4(0, 40, 0));
	pRootNode->AddChild(pMesh2->GetNode());
	pRootNode->AddChild(pMesh3->GetNode());
	pRootNode->AddChild(pMesh4->GetNode());

	FbxNode* pSkeletonRootNode = CreateSkeleton(pScene, "SelfSkeleton");
	pRootNode->AddChild(pSkeletonRootNode);
	FbxNode* pPatchNode[] = { pMesh0->GetNode(),pMesh1->GetNode(),pMesh2->GetNode(),pMesh3->GetNode(),pMesh4->GetNode() };
	TArray<FbxNode*> Patchs= { pMesh0->GetNode(),pMesh1->GetNode(),pMesh2->GetNode(),pMesh3->GetNode(),pMesh4->GetNode() };
	LinkPatchToSkeleton(pScene, pPatchNode, pSkeletonRootNode);
	StoreBindPose(pScene, Patchs);
	SaveScene(pManager, pScene, "FbxSkin");
	check(pScene);
}