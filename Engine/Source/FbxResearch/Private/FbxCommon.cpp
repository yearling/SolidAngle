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

FbxManager* InitFBXSDK()
{
	FbxManager* pManager = FbxManager::Create();
	if (!pManager)
	{
		UE_LOG(FbxSDKLog, Error, TEXT("Error: Unable to create FBX Manager!\n"));
	}
	else
	{
		UE_LOG(FbxSDKLog, Log, TEXT("Autodesk FBX SDK version %ls"), ANSI_TO_TCHAR(pManager->GetVersion()));
	}
	

	return pManager;
}

FbxScene* CreateScene(FbxManager* pManager)
{
	return FbxScene::Create(pManager, "My Scene");
}

void DestroySdkObjects(FbxManager* pManager)
{
	if (pManager) pManager->Destroy();
}

void CreateAndFillIOSettings(FbxManager* pManager)
{
	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());
}

FbxAnimLayer*  CreateAnimation(FbxScene* pScene)
{
	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, "Self Animation Stack");
	FbxAnimLayer*  pAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
	lAnimStack->AddMember(pAnimLayer);
	return pAnimLayer;
}

FbxFileTexture*  CreateTexture(FbxScene* pScene)
{
	FbxFileTexture* pTexture = FbxFileTexture::Create(pScene, "Diffuse Texture");

	// Resource file must be in the application's directory.
	FbxString lTexPath = "\\Crate.jpg";

	// Set texture properties.
	pTexture->SetFileName(lTexPath.Buffer());
	pTexture->SetTextureUse(FbxTexture::eStandard);
	pTexture->SetMappingType(FbxTexture::eUV);
	pTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
	pTexture->SetSwapUV(false);
	pTexture->SetTranslation(0.0, 0.0);
	pTexture->SetScale(1.0, 1.0);
	pTexture->SetRotation(0.0, 0.0);
	return pTexture;
}




FbxSurfacePhong* CreateMaterial(FbxScene* pScene)
{
	FbxString lMaterialName = "SelfCreateMtl";
	FbxString lShadingName = "Phong";
	FbxDouble3 lBlack(0.0, 0.0, 0.0);
	FbxDouble3 lWhite(1.0, 1.0, 1.0);
	FbxDouble3 lDiffuseColor(0.75, 0.75, 0.0);
	FbxSurfacePhong*  pMaterial = FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());

	// Generate primary and secondary colors.
	pMaterial->Emissive.Set(lBlack);
	pMaterial->Ambient.Set(lBlack);
	pMaterial->Diffuse.Set(lWhite);
	pMaterial->TransparencyFactor.Set(40.5);
	pMaterial->ShadingModel.Set(lShadingName);
	pMaterial->Shininess.Set(0.5);

	// the texture need to be connected to the material on the corresponding property
	FbxFileTexture*  pTexture = CreateTexture(pScene);

	if (pTexture)
		pMaterial->Diffuse.ConnectSrcObject(pTexture);
	return pMaterial;
}


void AddMaterials(FbxMesh* pMesh, FbxSurfacePhong* pMaterial)
{
	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = pMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	//get the node of mesh, add material for it.
	FbxNode* lNode = pMesh->GetNode();
	if (lNode == NULL)
		return;
	lNode->AddMaterial(pMaterial);

	// We are in eByPolygon, so there's only need for 12 index (a cube has 12 polygons).
	lMaterialElement->GetIndexArray().SetCount(12);

	// Set the Index 0 to 6 to the material in position 0 of the direct array.
	for (int i = 0; i < 12; ++i)
		lMaterialElement->GetIndexArray().SetAt(i, 0);
}

