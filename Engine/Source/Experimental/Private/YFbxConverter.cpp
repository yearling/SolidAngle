#include "YFbxConverter.h"
#include "Logging\LogMacros.h"
#include "fbxsdk\core\fbxmanager.h"
#include "fbxsdk\scene\geometry\fbxmesh.h"
#include "fbxsdk\scene\fbxaxissystem.h"
#include "fbxsdk\scene\geometry\fbxlayer.h"
#include "FbxSkeletalMeshImportData.h"
#include "RawMesh.h"
#include "StaticMesh.h"
#include "YTexture.h"
#include "YMaterial.h"
#include "YRawMesh.h"
#include "FbxConvertUtilites.h"

DEFINE_LOG_CATEGORY(LogYFbxConverter);
static const int32 LARGE_MESH_MATERIAL_INDEX_THRESHOLD = 64;
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
	FileBasePath = FPaths::GetPath(Filename);
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
		else if (SceneInfo.TotalGeometryNum > 0)
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
				if (FbxMeshArray.Num() > 0)
				{
					ImportStaticMeshAsSingle(FbxMeshArray, "StaticMesh", nullptr, 0, nullptr);
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
	FbxAMatrix GlobalMat = pRoot->EvaluateGlobalTransform();
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
		if (Node->GetMesh()->GetPolygonVertexCount() > 0)
		{
			outMeshArray.Add(Node);
		}
	}

	for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
	{
		FillFbxMeshArray(Node->GetChild(ChildIndex), outMeshArray);
	}
}

