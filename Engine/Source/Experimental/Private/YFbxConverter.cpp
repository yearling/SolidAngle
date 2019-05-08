#include "YFbxConverter.h"
#include "Logging\LogMacros.h"
#include "fbxsdk\core\fbxmanager.h"
#include "fbxsdk\scene\geometry\fbxmesh.h"
#include "fbxsdk\scene\fbxaxissystem.h"
#include "FbxSkeletalMeshImportData.h"
#include "RawMesh.h"
#include "StaticMesh.h"


DEFINE_LOG_CATEGORY(LogYFbxConverter);
YFbxConverter::YFbxConverter()
{
	// Create the SdkManager
	SdkManager = FbxManager::Create();

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(ios);
	GeometryConverter = new FbxGeometryConverter(SdkManager);
}


YFbxConverter::~YFbxConverter()
{
}

bool YFbxConverter::Init(const FString& Filename)
{
	FileToImport = Filename;
	Importer = FbxImporter::Create(SdkManager, "");
	int32 SDKMajor, SDKMinor, SDKRevision;
	FbxManager::GetFileFormatVersion(SDKMajor, SDKMinor, SDKRevision);
	Importer->ParseForStatistics(true);
	const bool bImportStatus = Importer->Initialize(TCHAR_TO_UTF8(*Filename));
	if (!bImportStatus)  // Problem with the file to be imported
	{
		UE_LOG(LogYFbxConverter, Error, TEXT("Call to FbxImporter::Initialize() failed."));
		UE_LOG(LogYFbxConverter, Warning, TEXT("Error returned: %s"), UTF8_TO_TCHAR(Importer->GetStatus().GetErrorString()));

		if (Importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			UE_LOG(LogYFbxConverter, Warning, TEXT("FBX version number for this FBX SDK is %d.%d.%d"),
				SDKMajor, SDKMinor, SDKRevision);
		}
		return false;
	}

	int32 FileMajor, FileMinor, FileRevision;
	Importer->GetFileVersion(FileMajor, FileMinor, FileRevision);

	int32 FileVersion = (FileMajor << 16 | FileMinor << 8 | FileRevision);
	int32 SDKVersion = (SDKMajor << 16 | SDKMinor << 8 | SDKRevision);

	if (FileVersion != SDKVersion)
	{

		// Appending the SDK version to the config key causes the warning to automatically reappear even if previously suppressed when the SDK version we use changes. 
		FString ConfigStr = FString::Printf(TEXT("Warning_OutOfDateFBX_%d"), SDKVersion);

		FString FileVerStr = FString::Printf(TEXT("%d.%d.%d"), FileMajor, FileMinor, FileRevision);
		FString SDKVerStr = FString::Printf(TEXT("%d.%d.%d"), SDKMajor, SDKMinor, SDKRevision);
		UE_LOG(LogYFbxConverter, Warning, TEXT("An out of date FBX has been detected.\nImporting different versions of FBX files than the SDK version can cause undesirable results.\n\nFile Version: %s\nSDK Version: %s"),
			*FileVerStr, *SDKVerStr);
	}

	FbxStatistics Statistics;
	Importer->GetStatistics(&Statistics);
	int32 ItemIndex;
	FbxString ItemName;
	int32 ItemCount;
	bool bHasAnimation = false;

	for (ItemIndex = 0; ItemIndex < Statistics.GetNbItems(); ItemIndex++)
	{
		Statistics.GetItemPair(ItemIndex, ItemName, ItemCount);
		const FString NameBuffer(ItemName.Buffer());
		UE_LOG(LogYFbxConverter, Log, TEXT("ItemName: %s, ItemCount : %d"), *NameBuffer, ItemCount);
	}

	Scene = FbxScene::Create(SdkManager, "");
	UE_LOG(LogYFbxConverter, Log, TEXT("Loading FBX Scene from %s"), *Filename);


	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_SKINS, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_DEFORMATION, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	 SdkManager->GetIOSettings()->SetBoolProp(IMP_TAKE, true);

	 bool bStatus = Importer->Import(Scene);
	YFbxSceneInfo SceneInfo;
	if (GetSceneInfo(Filename, SceneInfo))
	{
		if (SceneInfo.SkinnedMeshNum > 0)
		{
			bHasSkin = true;
		}
		else if(SceneInfo.TotalGeometryNum>0)
		{
			bHasStaticMesh = true;
		}
		if (SceneInfo.bHasAnimation)
		{
			bHasAnimation = true;
		}
	}
	return true;
}