FbxMesh* CreateCubeMeshPolygon(FbxScene* pScene, char* pName)
{
	int i, j;
	FbxMesh* lMesh = FbxMesh::Create(pScene, pName);

	FbxVector4 lControlPoint0(-50, 0, 50);
	FbxVector4 lControlPoint1(50, 0, 50);
	FbxVector4 lControlPoint2(50, 100, 50);
	FbxVector4 lControlPoint3(-50, 100, 50);
	FbxVector4 lControlPoint4(-50, 0, -50);
	FbxVector4 lControlPoint5(50, 0, -50);
	FbxVector4 lControlPoint6(50, 100, -50);
	FbxVector4 lControlPoint7(-50, 100, -50);

	FbxVector4 lNormalXPos(1, 0, 0);
	FbxVector4 lNormalXNeg(-1, 0, 0);
	FbxVector4 lNormalYPos(0, 1, 0);
	FbxVector4 lNormalYNeg(0, -1, 0);
	FbxVector4 lNormalZPos(0, 0, 1);
	FbxVector4 lNormalZNeg(0, 0, -1);

	// Create control points.
	lMesh->InitControlPoints(24);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;
	lControlPoints[4] = lControlPoint1;
	lControlPoints[5] = lControlPoint5;
	lControlPoints[6] = lControlPoint6;
	lControlPoints[7] = lControlPoint2;
	lControlPoints[8] = lControlPoint5;
	lControlPoints[9] = lControlPoint4;
	lControlPoints[10] = lControlPoint7;
	lControlPoints[11] = lControlPoint6;
	lControlPoints[12] = lControlPoint4;
	lControlPoints[13] = lControlPoint0;
	lControlPoints[14] = lControlPoint3;
	lControlPoints[15] = lControlPoint7;
	lControlPoints[16] = lControlPoint3;
	lControlPoints[17] = lControlPoint2;
	lControlPoints[18] = lControlPoint6;
	lControlPoints[19] = lControlPoint7;
	lControlPoints[20] = lControlPoint1;
	lControlPoints[21] = lControlPoint0;
	lControlPoints[22] = lControlPoint4;
	lControlPoints[23] = lControlPoint5;

	// We want to have one normal for each vertex (or control point),
	// so we set the mapping mode to eByControlPoint.
	FbxGeometryElementNormal* lGeometryElementNormal = lMesh->CreateElementNormal();

	lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);

	// Set the normal values for every control point.
	lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);


	// Array of polygon vertices.
	int lPolygonVertices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,12, 13,
		14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };


	// Create UV for Diffuse channel.
	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV("DiffuseUV");
	FBX_ASSERT(lUVDiffuseElement != NULL);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	FbxVector2 lVectors0(0, 0);
	FbxVector2 lVectors1(1, 0);
	FbxVector2 lVectors2(1, 1);
	FbxVector2 lVectors3(0, 1);

	lUVDiffuseElement->GetDirectArray().Add(lVectors0);
	lUVDiffuseElement->GetDirectArray().Add(lVectors1);
	lUVDiffuseElement->GetDirectArray().Add(lVectors2);
	lUVDiffuseElement->GetDirectArray().Add(lVectors3);

	//Now we have set the UVs as eIndexToDirect reference and in eByPolygonVertex  mapping mode
	//we must update the size of the index array.
	lUVDiffuseElement->GetIndexArray().SetCount(24);

	// Create polygons. Assign texture and texture UV indices.
	for (i = 0; i < 6; i++)
	{
		// all faces of the cube have the same texture
		lMesh->BeginPolygon(-1, -1, -1, false);

		for (j = 0; j < 4; j++)
		{
			// Control point index
			lMesh->AddPolygon(lPolygonVertices[i * 4 + j]);

			// update the index array of the UVs that map the texture to the face
			lUVDiffuseElement->GetIndexArray().SetAt(i * 4 + j, j);
		}

		lMesh->EndPolygon();
	}

	// create a FbxNode
	FbxNode* lNode = FbxNode::Create(pScene, pName);

	// set the node attribute
	lNode->SetNodeAttribute(lMesh);

	// set the shading mode to view texture
	lNode->SetShadingMode(FbxNode::eTextureShading);

	// rescale the cube
	//lNode->LclScaling.Set(FbxVector4(0.3, 0.3, 0.3));

	// return the FbxNode
	return lMesh;
}