UStaticMesh * YFbxConverter::ImportStaticMeshAsSingle(TArray<FbxNode*>& MeshNodeArray, const FName InName, TRefCountPtr<YStaticMesh>InStaticMesh, int LODIndex, void * ExistMeshDataPtr)
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
	bool bBuildStatus = true;
	for (int32 MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); MeshIndex++)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];
		FbxMesh* FbxMesh = Node->GetMesh();
		FbxLayer* LayerSmoothing = FbxMesh->GetLayer(0, FbxLayerElement::eSmoothing);
		if (!LayerSmoothing)
		{
			UE_LOG(LogYFbxConverter, Log, TEXT("Prompt_NoSmoothgroupForFBXScene , No smoothing group information was found in this FBX scene.  Please make sure to enable the 'Export Smoothing Groups' option in the FBX Exporter plug-in before exporting the file.  Even for tools that don't support smoothing groups, the FBX Exporter will generate appropriate smoothing data at export-time so that correct vertex normals can be inferred while importing."));
		}
	}
	TRefCountPtr<YStaticMesh> StaticMesh(new YStaticMesh());
	if (StaticMesh->SourceModels.Num() < LODIndex + 1)
	{
		new(StaticMesh->SourceModels) YStaticMeshSourceModel();
		if (StaticMesh->SourceModels.Num() < LODIndex + 1)
		{
			//有可能进来的lod顺序不对
			LODIndex = StaticMesh->SourceModels.Num() - 1;
		}
	}
	YStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
	StaticMesh->LightingGuid = FGuid::NewGuid();
	StaticMesh->LightMapResolution = 64;
	StaticMesh->LightMapCoordinateIndex = 1;
	YRawMesh NewRawMesh;
	SrcModel.RawMeshBulkData->LoadRawMesh(NewRawMesh);
	TArray< YFbxMaterial> MeshMaterials;

	for (int32 MeshIndex = 0; MeshIndex < MeshNodeArray.Num(); ++MeshIndex)
	{
		FbxNode* Node = MeshNodeArray[MeshIndex];
		if (Node->GetMesh())
		{
			if (!BuildStaticMeshFromGeometry(Node, StaticMesh, MeshMaterials, LODIndex, NewRawMesh, ImportOptions->VertexColorImportOption, ImportOptions->VertexOverrideColor))
			{
				bBuildStatus = false;
				break;
			}
		}
	}
	SrcModel.RawMeshBulkData->SaveRawMesh(NewRawMesh);

	if (bBuildStatus)
	{
		UE_LOG(LogYFbxConverter, Verbose, TEXT("==Initial material list:"));
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			UE_LOG(LogYFbxConverter, Verbose, TEXT("%d: %s"), MaterialIndex, *MeshMaterials[MaterialIndex].GetName());
		}
		// Compress the material array by removing any duplicates.
		// 第一步，先压缩Fbx对应的材质，比如Mesh0和Mesh1都引用了同一个FBXMaterial，但是由于Mesh0与Mesh1位于不同的FbxNode下，其材质节点的引入是独立的，所以要合并一下
		bool bDoRemap = false;
		TArray<int32> MaterialMap; // odd index to new index
		TArray<YFbxMaterial> UniqueMaterials;
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			bool bUnique = true;
			for (int32 OtherMaterialIndex = MaterialIndex - 1; OtherMaterialIndex >= 0; --OtherMaterialIndex)
			{
				if (MeshMaterials[MaterialIndex].FbxMaterial == MeshMaterials[OtherMaterialIndex].FbxMaterial)
				{
					int32 UniqueIndex = MaterialMap[OtherMaterialIndex];
					MaterialMap.Add(UniqueIndex);
					bDoRemap = true;
					bUnique = false;
					break;
				}
			}
			if (bUnique)
			{
				int32 UniqueIndex = UniqueMaterials.Add(MeshMaterials[MaterialIndex]);
				MaterialMap.Add(UniqueIndex);
			}
			else
			{
				UE_LOG(LogYFbxConverter, Verbose, TEXT(" remap %d -> %d"), MaterialIndex, MaterialMap[MaterialIndex]);
			}
		}
		//UniqueMaterials 中存的是 YFbxMaterials 
		//MaterialMap中存的是 index: 原来的材质索引（NewRawMesh.FaceMaterialIndices)中存的索引， value： 重定向的索引，UniqueMaterials[value]才是对应的YFbxMaterial
		if (UniqueMaterials.Num() > LARGE_MESH_MATERIAL_INDEX_THRESHOLD)
		{
			UE_LOG(LogYFbxConverter, Warning, TEXT("StaticMesh has a large number %d materials and may render inefficently. Consider breaking up the mesh into muliple Static Mesh Asset"), UniqueMaterials.Num());
		}
		//The fix is required for blender file. The static mesh build have change and now required that
		//the sections (face declaration) must be declare in the same order as the material index
		//重排材质的顺序，按mesh引用的材质的顺序来
		TArray<uint32> SortedMaterialIndex;
		TArray<int32> UsedMaterials; //哪些材质被使用了，因为材质是统一引进的，也就是说FbxMaterial没有跟mesh一一对应（有些mesh没有引入，但是材质引入了），有可能有些FbxMaterial引进了但是没有被Mesh使用，也就是说这一步的时候，材质比所需要的材质多。
		for (int32 FaceMaterialIndex = 0; FaceMaterialIndex < NewRawMesh.FaceMaterialIndices.Num(); ++FaceMaterialIndex)
		{
			int32 MaterialIndex = NewRawMesh.FaceMaterialIndices[FaceMaterialIndex];
			if (!UsedMaterials.Contains(MaterialIndex))
			{
				int32 NewIndex = UsedMaterials.Add(MaterialIndex);
				if (NewIndex != MaterialIndex)
				{
					bDoRemap = true;
				}
			}
		}
		// UsedMaterials里存的是按Face引用顺序的MaterialIndex
		for (int32 MaterialIndex = 0; MaterialIndex < MeshMaterials.Num(); ++MaterialIndex)
		{
			int32 SkinIndex = 0xffff;
			if (bDoRemap)
			{
				int32 UsedIndex = 0;
				if (UsedMaterials.Find(MaterialIndex, UsedIndex))
				{
					SkinIndex = UsedIndex;// 如果该Face引用的material比较靠前，那么排序用的index就比较小
				}
			}
			//SkinIndex 如果没有Face引用，则其值比较大，排到靠后
			int32 RemappedIndex = MaterialMap[MaterialIndex];
			uint32 SortedMaterialKey = ((uint32)SkinIndex << 16) | ((uint32)RemappedIndex & 0xffff);
			if (!SortedMaterialIndex.IsValidIndex(SortedMaterialKey)) // 没看懂
			{
				SortedMaterialIndex.Add(SortedMaterialKey);
			}
		}

		SortedMaterialIndex.Sort(); 
		UE_LOG(LogYFbxConverter, Verbose, TEXT("== After sorting:"));
		TArray<YFbxMaterial> SortedMaterials;
		for (int32 SortedIndex = 0; SortedIndex < SortedMaterialIndex.Num(); ++SortedIndex)
		{
			int32 RemmappedIndex = SortedMaterialIndex[SortedIndex] & 0xffff;
			SortedMaterials.Add(UniqueMaterials[RemmappedIndex]);
			UE_LOG(LogYFbxConverter, Verbose, TEXT("%d: %s"), SortedIndex, *UniqueMaterials[RemmappedIndex].GetName());
		}
		// SortedMaterials存的是重排序后的YFbxMaterials
		UE_LOG(LogYFbxConverter, Verbose, TEXT("== Mapping table:"));
		// 接下来更新一个MaterialMap，原来的MaterialMap是材质索引到UniqueMaterial，要更新为材质索引到SortedMaterials
		for (int32 MaterialIndex = 0; MaterialIndex < MaterialMap.Num(); ++MaterialIndex)
		{
			for (int32 SortedIndex = 0; SortedIndex < SortedMaterialIndex.Num(); ++SortedIndex)
			{
				int32 RemmappedIndex = SortedMaterialIndex[SortedIndex] & 0xffff;
				if (MaterialMap[MaterialIndex] == RemmappedIndex)
				{
					MaterialMap[MaterialIndex] = SortedIndex;
					UE_LOG(LogYFbxConverter, Verbose, TEXT(" sort %d -> %d "), MaterialIndex, SortedIndex);
					break;
				}
			}
		}
		// Remap material indices in RawMesh
		int32 MaxMaterialIndex = 0;
		int32 FirstOpenUVChannel = 1;
		{
			YRawMesh LocalRawMesh;
			SrcModel.RawMeshBulkData->LoadRawMesh(LocalRawMesh);
			if (bDoRemap)
			{
				for (int32 TriIndex = 0; TriIndex < LocalRawMesh.FaceMaterialIndices.Num(); ++TriIndex)
				{
					LocalRawMesh.FaceMaterialIndices[TriIndex] = MaterialMap[LocalRawMesh.FaceMaterialIndices[TriIndex]];
				}
			}

			// Compact material indices so that we won't have any sections with zero triangles.
			LocalRawMesh.CompactMaterialIndices();

			if (LocalRawMesh.MaterialIndexToImportIndex.Num() > 0)
			{
				TArray<YFbxMaterial> OldSortedMaterials;
				Exchange(OldSortedMaterials, SortedMaterials);
				SortedMaterials.Empty(LocalRawMesh.MaterialIndexToImportIndex.Num());
				for (int32 MaterialIndex = 0; MaterialIndex < LocalRawMesh.MaterialIndexToImportIndex.Num(); ++MaterialIndex)
				{
					YFbxMaterial Material;
					int32 ImportIndex = LocalRawMesh.MaterialIndexToImportIndex[MaterialIndex];
					if (OldSortedMaterials.IsValidIndex(ImportIndex))
					{
						Material = OldSortedMaterials[ImportIndex];
					}
					SortedMaterials.Add(Material);
				}
			}

			for (int32 TriIndex = 0; TriIndex < LocalRawMesh.FaceMaterialIndices.Num(); ++TriIndex)
			{
				MaxMaterialIndex = FMath::Max<int32>(MaxMaterialIndex, LocalRawMesh.FaceMaterialIndices[TriIndex]);
			}
			
			for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; i++)
			{
				if (LocalRawMesh.WedgeTexCoords[i].Num() == 0)
				{
					FirstOpenUVChannel = i;
					break;
				}
			}
			SrcModel.RawMeshBulkData->SaveRawMesh(LocalRawMesh);
		}

		// Setup per-section info and the materials array.
		if (LODIndex == 0)
		{
			StaticMesh->StaticMaterials.Empty();
		}

		int32 NumMaterials = FMath::Min(SortedMaterials.Num(), MaxMaterialIndex + 1);
		for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
		{
			YMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, MaterialIndex);
			FName MaterialFName = FName(*(SortedMaterials[MaterialIndex].GetName()));
			int32 Index = StaticMesh->StaticMaterials.Add(YStaticMaterial(SortedMaterials[MaterialIndex].Material, MaterialFName, MaterialFName));
			Info.MaterialIndex = Index;
			StaticMesh->SectionInfoMap.Remove(LODIndex, MaterialIndex);
			StaticMesh->SectionInfoMap.Set(LODIndex, MaterialIndex, Info);
		}

		YRawMesh LocalRawMesh;
		SrcModel.RawMeshBulkData->LoadRawMesh(LocalRawMesh);
		SrcModel.BuildSettings.bRemoveDegenerates = ImportOptions->bRemoveDegenerates;
		SrcModel.BuildSettings.bBuildAdjacencyBuffer = ImportOptions->bBuildAdjacencyBuffer;
		SrcModel.BuildSettings.bBuildReversedIndexBuffer = ImportOptions->bBuildReversedIndexBuffer;
		SrcModel.BuildSettings.bRecomputeNormals = ImportOptions->NormalGenerationMethod == EYFBXNormalImportMethod::YFBXNIM_ComputeNormals;
		SrcModel.BuildSettings.bRecomputeTangents = ImportOptions->NormalGenerationMethod != EYFBXNormalImportMethod::YFBXNIM_ImportNormalsAndTangents;
		SrcModel.BuildSettings.bUseMikkTSpace = (ImportOptions->NormalGenerationMethod == EYFBXNormalGenerationMethod::MikkTSpace) && (!ImportOptions->ShouldImportNormals() || !ImportOptions->ShouldImportTangents());
		if (ImportOptions->bGenerateLightmapUVs)
		{
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.DstLightmapIndex = FirstOpenUVChannel;
			StaticMesh->LightMapCoordinateIndex = FirstOpenUVChannel;
		}
		else
		{
			SrcModel.BuildSettings.bGenerateLightmapUVs = false;
		}


	}

	return nullptr;
}