bool YFbxConverter::Import(TUniquePtr<YFBXImportOptions> ImportOptionsIn)
{
	ImportOptions = std::move(ImportOptionsIn);
	if (!ConvertScene())
		return false;

	FbxNode* RootNodeToImport = nullptr;
	RootNodeToImport = Scene->GetRootNode();

	int32 InterestingNodeCount = 1;

	bool bImportStaticMeshLODs = ImportOptions->bImportStaticMeshLODs;
	bool bCombineMeshes = ImportOptions->bCombineToSingle;
	bool bCombineMeshesLOD = false;
	if (bHasStaticMesh)
	{
		if (bCombineMeshes && !bImportStaticMeshLODs)
		{
			InterestingNodeCount = 1;
		}
		else
		{
			// 如果不引用lod，则统计mesh的在lod中的数量
			bool bCountLODGroupMeshes = !bImportStaticMeshLODs;
			int32 NumLODGroups = 0;
			InterestingNodeCount = GetFbxMeshCount(RootNodeToImport, bCountLODGroupMeshes, NumLODGroups);
			if (bImportStaticMeshLODs && bCombineMeshes && NumLODGroups > 0)
			{
				bCombineMeshes = false;
				bCombineMeshesLOD = true;
			}
		}
		
		if (RootNodeToImport && InterestingNodeCount > 0)
		{
			int32 NodeIndex = 0;
			int32 ImportedMeshCount = 0;
			UStaticMesh* NewStaticMesh = NULL;
			if (bCombineMeshes)
			{
				TArray<FbxNode*> FbxMeshArray;
				FillFbxMeshArray(RootNodeToImport, FbxMeshArray);
				if (FbxMeshArray > 0)
				{

				}
			}
		}
	}

	return true;
}