FbxMesh* CreateCubeMeshTriangle(FbxScene* pScene, char* pName)
{
	FbxMesh* lMesh = FbxMesh::Create(pScene, pName);

	FbxVector4 lControlPoint0(-50, 0, 50);
	FbxVector4 lControlPoint1(50, 0, 50);
	FbxVector4 lControlPoint2(50, 100, 50);
	FbxVector4 lControlPoint3(-50, 100, 50);
	FbxVector4 lControlPoint4(-50, 0, -50);
	FbxVector4 lControlPoint5(50, 0, -50);
	FbxVector4 lControlPoint6(50, 100, -50);
	FbxVector4 lControlPoint7(-50, 100, -50);

	FbxVector4 lNormalXPos(1, 0, 0);
	FbxVector4 lNormalXNeg(-1, 0, 0);
	FbxVector4 lNormalYPos(0, 1, 0);
	FbxVector4 lNormalYNeg(0, -1, 0);
	FbxVector4 lNormalZPos(0, 0, 1);
	FbxVector4 lNormalZNeg(0, 0, -1);

	// Create control points.
	lMesh->InitControlPoints(8);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;
	lControlPoints[4] = lControlPoint4;
	lControlPoints[5] = lControlPoint5;
	lControlPoints[6] = lControlPoint6;
	lControlPoints[7] = lControlPoint7;

	// We want to have one normal for each vertex (or control point),
	// so we set the mapping mode to eByControlPoint.
	FbxGeometryElementNormal* lGeometryElementNormal = lMesh->CreateElementNormal();

	lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);

	// Set the normal values for every control point.
	lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos); //0
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg); //1
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos); //2
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg); //3
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos); //4
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg); //5
	lGeometryElementNormal->GetIndexArray().SetCount(36);

	// Array of polygon vertices.
	int lPolygonVertices[] = { 0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 7,
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		3, 2, 6,
		3, 6, 7,
		0, 5, 1,
		0, 4, 5 };
	int NormalIndex[] = {
		4, 4, 4,
		4, 4, 4,
		0, 0, 0,
		0, 0, 0,
		5, 5, 5,
		5, 5, 5,
		1, 1, 1,
		1, 1, 1,
		2, 2, 2,
		2, 2, 2,
		3, 3, 3,
		3, 3, 3
	};

	int UVIndex[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 3, 2,
		0, 1, 2,
		0, 2 ,3,
		0,2 ,1,
		0, 3, 2
	};
	// Create UV for Diffuse channel.
	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV("DiffuseUV");
	FBX_ASSERT(lUVDiffuseElement != NULL);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	FbxVector2 lVectors0(0, 0);
	FbxVector2 lVectors1(1, 0);
	FbxVector2 lVectors2(1, 1);
	FbxVector2 lVectors3(0, 1);

	lUVDiffuseElement->GetDirectArray().Add(lVectors0);
	lUVDiffuseElement->GetDirectArray().Add(lVectors1);
	lUVDiffuseElement->GetDirectArray().Add(lVectors2);
	lUVDiffuseElement->GetDirectArray().Add(lVectors3);

	//Now we have set the UVs as eIndexToDirect reference and in eByPolygonVertex  mapping mode
	//we must update the size of the index array.
	lUVDiffuseElement->GetIndexArray().SetCount(36);

	// Create polygons. Assign texture and texture UV indices.
	for (int32 i = 0; i < 12; i++)
	{
		// all faces of the cube have the same texture
		lMesh->BeginPolygon(-1, -1, -1, false);

		for (int32 j = 0; j < 3; j++)
		{
			// Control point index
			lMesh->AddPolygon(lPolygonVertices[i * 3 + j]);

			// update the index array of the UVs that map the texture to the face
			lUVDiffuseElement->GetIndexArray().SetAt(i * 3 + j, UVIndex[i * 3 + j]);
			//lGeometryElementNormal->GetIndexArray().SetAt(i*3+j,NormalIndex[i+3+j]); 
			lGeometryElementNormal->GetIndexArray().SetAt(i * 3 + j, NormalIndex[i * 3 + j]);
		}
		lMesh->EndPolygon();
	}
	// create a FbxNode
	FbxNode* lNode = FbxNode::Create(pScene, pName);

	// set the node attribute
	lNode->SetNodeAttribute(lMesh);

	// set the shading mode to view texture
	lNode->SetShadingMode(FbxNode::eTextureShading);

	// rescale the cube
	//lNode->LclScaling.Set(FbxVector4(0.3, 0.3, 0.3));

	// return the FbxNode
	FbxSurfacePhong* pMaterial = CreateMaterial(pScene);
	AddMaterials(lMesh, pMaterial);
	return lMesh;
}