struct YFBXUVs
{
	YFBXUVs(FbxMesh* Mesh)
		:UniqueUVCount(0)
	{
		check(Mesh);
		int LayerCount = Mesh->GetLayerCount();
		if (LayerCount > 0)
		{
			//	store the UVs in arrays for fast access in the later looping of triangles 
		//
		// mapping from UVSets to Fbx LayerElementUV
		// Fbx UVSets may be duplicated, remove the duplicated UVSets in the mapping 
			for (int32 UVLayerIndex = 0; UVLayerIndex < LayerCount; ++UVLayerIndex)
			{
				FbxLayer* lLayer = Mesh->GetLayer(UVLayerIndex);
				int UVSetCount = lLayer->GetUVSetCount();
				if (UVSetCount)
				{
					FbxArray<const FbxLayerElementUV*> EleUVs = lLayer->GetUVSets();
					for (int UVIndex = 0; UVIndex < UVSetCount; ++UVIndex)
					{
						FbxLayerElementUV const * ElementUV = EleUVs[UVIndex];
						if (ElementUV)
						{
							const char* UVSetName = ElementUV->GetName();
							FString LocalUVSetName = UTF8_TO_TCHAR(UVSetName);
							if (LocalUVSetName.IsEmpty())
							{
								LocalUVSetName = TEXT("UVmap_") + FString::FromInt(UVLayerIndex);
							}

							UVSets.AddUnique(LocalUVSetName);
						}
					}
				}
			}
		}


		// If the the UV sets are named using the following format (UVChannel_X; where X ranges from 1 to 4)
		// we will re-order them based on these names.  Any UV sets that do not follow this naming convention
		// will be slotted into available spaces.

		if (UVSets.Num())
		{
			for (int32 ChannelNumIndex = 0; ChannelNumIndex < 4; ++ChannelNumIndex)
			{
				FString ChannelName = FString::Printf(TEXT("UVChannel_%d"), ChannelNumIndex + 1);
				int32 SetIndex = UVSets.Find(ChannelName);
				if (SetIndex != INDEX_NONE && SetIndex != ChannelNumIndex)
				{
					//为了防止指定的channel的slot为空， 采用的方法就是先把所有的slot填满，
					for (int32 Arrsize = UVSets.Num(); Arrsize < ChannelNumIndex + 1; ++Arrsize)
					{
						UVSets.Add(FString(TEXT("")));
					}
					UVSets.Swap(SetIndex, ChannelNumIndex);
				}
			}
		}
	}

	void Phase2(FbxMesh* Mesh)
	{
		//	store the UVs in arrays for fast access in the later looping of triangles 
		UniqueUVCount = UVSets.Num();
		if (UniqueUVCount > 0)
		{
			LayerElementUV.AddZeroed(UniqueUVCount);
			UVReferenceMode.AddZeroed(UniqueUVCount);
			UVMappingMode.AddZeroed(UniqueUVCount);
		}
		for (int32 UVIndex = 0; UVIndex < UniqueUVCount; ++UVIndex)
		{
			LayerElementUV[UVIndex] = nullptr;
			for (int32 UVLayerIndex = 0; UVLayerIndex < Mesh->GetLayerCount(); ++UVLayerIndex)
			{
				FbxLayer* lLayer = Mesh->GetLayer(UVLayerIndex);
				int UVSetCount = lLayer->GetUVSetCount();
				if (UVSetCount)
				{
					FbxArray<const FbxLayerElementUV*> EleUVs = lLayer->GetUVSets();
					for (int FbxUVIndex = 0; FbxUVIndex < UVSetCount; ++FbxUVIndex)
					{
						FbxLayerElementUV const * ElementUV = EleUVs[FbxUVIndex];
						if (ElementUV)
						{
							const char* UVSetName = ElementUV->GetName();
							FString LocalUVSetName = UTF8_TO_TCHAR(UVSetName);
							if (LocalUVSetName.IsEmpty())
							{
								LocalUVSetName = TEXT("UVmap_") + FString::FromInt(UVLayerIndex);
							}

							if (LocalUVSetName == UVSets[UVIndex])
							{
								LayerElementUV[UVIndex] = ElementUV;
								UVReferenceMode[UVIndex] = ElementUV->GetReferenceMode();
								UVMappingMode[UVIndex] = ElementUV->GetMappingMode();
								break;
							}
						}
					}
				}
			}
		}
		//限制到4层
		UniqueUVCount = FMath::Min<int32>(UniqueUVCount, MAX_MESH_TEXTURE_COORDS);
	}