bool YFbxConverter::GetSceneInfo(const FString& FileName, YFbxSceneInfo& SceneInfo)
{
	FbxTimeSpan GlobalTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
	SceneInfo.TotalMaterialNum = Scene->GetMaterialCount();
	SceneInfo.TotalTextureNum = Scene->GetTextureCount();
	SceneInfo.TotalGeometryNum = 0;
	SceneInfo.NonSkinnedMeshNum = 0;
	SceneInfo.SkinnedMeshNum = 0;
	for (int32 GeometryIndex = 0; GeometryIndex < Scene->GetGeometryCount(); GeometryIndex++)
	{
		FbxGeometry * Geometry = Scene->GetGeometry(GeometryIndex);
		if (Geometry->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxNode* GeoNode = Geometry->GetNode();
			FbxMesh* Mesh = (FbxMesh*)Geometry;
			//Skip staticmesh sub LOD group that will be merge with the other same lod index mesh
			if (GeoNode && Mesh->GetDeformerCount(FbxDeformer::eSkin) <= 0)
			{
				FbxNode* ParentNode = RecursiveFindParentLodGroup(GeoNode->GetParent());
				if (ParentNode != nullptr && ParentNode->GetNodeAttribute() && ParentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
				{
					bool IsLodRoot = false;
					for (int32 ChildIndex = 0; ChildIndex < ParentNode->GetChildCount(); ++ChildIndex)
					{
						FbxNode *MeshNode = FindLODGroupNode(ParentNode, ChildIndex);
						if (GeoNode == MeshNode)
						{
							IsLodRoot = true;
							break;
						}
					}
					if (!IsLodRoot)
					{
						//Skip static mesh sub LOD
						// 如下图效果
						//			root 
						//			/
						//		LOD0
						//   /    |      \
						//  mesh0  dummy0  mesh1
						//         /   \
						//      mesh2  mesh3
						//  其中mesh3就被continue了，原因是不属于lod子节点的第一个
						continue;
					}
				}
			}
			SceneInfo.TotalGeometryNum++;

			SceneInfo.MeshInfo.AddZeroed(1);
			YFbxMeshInfo& MeshInfo = SceneInfo.MeshInfo.Last();
			if (Geometry->GetName()[0] != '\0')
				MeshInfo.Name = MakeName(Geometry->GetName());
			else
				MeshInfo.Name = MakeString(GeoNode ? GeoNode->GetName() : "None");
			MeshInfo.bTriangulated = Mesh->IsTriangleMesh();
			MeshInfo.MaterialNum = GeoNode ? GeoNode->GetMaterialCount() : 0;
			MeshInfo.FaceNum = Mesh->GetPolygonCount();
			MeshInfo.VertexNum = Mesh->GetControlPointsCount();

			// LOD info
			MeshInfo.LODGroup = NULL;
			if (GeoNode)
			{
				FbxNode* ParentNode = RecursiveFindParentLodGroup(GeoNode->GetParent());
				if (ParentNode != nullptr && ParentNode->GetNodeAttribute() && ParentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
				{
					MeshInfo.LODGroup = MakeString(ParentNode->GetName());
					for (int32 LODIndex = 0; LODIndex < ParentNode->GetChildCount(); LODIndex++)
					{
						FbxNode *MeshNode = FindLODGroupNode(ParentNode, LODIndex, GeoNode);
						if (GeoNode == MeshNode)
						{
							MeshInfo.LODLevel = LODIndex;
							break;
						}
					}
				}
			}

			// skeletal mesh
			if (Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
			{
				SceneInfo.SkinnedMeshNum++;
				MeshInfo.bIsSkelMesh = true;
				MeshInfo.MorphNum = Mesh->GetShapeCount();
				// skeleton root
				FbxSkin* Skin = (FbxSkin*)Mesh->GetDeformer(0, FbxDeformer::eSkin);
				int32 ClusterCount = Skin->GetClusterCount();
				FbxNode* Link = NULL;
				for (int32 ClusterId = 0; ClusterId < ClusterCount; ++ClusterId)
				{
					FbxCluster* Cluster = Skin->GetCluster(ClusterId);
					Link = Cluster->GetLink();
					while (Link && Link->GetParent() && Link->GetParent()->GetSkeleton())
					{
						Link = Link->GetParent();
					}

					if (Link != NULL)
					{
						break;
					}
				}

				MeshInfo.SkeletonRoot = MakeString(Link ? Link->GetName() : ("None"));
				MeshInfo.SkeletonElemNum = Link ? Link->GetChildCount(true) : 0;

				if (Link)
				{
					FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
					Link->GetAnimationInterval(AnimTimeSpan);
					GlobalTimeSpan.UnionAssignment(AnimTimeSpan);
				}
			}
			else
			{
				SceneInfo.NonSkinnedMeshNum++;
				MeshInfo.bIsSkelMesh = false;
				MeshInfo.SkeletonRoot = NULL;
			}
			MeshInfo.UniqueId = Mesh->GetUniqueID();
		}
	}

	SceneInfo.bHasAnimation = false;
	int32 AnimCurveNodeCount = Scene->GetSrcObjectCount<FbxAnimCurveNode>();
	// sadly Max export with animation curve node by default without any change, so 
	// we'll have to skip the first two curves, which is translation/rotation
	// if there is a valid animation, we'd expect there are more curve nodes than 2. 
	for (int32 AnimCurveNodeIndex = 2; AnimCurveNodeIndex < AnimCurveNodeCount; AnimCurveNodeIndex++)
	{
		FbxAnimCurveNode* CurAnimCruveNode = Scene->GetSrcObject<FbxAnimCurveNode>(AnimCurveNodeIndex);
		if (CurAnimCruveNode->IsAnimated(true))
		{
			SceneInfo.bHasAnimation = true;
			break;
		}
	}

	SceneInfo.FrameRate = FbxTime::GetFrameRate(Scene->GetGlobalSettings().GetTimeMode());

	if (GlobalTimeSpan.GetDirection() == FBXSDK_TIME_FORWARD)
	{
		SceneInfo.TotalTime = (GlobalTimeSpan.GetDuration().GetMilliSeconds()) / 1000.f * SceneInfo.FrameRate;
	}
	else
	{
		SceneInfo.TotalTime = 0;
	}

	FbxNode* RootNode = Scene->GetRootNode();
	YFbxNodeInfo RootInfo;
	RootInfo.ObjectName = MakeName(RootNode->GetName());
	RootInfo.UniqueId = RootNode->GetUniqueID();
	RootInfo.Transform = RootNode->EvaluateGlobalTransform();

	RootInfo.AttributeName = NULL;
	RootInfo.AttributeUniqueId = 0;
	RootInfo.AttributeType = NULL;

	RootInfo.ParentName = NULL;
	RootInfo.ParentUniqueId = 0;

	//Add the rootnode to the SceneInfo
	SceneInfo.HierarchyInfo.Add(RootInfo);
	//Fill the hierarchy info
	TraverseHierarchyNodeRecursively(SceneInfo, RootNode, RootInfo);
	return true;
}

FbxNode* YFbxConverter::RecursiveFindParentLodGroup(FbxNode* pParentNode)
{
	if (pParentNode == nullptr)
	{
		return nullptr;
	}
	if (pParentNode->GetNodeAttribute() && pParentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
	{
		return pParentNode;
	}
	return RecursiveFindParentLodGroup(pParentNode->GetParent());
}

FbxNode* YFbxConverter::FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind /*= nullptr*/)
{
	check(NodeLodGroup);
	check(NodeLodGroup->GetNodeAttribute() && NodeLodGroup->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup);
	check(NodeLodGroup->GetChildCount() >= LodIndex);
	FbxNode* ChildNode = NodeLodGroup->GetChild(LodIndex);
	return RecursiveGetFirstMeshNode(ChildNode, NodeToFind);
}

FbxNode * YFbxConverter::RecursiveGetFirstMeshNode(FbxNode* Node, FbxNode* NodeToFind /*= nullptr*/)
{
	if (Node->GetMesh() != nullptr)
		return Node;
	for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
	{
		FbxNode* MeshNode = RecursiveGetFirstMeshNode(Node->GetChild(ChildIndex), NodeToFind);
		if (NodeToFind == nullptr)
		{
			if (MeshNode != nullptr)
			{
				return MeshNode;
			}
		}
		else if (MeshNode == NodeToFind)
		{
			return MeshNode;
		}
	}
	return nullptr;
}

void YFbxConverter::TraverseHierarchyNodeRecursively(YFbxSceneInfo& SceneInfo, FbxNode *ParentNode, YFbxNodeInfo &ParentInfo)
{
	int32 NodeCount = ParentNode->GetChildCount();
	for (int32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		FbxNode* ChildNode = ParentNode->GetChild(NodeIndex);
		YFbxNodeInfo ChildInfo;
		ChildInfo.ObjectName = MakeName(ChildNode->GetName());
		ChildInfo.UniqueId = ChildNode->GetUniqueID();
		ChildInfo.ParentName = ParentInfo.ObjectName;
		ChildInfo.ParentUniqueId = ParentInfo.UniqueId;
		ChildInfo.RotationPivot = ChildNode->RotationPivot.Get();
		ChildInfo.ScalePivot = ChildNode->ScalingPivot.Get();
		ChildInfo.Transform = ChildNode->EvaluateLocalTransform();
		if (ChildNode->GetNodeAttribute())
		{
			FbxNodeAttribute *ChildAttribute = ChildNode->GetNodeAttribute();
			ChildInfo.AttributeUniqueId = ChildAttribute->GetUniqueID();
			if (ChildAttribute->GetName()[0] != '\0')
			{
				ChildInfo.AttributeName = MakeName(ChildAttribute->GetName());
			}
			else
			{
				//Get the name of the first node that link this attribute
				ChildInfo.AttributeName = MakeName(ChildAttribute->GetNode()->GetName());
			}

			switch (ChildAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::eUnknown:
				ChildInfo.AttributeType = "eUnknown";
				break;
			case FbxNodeAttribute::eNull:
				ChildInfo.AttributeType = "eNull";
				break;
			case FbxNodeAttribute::eMarker:
				ChildInfo.AttributeType = "eMarker";
				break;
			case FbxNodeAttribute::eSkeleton:
				ChildInfo.AttributeType = "eSkeleton";
				break;
			case FbxNodeAttribute::eMesh:
				ChildInfo.AttributeType = "eMesh";
				break;
			case FbxNodeAttribute::eNurbs:
				ChildInfo.AttributeType = "eNurbs";
				break;
			case FbxNodeAttribute::ePatch:
				ChildInfo.AttributeType = "ePatch";
				break;
			case FbxNodeAttribute::eCamera:
				ChildInfo.AttributeType = "eCamera";
				break;
			case FbxNodeAttribute::eCameraStereo:
				ChildInfo.AttributeType = "eCameraStereo";
				break;
			case FbxNodeAttribute::eCameraSwitcher:
				ChildInfo.AttributeType = "eCameraSwitcher";
				break;
			case FbxNodeAttribute::eLight:
				ChildInfo.AttributeType = "eLight";
				break;
			case FbxNodeAttribute::eOpticalReference:
				ChildInfo.AttributeType = "eOpticalReference";
				break;
			case FbxNodeAttribute::eOpticalMarker:
				ChildInfo.AttributeType = "eOpticalMarker";
				break;
			case FbxNodeAttribute::eNurbsCurve:
				ChildInfo.AttributeType = "eNurbsCurve";
				break;
			case FbxNodeAttribute::eTrimNurbsSurface:
				ChildInfo.AttributeType = "eTrimNurbsSurface";
				break;
			case FbxNodeAttribute::eBoundary:
				ChildInfo.AttributeType = "eBoundary";
				break;
			case FbxNodeAttribute::eNurbsSurface:
				ChildInfo.AttributeType = "eNurbsSurface";
				break;
			case FbxNodeAttribute::eShape:
				ChildInfo.AttributeType = "eShape";
				break;
			case FbxNodeAttribute::eLODGroup:
				ChildInfo.AttributeType = "eLODGroup";
				break;
			case FbxNodeAttribute::eSubDiv:
				ChildInfo.AttributeType = "eSubDiv";
				break;
			case FbxNodeAttribute::eCachedEffect:
				ChildInfo.AttributeType = "eCachedEffect";
				break;
			case FbxNodeAttribute::eLine:
				ChildInfo.AttributeType = "eLine";
				break;
			}
		}
		else
		{
			ChildInfo.AttributeType = "eNull";
			ChildInfo.AttributeName = NULL;
		}

		SceneInfo.HierarchyInfo.Add(ChildInfo);
		TraverseHierarchyNodeRecursively(SceneInfo, ChildNode, ChildInfo);
	}
}

ANSICHAR* YFbxConverter::MakeName(const ANSICHAR* Name)
{
	const int SpecialChars[] = { '.', ',', '/', '`', '%' };

	const int len = FCStringAnsi::Strlen(Name);
	ANSICHAR* TmpName = new ANSICHAR[len + 1];

	FCStringAnsi::Strcpy(TmpName, len + 1, Name);

	for (int32 i = 0; i < ARRAY_COUNT(SpecialChars); i++)
	{
		ANSICHAR* CharPtr = TmpName;
		while ((CharPtr = FCStringAnsi::Strchr(CharPtr, SpecialChars[i])) != NULL)
		{
			CharPtr[0] = '_';
		}
	}

	// Remove namespaces
	ANSICHAR* NewName;
	NewName = FCStringAnsi::Strchr(TmpName, ':');

	// there may be multiple namespace, so find the last ':'
	while (NewName && FCStringAnsi::Strchr(NewName + 1, ':'))
	{
		NewName = FCStringAnsi::Strchr(NewName + 1, ':');
	}

	if (NewName)
	{
		return NewName + 1;
	}

	return TmpName;
}

FString YFbxConverter::MakeString(const ANSICHAR* Name)
{
	return FString(ANSI_TO_TCHAR(Name));
}

static void DebugRootInfo(FbxNode* pRoot)
{
	if (!pRoot)
		return;
	//UE_LOG(LogYFbxConverter, Log, TEXT("Fbx")
	FbxVector4 vecLRL = pRoot->EvaluateLocalRotation();
	FbxAMatrix GlobalMat=  pRoot->EvaluateGlobalTransform();
	FbxVector4 preRotation = pRoot->GetPreRotation(FbxNode::EPivotSet::eSourcePivot);
	FbxVector4 vecScale = pRoot->EvaluateLocalScaling();
}
bool YFbxConverter::ConvertScene()
{
	FbxNode* pRootNode = Scene->GetRootNode();
	DebugRootInfo(pRootNode);
	DebugRootInfo(pRootNode->GetChild(0));
	DebugRootInfo(pRootNode->GetChild(0)->GetChild(0));
	FbxAxisSystem SourceSetup = Scene->GetGlobalSettings().GetAxisSystem();
	if (SourceSetup != FbxAxisSystem::eMayaYUp)
	{
		if (SourceSetup == FbxAxisSystem::eMax)
		{
			UE_LOG(LogYFbxConverter, Log, TEXT("Fbx file source Axis is 3ds max Z-up"));
		}
		FbxRootNodeUtility::RemoveAllFbxRoots(Scene);
		FbxAxisSystem ConvertToSystem(FbxAxisSystem::eMayaYUp);
		ConvertToSystem.ConvertScene(Scene);
		pRootNode = Scene->GetRootNode();
		DebugRootInfo(pRootNode);
		DebugRootInfo(pRootNode->GetChild(0));
		DebugRootInfo(pRootNode->GetChild(0)->GetChild(0));
	}

	FbxSystemUnit SceneSystemUnit = Scene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit != FbxSystemUnit::cm)
	{
		if (SceneSystemUnit == FbxSystemUnit::m)
		{
			UE_LOG(LogYFbxConverter, Log, TEXT("Fbx file source unit is m"));
		}
		else if (SceneSystemUnit == FbxSystemUnit::Inch)
		{
			UE_LOG(LogYFbxConverter, Log, TEXT("Fbx file source unit is inch"));
		}
		FbxSystemUnit::cm.ConvertScene(Scene);
		pRootNode = Scene->GetRootNode();
		DebugRootInfo(pRootNode);
		DebugRootInfo(pRootNode->GetChild(0));
		DebugRootInfo(pRootNode->GetChild(0)->GetChild(0));
	}

	return true;
}

int32 YFbxConverter::GetFbxMeshCount(FbxNode * Node, bool bCountLODs, int32 & OutNumLODGroups)
{
	bool bLODGroup = Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup;
	if (bLODGroup)
	{
		++OutNumLODGroups;
	}
	int32 MeshCount = 0;
	if (!bLODGroup || bCountLODs) // 当前节点不是eLOD节点，或者不引入LOD
	{
		if (Node->GetMesh())
		{
			MeshCount = 1;
		}

		for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
		{
			MeshCount += GetFbxMeshCount(Node->GetChild(ChildIndex), bCountLODs, OutNumLODGroups);
		}
	}
	else // 当前节点是eLODGroup 并且 要引入LOD
	{
		MeshCount = 1;
	}
	return  MeshCount;
}

void YFbxConverter::FillFbxMeshArray(FbxNode * Node, TArray<FbxNode*>& outMeshArray)
{
	if (Node->GetMesh())
	{
		if (Node->GetMesh()->GetPolygonVertexCount > 0)
		{
			outMeshArray.Add(Node);
		}
	}

	for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
	{
		FillFbxMeshArray(Node->GetChild(ChildIndex), outMeshArray);
	}
}

UStaticMesh * YFbxConverter::ImportStaticMeshAsSingle(TArray<FbxNode*>& MeshNodeArray, const FName InName, UStaticMesh * InStaticMesh, int LODIndex, void * ExistMeshDataPtr)
{
	if (MeshNodeArray.Num() == 0)
	{
		return nullptr;
	}
	int32 NumVerts = 0;
	for (int32 MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); MeshIndex++)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];
		FbxMesh* FbxMesh = Node->GetMesh();
		if (FbxMesh)
		{
			NumVerts += FbxMesh->GetControlPointsCount();
			if (!ImportOptions->bCombineToSingle)
			{
				NumVerts = 0;
			}
		}
	}
	FString MeshName = InName.ToString();
	for (int32 MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); MeshIndex++)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];
		FbxMesh* FbxMesh = Node->GetMesh();
		FbxLayer* LayerSmoothing = FbxMesh->GetLayer(0, FbxLayerElement::eSmoothing);
		if (!LayerSmoothing && !GIsAutomationTesting)
		{
			UE_LOG(LogYFbxConverter, Log, TEXT("Prompt_NoSmoothgroupForFBXScene", "No smoothing group information was found in this FBX scene.  Please make sure to enable the 'Export Smoothing Groups' option in the FBX Exporter plug-in before exporting the file.  Even for tools that don't support smoothing groups, the FBX Exporter will generate appropriate smoothing data at export-time so that correct vertex normals can be inferred while importing."));
		}
		UStaticMesh* StaticMesh = new UStaticMesh();
	}
}