FbxMesh* CreatecuboidMeshTriangle(FbxScene* pScene, char* pName)
{
	FbxMesh* lMesh = FbxMesh::Create(pScene, pName);
	const double x = 70.0;
	const double y = 30.0;
	const double z = 50.0;
	FbxVector4 lControlPoint0(-x, -y, z);
	FbxVector4 lControlPoint1(x, -y, z);
	FbxVector4 lControlPoint2(x, y, z);
	FbxVector4 lControlPoint3(-x, y, z);
	FbxVector4 lControlPoint4(-x, -y, -z);
	FbxVector4 lControlPoint5(x, -y, -z);
	FbxVector4 lControlPoint6(x, y, -z);
	FbxVector4 lControlPoint7(-x, y, -z);

	FbxVector4 lNormalXPos(1, 0, 0);
	FbxVector4 lNormalXNeg(-1, 0, 0);
	FbxVector4 lNormalYPos(0, 1, 0);
	FbxVector4 lNormalYNeg(0, -1, 0);
	FbxVector4 lNormalZPos(0, 0, 1);
	FbxVector4 lNormalZNeg(0, 0, -1);

	// Create control points.
	lMesh->InitControlPoints(8);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;
	lControlPoints[4] = lControlPoint4;
	lControlPoints[5] = lControlPoint5;
	lControlPoints[6] = lControlPoint6;
	lControlPoints[7] = lControlPoint7;

	// We want to have one normal for each vertex (or control point),
	// so we set the mapping mode to eByControlPoint.
	FbxGeometryElementNormal* lGeometryElementNormal = lMesh->CreateElementNormal();

	lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);

	// Set the normal values for every control point.
	lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos); //0
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg); //1
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos); //2
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg); //3
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos); //4
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg); //5
	lGeometryElementNormal->GetIndexArray().SetCount(36);

	// Array of polygon vertices.
	int lPolygonVertices[] = { 0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 7,
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		3, 2, 6,
		3, 6, 7,
		0, 5, 1,
		0, 4, 5 };
	int NormalIndex[] = {
		4, 4, 4,
		4, 4, 4,
		0, 0, 0,
		0, 0, 0,
		5, 5, 5,
		5, 5, 5,
		1, 1, 1,
		1, 1, 1,
		2, 2, 2,
		2, 2, 2,
		3, 3, 3,
		3, 3, 3
	};

	int UVIndex[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 2, 3,
		0, 1, 2,
		0, 3, 2,
		0, 1, 2,
		0, 2 ,3,
		0,2 ,1,
		0, 3, 2
	};
	// Create UV for Diffuse channel.
	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV("DiffuseUV");
	FBX_ASSERT(lUVDiffuseElement != NULL);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	FbxVector2 lVectors0(0, 0);
	FbxVector2 lVectors1(1, 0);
	FbxVector2 lVectors2(1, 1);
	FbxVector2 lVectors3(0, 1);

	lUVDiffuseElement->GetDirectArray().Add(lVectors0);
	lUVDiffuseElement->GetDirectArray().Add(lVectors1);
	lUVDiffuseElement->GetDirectArray().Add(lVectors2);
	lUVDiffuseElement->GetDirectArray().Add(lVectors3);

	//Now we have set the UVs as eIndexToDirect reference and in eByPolygonVertex  mapping mode
	//we must update the size of the index array.
	lUVDiffuseElement->GetIndexArray().SetCount(36);

	// Create polygons. Assign texture and texture UV indices.
	for (int32 i = 0; i < 12; i++)
	{
		// all faces of the cube have the same texture
		lMesh->BeginPolygon(-1, -1, -1, false);

		for (int32 j = 0; j < 3; j++)
		{
			// Control point index
			lMesh->AddPolygon(lPolygonVertices[i * 3 + j]);

			// update the index array of the UVs that map the texture to the face
			lUVDiffuseElement->GetIndexArray().SetAt(i * 3 + j, UVIndex[i * 3 + j]);
			//lGeometryElementNormal->GetIndexArray().SetAt(i*3+j,NormalIndex[i+3+j]); 
			lGeometryElementNormal->GetIndexArray().SetAt(i * 3 + j, NormalIndex[i * 3 + j]);
		}
		lMesh->EndPolygon();
	}
	// create a FbxNode
	FbxNode* lNode = FbxNode::Create(pScene, pName);

	// set the node attribute
	lNode->SetNodeAttribute(lMesh);

	// set the shading mode to view texture
	lNode->SetShadingMode(FbxNode::eTextureShading);

	// rescale the cube
	//lNode->LclScaling.Set(FbxVector4(0.3, 0.3, 0.3));

	// return the FbxNode
	FbxSurfacePhong* pMaterial = CreateMaterial(pScene);
	AddMaterials(lMesh, pMaterial);
	return lMesh;
}

bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename)
{
	if (pSdkManager == NULL) return false;
	if (pScene == NULL) return false;
	if (pFilename == NULL) return false;

	bool lStatus = true;

	// Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(pSdkManager, "");
	if (lExporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings()) == false)
	{
		return false;
	}

	// Set the export states. By default, the export states are always set to 
	// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
	// shows how to change these states.
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_MATERIAL, true);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_TEXTURE, true);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_EMBEDDED, false);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_SHAPE, true);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_GOBO, true);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_ANIMATION, true);
	pSdkManager->GetIOSettings()->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	// Export the scene.
	lStatus = lExporter->Export(pScene);

	// Destroy the exporter.
	lExporter->Destroy();

	return lStatus;
}