	int32 FindLightUVIndex() const
	{
		for (int32 UVSetIndex = 0; UVSetIndex < UVSets.Num(); ++UVSetIndex)
		{
			if (UVSets[UVSetIndex] == TEXT("LightMapUV"))
			{
				return UVSetIndex;
			}
		}
		return INDEX_NONE;
	}

	int32 ComputeUVIndex(int32 UVLayerIndex, int32 lControlPointIndex, int32 FaceCornerIndex) const
	{
		int32 UVMapIndex = (UVMappingMode[UVLayerIndex] == FbxLayerElement::eByControlPoint) ? lControlPointIndex : FaceCornerIndex;
		int32 Ret;
		if (UVReferenceMode[UVLayerIndex] == FbxLayerElement::eDirect)
		{
			Ret = UVMapIndex;
		}
		else
		{
			FbxLayerElementArrayTemplate<int>& Array = LayerElementUV[UVLayerIndex]->GetIndexArray();
			Ret = Array.GetAt(UVMapIndex);
		}
		return Ret;
	}
	TArray<FString> UVSets;
	TArray<FbxLayerElementUV const*> LayerElementUV;
	TArray<FbxLayerElement::EReferenceMode> UVReferenceMode;
	TArray<FbxLayerElement::EMappingMode> UVMappingMode;
	int32 UniqueUVCount;
};

bool YFbxConverter::BuildStaticMeshFromGeometry(FbxNode* Node, TRefCountPtr<YStaticMesh> StaticMesh, TArray<YFbxMaterial>& MeshMaterials, int32 LODIndex, YRawMesh& RawMesh, EYVertexColorImportOption::Type VertexColorImportOption, const FColor& VertexOverrideColor)
{
	check(StaticMesh->SourceModels.IsValidIndex(LODIndex));
	FbxMesh* Mesh = Node->GetMesh();
	YStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
	Mesh->RemoveBadPolygons();

	FbxLayer* BaseLayer = Mesh->GetLayer(0);
	if (BaseLayer == nullptr)
	{
		UE_LOG(LogYFbxConverter, Error, TEXT("There is no geometry information in mesh %s"), Mesh->GetName());
		return false;
	}
	YFBXUVs FBXUVs(Mesh);
	int32 FBXNamedLightMapCoordinateIndex = FBXUVs.FindLightUVIndex();
	if (FBXNamedLightMapCoordinateIndex != INDEX_NONE)
	{
		StaticMesh->LightMapCoordinateIndex = FBXNamedLightMapCoordinateIndex;
	}

	int32 MaterialCount = 0;
	TArray<YMaterialInterface*> Materials;
	if (ImportOptions->bImportMaterials)
	{
		CreateNodeMaterials(Node, Materials, FBXUVs.UVSets);
	}
	MaterialCount = Node->GetMaterialCount();
	check(!ImportOptions->bImportMaterials || Materials.Num() == MaterialCount);
	int32 MaterialIndexOffset = MeshMaterials.Num();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		YFbxMaterial* NewMaterial = new(MeshMaterials) YFbxMaterial;
		FbxSurfaceMaterial* FbxMaterial = Node->GetMaterial(MaterialIndex);
		NewMaterial->FbxMaterial = FbxMaterial;
		if (ImportOptions->bImportMaterials)
		{
			NewMaterial->Material = Materials[MaterialIndex];
		}
	}
	// Must do this before triangulating the mesh due to an FBX bug in TriangulateMeshAdvance

	int32 LayerSmoothingCount = Mesh->GetLayerCount(FbxLayerElement::eSmoothing);
	for (int32 i = 0; i < LayerSmoothingCount; ++i)
	{
		FbxLayerElementSmoothing const* SmoothingInfo = Mesh->GetLayer(0)->GetSmoothing();
		if (SmoothingInfo && SmoothingInfo->GetMappingMode() != FbxLayerElement::eByPolygon)
		{
			GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh, i);
		}
	}
	if (!Mesh->IsTriangleMesh())
	{
		UE_LOG(LogYFbxConverter, Warning, TEXT("Triangulating static mesh %s"), UTF8_TO_TCHAR(Node->GetName()));
		const bool bReplace = true;
		FbxNodeAttribute* ConvertedNode = GeometryConverter->Triangulate(Mesh, bReplace);
		if (ConvertedNode != nullptr && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Mesh = (fbxsdk::FbxMesh*)ConvertedNode;
		}
		else
		{
			UE_LOG(LogYFbxConverter, Warning, TEXT("Triangulating static mesh %s can not trianguate"), UTF8_TO_TCHAR(Node->GetName()));
			return false;
		}
	}

	//renew the base layer
	BaseLayer = Mesh->GetLayer(0);

	//	get the "material index" layer.  Do this AFTER the triangulation step as that may reorder material indices
	FbxLayerElementMaterial* LayerElementMaterial = BaseLayer->GetMaterials();
	FbxLayerElement::EMappingMode MaterialMappingModel = LayerElementMaterial ? LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;

	FBXUVs.Phase2(Mesh);

	//get smooth group layer 
	bool bSmoothingAvailable = false;
	FbxLayerElementSmoothing const* SmoothingInfo = BaseLayer->GetSmoothing();
	FbxLayerElement::EReferenceMode SmoothingReferenceModel(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode SmoothingMappingModel(FbxLayerElement::eByEdge);
	if (SmoothingInfo)
	{
		if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
		{
			bSmoothingAvailable = true;
		}
		SmoothingMappingModel = SmoothingInfo->GetMappingMode();
		SmoothingReferenceModel = SmoothingInfo->GetReferenceMode();
	}

	// get the first vertex coloer layer 
	FbxLayerElementVertexColor* LayerElementVertexColor = BaseLayer->GetVertexColors();
	FbxLayerElement::EReferenceMode VertexColorReferceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementVertexColor)
	{
		VertexColorReferceMode = LayerElementVertexColor->GetReferenceMode();
		VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();
	}

	// get the first normal layer
	FbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
	FbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
	FbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

	//where there is normal, tangent and binormal data in this mesh
	bool bHasNTBInformation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

	FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementNormal)
	{
		NormalReferenceMode = LayerElementNormal->GetReferenceMode();
		NormalMappingMode = LayerElementNormal->GetMappingMode();
	}

	FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementTangent)
	{
		TangentReferenceMode = LayerElementTangent->GetReferenceMode();
		TangentMappingMode = LayerElementTangent->GetMappingMode();
	}

	FbxLayerElement::EReferenceMode BinormalReferenceMode(FbxLayerElement::eDirect);
	FbxLayerElement::EMappingMode BinormalMappingMode(FbxLayerElement::eByControlPoint);
	if (LayerElementBinormal)
	{
		BinormalReferenceMode = LayerElementBinormal->GetReferenceMode();
		BinormalMappingMode = LayerElementBinormal->GetMappingMode();
	}
	bool bImportedCollision = true;
	bool bEnableCollision = bImportedCollision || (/*GBuildStaticMeshCollision &&*/ LODIndex == 0 && ImportOptions->bRemoveDegenerates);

	for (int32 SectionIndex = MaterialIndexOffset; SectionIndex < MaterialIndexOffset + MaterialCount; ++SectionIndex)
	{
		YMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, SectionIndex);
		Info.bEnableCollision = bEnableCollision;
		StaticMesh->SectionInfoMap.Set(LODIndex, SectionIndex, Info);
	}

	FbxAMatrix TotalMatrix;
	FbxAMatrix TotalMatrixForNormal;
	TotalMatrix = ComputeTotalMatrix(Node);
	TotalMatrixForNormal = TotalMatrix.Inverse();
	TotalMatrixForNormal = TotalMatrixForNormal.Transpose();

	int32 TriangleCount = Mesh->GetPolygonCount();
	if (TriangleCount == 0)
	{
		UE_LOG(LogYFbxConverter, Error, TEXT("No Triangle were foudn on mesh %s "), UTF8_TO_TCHAR(Mesh->GetName()));
		return false;
	}

	int32 VertexCount = Mesh->GetControlPointsCount();
	int32 WedgeCount = TriangleCount * 3;
	bool OddNegtiveScale = IsOddNegativeScale(TotalMatrix);

	int32 VertexOffset = RawMesh.VertexPositions.Num();
	int32 WedgeOffset = RawMesh.WedgeIndices.Num();
	int32 TriangleOffset = RawMesh.FaceMaterialIndices.Num();
	int32 MaxMaterialIndex = 0;

	// Reserve space for attributes.
	RawMesh.FaceMaterialIndices.AddZeroed(TriangleCount);
	RawMesh.FaceSmoothingMasks.AddZeroed(TriangleCount);
	RawMesh.WedgeIndices.AddZeroed(WedgeCount);

	if (bHasNTBInformation || RawMesh.WedgeTangentX.Num() > 0 || RawMesh.WedgeTangentY.Num() > 0)
	{
		RawMesh.WedgeTangentX.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentX.Num());
		RawMesh.WedgeTangentY.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentY.Num());
	}

	if (LayerElementNormal || RawMesh.WedgeTangentZ.Num() > 0)
	{
		check(WedgeOffset == RawMesh.WedgeTangentZ.Num());
		RawMesh.WedgeTangentZ.AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTangentZ.Num());
	}

	if (LayerElementVertexColor || VertexColorImportOption != EYVertexColorImportOption::Replace || RawMesh.WedgeColors.Num())
	{
		int32 NumNewColors = WedgeOffset + WedgeCount - RawMesh.WedgeColors.Num();
		int32 FirstNewColor = RawMesh.WedgeColors.Num();
		RawMesh.WedgeColors.AddUninitialized(NumNewColors);
		for (int32 WedgeIndex = FirstNewColor; WedgeIndex < FirstNewColor + NumNewColors; ++WedgeIndex)
		{
			RawMesh.WedgeColors[WedgeIndex] = FColor::White;
		}
	}

	// When importing multiple mesh pieces to the same static mesh.  Ensure each mesh piece has the same number of Uv's
	int32 ExistingUVCount = 0;
	for (int32 ExistingUVIndex = 0; ExistingUVIndex < MAX_TEXTURE_COORDS; ++ExistingUVIndex)
	{
		if (RawMesh.WedgeTexCoords[ExistingUVIndex].Num() > 0)
		{
			++ExistingUVCount;
		}
		else
		{
			break;
		}
	}
	int32 UVCount = FMath::Max(FBXUVs.UniqueUVCount, ExistingUVCount);
	// At least one UV set must exist.  
	UVCount = FMath::Max(1, UVCount);
	for (int32 UVLayerIndex = 0; UVLayerIndex < UVCount; ++UVLayerIndex)
	{
		RawMesh.WedgeTexCoords[UVLayerIndex].AddZeroed(WedgeOffset + WedgeCount - RawMesh.WedgeTexCoords[UVLayerIndex].Num());
	}

	int32 TriangleIndex;
	TMap<int32, int32> IndexMap; // 因为顶点数组没有扩展，所以要保存一份当前Mesh的每个顶点index到整个staticMesh的顶点index. 注意：没有扩展顶点
	bool bHasNonDegenrateTriangles = false;
	for (TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex)
	{
		int32 DestTriangleIndex = TriangleOffset + TriangleIndex;
		FVector CornerPosition[3];
		for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
		{
			// If there are odd number negative scale, invert the vertex order for triangles
			int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + (OddNegtiveScale ? 2 - CornerIndex : CornerIndex);
			// Store vertex index and position.
			int32 ControlPointIndex = Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);
			int32 TriangleCornerIndex = TriangleIndex * 3 + CornerIndex;
			int32* ExistingIndex = IndexMap.Find(ControlPointIndex);
			if (ExistingIndex)
			{
				RawMesh.WedgeIndices[WedgeIndex] = *ExistingIndex;
				CornerPosition[CornerIndex] = RawMesh.VertexPositions[*ExistingIndex];
			}
			else
			{
				FbxVector4 FbxPosition = Mesh->GetControlPoints()[ControlPointIndex];
				FbxVector4 FinialPostion = TotalMatrix.MultT(FbxPosition);
				int32 VertexIndex = RawMesh.VertexPositions.Add(YFbxDataConverter::ConvertPos(FinialPostion));
				RawMesh.WedgeIndices[WedgeIndex] = VertexIndex;
				IndexMap.Add(ControlPointIndex, VertexIndex);
				CornerPosition[CornerIndex] = RawMesh.VertexPositions[VertexIndex];
			}

			if (LayerElementNormal)
			{
				//normals may have different reference and mapping mode than tangents and binormals
				int32 NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleCornerIndex;
				int32 NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ? NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);
				FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
				TempValue = TotalMatrixForNormal.MultT(TempValue);
				FVector TangentZ = YFbxDataConverter::ConvertDir(TempValue);
				RawMesh.WedgeTangentZ[WedgeIndex] = TangentZ.GetSafeNormal();

				//tangents and binormals share the same reference, mapping mode and index array
				if (bHasNTBInformation)
				{
					int32 TangentMapIndex = (TangentMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleCornerIndex;
					int32 TangentValueIndex = (TangentReferenceMode == FbxLayerElement::eDirect) ? TangentMapIndex : LayerElementTangent->GetIndexArray().GetAt(TangentMapIndex);
					FbxVector4 TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentX = YFbxDataConverter::ConvertDir(TempValue);
					RawMesh.WedgeTangentX[WedgeIndex] = TangentX.GetSafeNormal();

					int32 BinormalMapIndex = (BinormalMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleCornerIndex;
					int32 BinormalValueIndex = (BinormalReferenceMode == FbxLayerElement::eDirect) ? BinormalMapIndex : LayerElementBinormal->GetIndexArray().GetAt(BinormalMapIndex);
					TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
					TempValue = TotalMatrixForNormal.MultT(TempValue);
					FVector TangentY = -YFbxDataConverter::ConvertDir(TempValue); //LHS
					RawMesh.WedgeTangentY[WedgeIndex] = TangentY.GetSafeNormal();
				}
			}

			// vertex color
			if (VertexColorImportOption == EYVertexColorImportOption::Replace)
			{
				if (LayerElementVertexColor)
				{
					int32 VertexColorMappingIndex = (VertexColorMappingMode == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleCornerIndex;
					int32 VertexColorIndex = (VertexColorReferceMode == FbxLayerElement::eDirect) ? VertexColorMappingIndex : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIndex);
					FbxColor VertexColor = LayerElementVertexColor->GetDirectArray().GetAt(VertexColorIndex);
					RawMesh.WedgeColors[WedgeIndex] = FColor(uint8(255.0f*VertexColor.mRed), uint8(255.0f*VertexColor.mGreen), uint8(255.0f*VertexColor.mBlue), uint8(255.0f*VertexColor.mAlpha));
				}
			}
			else if (VertexColorImportOption == EYVertexColorImportOption::Ignore)
			{
				// try to match this triangles current vertex with one that existed in the previous mesh.
				// This is a find in a tmap which uses a fast hash table lookup.
			}
			else
			{
				// set the triangle's vertex color to a constant override
				check(VertexColorImportOption == EVertexColorImportOption::Override);
				RawMesh.WedgeColors[WedgeIndex] = VertexOverrideColor;
			}
		}
		// Check if the triangle just discovered is non-degenerate if we haven't found one yet
		if (!bHasNonDegenrateTriangles)
		{
			float ComparisionThreshold = ImportOptions->bRemoveDegenerates ? THRESH_POINTS_ARE_SAME : 0.0f;
			if (!(CornerPosition[0].Equals(CornerPosition[1], ComparisionThreshold) || CornerPosition[0].Equals(CornerPosition[2], ComparisionThreshold) || CornerPosition[1].Equals(CornerPosition[2], ComparisionThreshold)))
			{
				bHasNonDegenrateTriangles = true;
			}
		}

		//smooth mask
		if (bSmoothingAvailable && SmoothingInfo)
		{
			if (SmoothingMappingModel == FbxLayerElement::eByPolygon)
			{
				int lSmoothingIndex = (SmoothingReferenceModel == FbxLayerElement::eDirect) ? TriangleIndex : SmoothingInfo->GetIndexArray().GetAt(TriangleIndex);
				RawMesh.FaceSmoothingMasks[DestTriangleIndex] = SmoothingInfo->GetDirectArray().GetAt(lSmoothingIndex);
			}
			else
			{
				UE_LOG(LogYFbxConverter, Warning, TEXT("Unsupported smoothing group mapping mode on mesh %s"), UTF8_TO_TCHAR(Mesh->GetName()));
			}
		}

		//
		// uvs
		//
		// In FBX file, the same UV may be saved multiple times, i.e., there may be same UV in LayerElementUV
		// So we don't import the duplicate UVs
		int32 UVLayerIndex = 0;
		for (UVLayerIndex = 0; UVLayerIndex < FBXUVs.UniqueUVCount; ++UVLayerIndex)
		{
			if (FBXUVs.LayerElementUV[UVLayerIndex] != nullptr)
			{
				for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
				{
					// If there are odd number negative scale, invert the vertex order for triangles
					int32 WedgeIndex = WedgeOffset + TriangleIndex * 3 + (OddNegtiveScale ? 2 - CornerIndex : CornerIndex);
					int32 ControlPointIndex = Mesh->GetPolygonVertex(TriangleIndex, CornerIndex);
					int32 UVMapIndex = (FBXUVs.UVMappingMode[UVLayerIndex] == FbxLayerElement::eByControlPoint) ? ControlPointIndex : TriangleIndex * 3 + CornerIndex;
					int32 UVIndex = (FBXUVs.UVReferenceMode[UVLayerIndex] == FbxLayerElement::eDirect) ? UVMapIndex : FBXUVs.LayerElementUV[UVLayerIndex]->GetIndexArray().GetAt(UVMapIndex);
					FbxVector2 UVVector = FBXUVs.LayerElementUV[UVLayerIndex]->GetDirectArray().GetAt(UVIndex);
					RawMesh.WedgeTexCoords[UVLayerIndex][WedgeIndex].X = static_cast<float>(UVVector[0]);
					RawMesh.WedgeTexCoords[UVLayerIndex][WedgeIndex].Y = 1 - static_cast<float>(UVVector[1]);
				}
			}
		}

		//material index
		int32 MaterialIndex = 0;
		if (MaterialCount > 0)
		{
			if (LayerElementMaterial)
			{
				switch (MaterialMappingModel)
				{
				case fbxsdk::FbxLayerElement::eAllSame:
				{
					MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(0);
				}
				break;
				case fbxsdk::FbxLayerElement::eByPolygon:
				{
					MaterialIndex = LayerElementMaterial->GetIndexArray().GetAt(TriangleIndex);
				}
				break;
				default:
					break;
				}
			}
		}
		MaterialIndex += MaterialIndexOffset;
		if (MaterialIndex >= MaterialCount + MaterialIndexOffset || MaterialIndex < 0)
		{
			UE_LOG(LogYFbxConverter, Warning, TEXT("Face material index inconsistency - forcing to 0"));
			MaterialIndex = 0;
		}
		RawMesh.FaceMaterialIndices[DestTriangleIndex] = MaterialIndex;
	}
	bool bIsValidMesh = bHasNonDegenrateTriangles;

	return bIsValidMesh;
}

int32 YFbxConverter::CreateNodeMaterials(FbxNode* FbxNode, TArray<YMaterialInterface*>& outMaterials, TArray<FString>& UVSets)
{
	int32 MaterialCount = FbxNode->GetMaterialCount();
	TArray<FbxSurfaceMaterial*> UsedSurfaceMaterials;
	FbxMesh* MeshNode = FbxNode->GetMesh();
	TSet<int32> UsedMaterialIndexes;
	if (MeshNode)
	{
		for (int32 ElementMaterialIndex = 0; ElementMaterialIndex < MeshNode->GetElementMaterialCount(); ++ElementMaterialIndex)
		{
			FbxGeometryElementMaterial* ElementMaterial = MeshNode->GetElementMaterial(ElementMaterialIndex);
			switch (ElementMaterial->GetMappingMode())
			{
			case FbxLayerElement::eAllSame:
			{
				if (ElementMaterial->GetIndexArray().GetCount() > 0)
				{
					// todo: why not GetDirectArray()
					UsedMaterialIndexes.Add(ElementMaterial->GetIndexArray()[0]);
				}
			}
			break;
			case FbxLayerElement::eByPolygon:
			{
				for (int32 MaterialIndex = 0; MaterialIndex < ElementMaterial->GetIndexArray().GetCount(); ++MaterialIndex)
				{
					UsedMaterialIndexes.Add(ElementMaterial->GetIndexArray()[MaterialIndex]);
				}
			}
			break;
			default:
				break;
			}
		}
	}

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		//Create only the material used by the mesh element material
		if (MeshNode == nullptr || UsedMaterialIndexes.Contains(MaterialIndex))
		{
			FbxSurfaceMaterial* FbxMaterial = FbxNode->GetMaterial(MaterialIndex);
			if (FbxMaterial)
			{
				CreateMaterial(*FbxMaterial, outMaterials, UVSets);
			}
		}
		else
		{
			outMaterials.Add(nullptr);
		}
	}
	return MaterialCount;
}

void YFbxConverter::CreateMaterial(FbxSurfaceMaterial& FbxMaterial, TArray<YMaterialInterface*>& OutMaterials, TArray<FString>& UVSets)
{
	FString MaterialFullName = UTF8_TO_TCHAR(MakeName(FbxMaterial.GetName()));
	YMaterialInterface* pMaterial = new YMaterialInterface();
	pMaterial->MaterialName = FName(*MaterialFullName);
	CreateMaterialProperty(FbxMaterial, pMaterial, FbxSurfaceMaterial::sDiffuse, false, UVSets);
	CreateMaterialProperty(FbxMaterial, pMaterial, FbxSurfaceMaterial::sNormalMap, true, UVSets);
	OutMaterials.Add(pMaterial);
}

bool YFbxConverter::CreateMaterialProperty(FbxSurfaceMaterial& FbxMaterial, YMaterialInterface* UnrealMaterial, const char* MaterialProperty, bool bSetupAsNormalMap, TArray<FString>& UVSet)
{
	bool bCreated = false;
	FbxProperty FbxMaterialProperty = FbxMaterial.FindProperty(MaterialProperty);
	if (FbxMaterialProperty.IsValid())
	{
		int32 UnsupportedTextureCount = FbxMaterialProperty.GetSrcObjectCount<FbxLayeredTexture>();
		UnsupportedTextureCount += FbxMaterialProperty.GetSrcObjectCount<FbxProceduralTexture>();
		if (UnsupportedTextureCount > 0)
		{
			UE_LOG(LogYFbxConverter, Warning, TEXT("Layered or procedural Textures are not supported (material %s)"), UTF8_TO_TCHAR(FbxMaterial.GetName()));
		}
		else
		{
			int32 TextureCount = FbxMaterialProperty.GetSrcObjectCount<FbxTexture>();
			if (TextureCount > 0)
			{
				for (int32 TextureIndex = 0; TextureIndex < TextureCount; ++TextureIndex)
				{
					FbxFileTexture* FbxTexture = FbxMaterialProperty.GetSrcObject<FbxFileTexture>(TextureIndex);

					YTexture* Texture = ImportTexture(FbxTexture, bSetupAsNormalMap);
					if (Texture)
					{
						float ScaleU = FbxTexture->GetScaleU();
						float ScaleV = FbxTexture->GetScaleV();

						YTextureSampler tmpTextureSampler;
						tmpTextureSampler.SamplerType = bSetupAsNormalMap ? YTextureSampleType::YTEXUTRESAMPLE_NORMAL : YTextureSampleType::YTEXTURESAMPLE_COLOR;
						tmpTextureSampler.Texture = Texture;

						//add /find UVSet and set it to the texture
						FbxString UVSetName = FbxTexture->UVSet.Get();
						FString LocalUVSetName = UTF8_TO_TCHAR(UVSetName.Buffer());
						if (LocalUVSetName.IsEmpty())
						{
							LocalUVSetName = TEXT("UVMap_0");
						}
						int SetIndex = UVSet.Find(LocalUVSetName);
						if ((SetIndex != 0 && SetIndex != INDEX_NONE) || ScaleU != 1.0f || ScaleV != 1.0f)
						{
							tmpTextureSampler.UVIndex = (SetIndex >= 0) ? SetIndex : 0;
							tmpTextureSampler.ScalingU = ScaleU;
							tmpTextureSampler.ScalingV = ScaleV;
						}
						if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sDiffuse) == 0)
						{
							UnrealMaterial->DiffuseTexture = tmpTextureSampler;
						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sEmissive) == 0)
						{

						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sSpecular) == 0)
						{

						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sNormalMap) == 0)
						{
							UnrealMaterial->NormalTexture = tmpTextureSampler;
						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sBump) == 0)
						{
						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sTransparentColor) == 0)
						{

						}
						else if (FCStringAnsi::Strcmp(MaterialProperty, FbxSurfaceMaterial::sTransparencyFactor) == 0)
						{

						}
						bCreated = true;

					}
				}
			}
		}
	}
	return bCreated;
}

YTexture* YFbxConverter::ImportTexture(FbxFileTexture* FbxTexture, bool bSetupAsNormalMap)
{
	if (!FbxTexture)
	{
		return nullptr;
	}

	FString Filename1 = UTF8_TO_TCHAR(FbxTexture->GetFileName());
	FString Extension = FPaths::GetExtension(Filename1).ToLower();
	FString TextureName = FPaths::GetBaseFilename(Filename1);
	YTexture* Texture = nullptr;
	// try to open from absolute path
	FString FileName = Filename1;
	TArray<uint8> DataBinary;
	if (!FFileHelper::LoadFileToArray(DataBinary, *FileName))
	{
		// try fbx file base path + relative path
		FString Filename2 = FileBasePath / UTF8_TO_TCHAR(FbxTexture->GetRelativeFileName());
		FileName = Filename2;
		if (!FFileHelper::LoadFileToArray(DataBinary, *FileName))
		{
			// try fbx file base path + texture file name (no path)
			FString Filename3 = UTF8_TO_TCHAR(FbxTexture->GetRelativeFileName());
			FString FileOnly = FPaths::GetCleanFilename(Filename3);
			Filename3 = FileBasePath / FileOnly;
			FileName = Filename3;
			if (!FFileHelper::LoadFileToArray(DataBinary, *FileName))
			{
				UE_LOG(LogYFbxConverter, Warning, TEXT("Unable to find TEXTure file %s. Tried:\n - %s\n - %s\n - %s"), *FileOnly, *Filename1, *Filename2, *Filename3);
			}
		}
	}
	if (DataBinary.Num() > 0)
	{
		UE_LOG(LogYFbxConverter, Verbose, TEXT("Loading texture file %s"), *FileName);
		const uint8* PtrTexture = DataBinary.GetData();
		const TCHAR* TextureType = *Extension;
		if (bSetupAsNormalMap)
		{
		}

		Texture = new YTexture();
		Texture->FileName = FileName;
	}
	else
	{
		Texture = new YTexture();
		Texture->FileName = FileName;
	}
	return Texture;
}

FbxAMatrix YFbxConverter::ComputeTotalMatrix(FbxNode* Node)
{
	FbxAMatrix Geometry;
	FbxVector4 Translation, Rotation, Scaling;
	Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);
	//For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
	FbxAMatrix& GlobalTransform = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);
	//We can bake the pivot only if we don't transform the vertex to the absolute position
	if (!ImportOptions->bTransformVertexToAbsolute)
	{
		if (ImportOptions->bBakePivotInVertex)
		{
			FbxAMatrix PivotGeometry;
			FbxVector4 RotationPivot = Node->GetRotationPivot(FbxNode::eSourcePivot);
			FbxVector4 FullPivot;
			FullPivot[0] = -RotationPivot[0];
			FullPivot[1] = -RotationPivot[1];
			FullPivot[2] = -RotationPivot[2];
			PivotGeometry.SetT(FullPivot);
			Geometry = Geometry * PivotGeometry;
		}
		else
		{
			//No Vertex transform and no bake pivot, it will be the mesh as-is.
			Geometry.SetIdentity();
		}
	}
	//We must always add the geometric transform. Only Max use the geometric transform which is an offset to the local transform of the node
	FbxAMatrix TotalMatrix = ImportOptions->bTransformVertexToAbsolute ? GlobalTransform * Geometry : Geometry;
	return TotalMatrix;
}

bool YFbxConverter::IsOddNegativeScale(FbxAMatrix& TotalMatrix)
{
	FbxVector4 Scale = TotalMatrix.GetS();
	int32 NegtiveNum = 0;
	if (Scale[0] < 0)
	{
		NegtiveNum++;
	}
	if (Scale[1] < 0)
	{
		NegtiveNum++;
	}
	if (Scale[2] < 0)
	{
		NegtiveNum++;
	}
	return NegtiveNum == 1 || NegtiveNum == 3;
}
