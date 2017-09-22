#include "FbxImporter.h"
#include "Misc\Paths.h"
#include "Misc\FbxErrors.h"
DEFINE_LOG_CATEGORY(LogFbx)
#define LOCTEXT_NAMESPACE "FbxMainImport"
using namespace UnFbx;
FVector UnFbx::FFbxDataConverter::ConvertPos(FbxVector4 Vector)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = -Vector[2];
	return Out;
}

FVector UnFbx::FFbxDataConverter::ConvertDir(FbxVector4 Vector)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = -Vector[2];
	return Out;
}

FRotator UnFbx::FFbxDataConverter::ConvertEuler(FbxDouble3 Euler)
{
	return FRotator::MakeFromEuler(FVector(-Euler[0], -Euler[1], Euler[2]));
}

FVector UnFbx::FFbxDataConverter::ConvertScale(FbxDouble3 Vector)
{
	FVector Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = Vector[2];
	return Out;
}
FVector UnFbx::FFbxDataConverter::ConvertScale(FbxVector4 Vector)
{

	FVector Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = Vector[2];
	return Out;
}

FRotator UnFbx::FFbxDataConverter::ConvertRotation(FbxQuaternion Quaternion)
{
	FRotator Out(ConvertRotToQuat(Quaternion));
	return Out;
}

FVector UnFbx::FFbxDataConverter::ConvertRotationToFVect(FbxQuaternion Quaternion, bool bInvertOrient)
{
	FQuat UnrealQuaternion = ConvertRotToQuat(Quaternion);
	FVector Euler;
	Euler = UnrealQuaternion.Euler();
	if (bInvertOrient)
	{
		check(0);
		// 没弄明白反转的是什么
		Euler.Y = -Euler.Y;
		Euler.Z = 180.f + Euler.Z;
	}
	return Euler;
}

FQuat UnFbx::FFbxDataConverter::ConvertRotToQuat(FbxQuaternion Quaternion)
{
	FQuat UnrealQuat;
	UnrealQuat.X = -Quaternion[0];
	UnrealQuat.Y = -Quaternion[1];
	UnrealQuat.Z = Quaternion[2];
	UnrealQuat.W = -Quaternion[3];

	return UnrealQuat;
}

float UnFbx::FFbxDataConverter::ConvertDist(FbxDouble Distance)
{
	float Out;
	Out = (float)Distance;
	return Out;
}

bool UnFbx::FFbxDataConverter::ConvertPropertyValue(FbxProperty& FbxProperty, UProperty& UnrealProperty, union UPropertyValue& OutUnrealPropertyValue)
{
	return true;
}

FTransform UnFbx::FFbxDataConverter::ConvertTransform(FbxAMatrix Matrix)
{
	FTransform Out;

	FQuat Rotation = ConvertRotToQuat(Matrix.GetQ());
	FVector Origin = ConvertPos(Matrix.GetT());
	FVector Scale = ConvertScale(Matrix.GetS());

	Out.SetTranslation(Origin);
	Out.SetScale3D(Scale);
	Out.SetRotation(Rotation);

	return Out;
}

FMatrix UnFbx::FFbxDataConverter::ConvertMatrix(FbxAMatrix Matrix)
{
	FMatrix UEMatrix;
	check(0);//没看明白为什么
	for (int i = 0; i < 4; ++i)
	{
		FbxVector4 Row = Matrix.GetRow(i);
		if (i == 1)
		{
			UEMatrix.M[i][0] = -Row[0];
			UEMatrix.M[i][1] = Row[1];
			UEMatrix.M[i][2] = -Row[2];
			UEMatrix.M[i][3] = -Row[3];
		}
		else
		{
			UEMatrix.M[i][0] = Row[0];
			UEMatrix.M[i][1] = -Row[1];
			UEMatrix.M[i][2] = Row[2];
			UEMatrix.M[i][3] = Row[3];
		}
	}

	return UEMatrix;
}

FColor UnFbx::FFbxDataConverter::ConvertColor(FbxDouble3 Color)
{
	//Fbx is in linear color space
	FColor SRGBColor = FLinearColor(Color[0], Color[1], Color[2]).ToFColor(true);
	return SRGBColor;
}

FbxVector4 UnFbx::FFbxDataConverter::ConvertToFbxPos(FVector Vector)
{
	FbxVector4 Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = -Vector[2];

	return Out;
}

FbxVector4 UnFbx::FFbxDataConverter::ConvertToFbxRot(FVector Vector)
{
	FbxVector4 Out;
	Out[0] = -Vector[0];
	Out[1] = -Vector[1];
	Out[2] = Vector[2];

	return Out;
}

FbxVector4 UnFbx::FFbxDataConverter::ConvertToFbxScale(FVector Vector)
{
	FbxVector4 Out;
	Out[0] = Vector[0];
	Out[1] = Vector[1];
	Out[2] = Vector[2];

	return Out;
}

FbxDouble3 UnFbx::FFbxDataConverter::ConvertToFbxColor(FColor Color)
{
	//Fbx is in linear color space
	FLinearColor FbxLinearColor(Color);
	FbxDouble3 Out;
	Out[0] = FbxLinearColor.R;
	Out[1] = FbxLinearColor.G;
	Out[2] = FbxLinearColor.B;

	return Out;
}

FbxString UnFbx::FFbxDataConverter::ConvertToFbxString(FName Name)
{
	FbxString OutString;

	FString UnrealString;
	Name.ToString(UnrealString);

	OutString = TCHAR_TO_UTF8(*UnrealString);

	return OutString;
}
FFbxImporter::FFbxImporter()
	: Scene(NULL)
	, ImportOptions(NULL)
	, GeometryConverter(NULL)
	, SdkManager(NULL)
	, Importer(NULL)
	, bFirstMesh(true)
	, Logger(NULL)
{
	// Create the SdkManager
	SdkManager = FbxManager::Create();

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(ios);

	// Create the geometry converter
	GeometryConverter = new FbxGeometryConverter(SdkManager);
	Scene = NULL;

	ImportOptions = new FBXImportOptions();
	FMemory::Memzero(*ImportOptions);
	ImportOptions->MaterialBasePath = NAME_None;

	CurPhase = NOTSTARTED;
}
FFbxImporter::~FFbxImporter()
{
	CleanUp();
}

FbxAMatrix UnFbx::FFbxDataConverter::JointPostConversionMatrix;

FbxString UnFbx::FFbxDataConverter::ConvertToFbxString(const FString& String)
{
	FbxString OutString;

	OutString = TCHAR_TO_UTF8(*String);

	return OutString;
}

TSharedPtr<FFbxImporter> FFbxImporter::StaticInstance;

FFbxImporter* FFbxImporter::GetInstance()
{
	if (!StaticInstance.IsValid())
	{
		StaticInstance = MakeShareable(new FFbxImporter());
	}
	return StaticInstance.Get();
}

void FFbxImporter::DeleteInstance()
{
	StaticInstance.Reset();
}
void FFbxImporter::CleanUp()
{
	ClearTokenizedErrorMessages();
	ReleaseScene();

	delete GeometryConverter;
	GeometryConverter = NULL;
	delete ImportOptions;
	ImportOptions = NULL;

	if (SdkManager)
	{
		SdkManager->Destroy();
	}
	SdkManager = NULL;
	Logger = NULL;
}
void FFbxImporter::ClearTokenizedErrorMessages()
{
	if (Logger)
	{
		Logger->TokenizedErrorMessages.Empty();
	}
}

void FFbxImporter::ReleaseScene()
{
	if (Importer)
	{
		Importer->Destroy();
		Importer = NULL;
	}

	if (Scene)
	{
		Scene->Destroy();
		Scene = NULL;
	}

	ImportedMaterialData.Clear();

	// reset
	CollisionModels.Clear();
	CurPhase = NOTSTARTED;
	bFirstMesh = true;
	LastMergeBonesChoice = EAppReturnType::Ok;
}

void FImportedMaterialData::Clear()
{
	FbxToUnrealMaterialMap.Empty();
	ImportedMaterialNames.Empty();
}

bool FFbxImporter::OpenFile(FString Filename, bool bParseStatistics, bool bForSceneInfo)
{
	bool Result = true;

	if (CurPhase != NOTSTARTED)
	{
		// something went wrong
		return false;
	}

	//GWarn->BeginSlowTask(LOCTEXT("OpeningFile", "Reading File"), true);
	//GWarn->StatusForceUpdate(20, 100, LOCTEXT("OpeningFile", "Reading File"));

	int32 SDKMajor, SDKMinor, SDKRevision;

	// Create an importer.
	Importer = FbxImporter::Create(SdkManager, "");

	// Get the version number of the FBX files generated by the
	// version of FBX SDK that you are using.
	FbxManager::GetFileFormatVersion(SDKMajor, SDKMinor, SDKRevision);

	// Initialize the importer by providing a filename.
	if (bParseStatistics)
	{
		Importer->ParseForStatistics(true);
	}

	const bool bImportStatus = Importer->Initialize(TCHAR_TO_UTF8(*Filename));

	//GWarn->StatusForceUpdate(100, 100, LOCTEXT("OpeningFile", "Reading File"));
	//GWarn->EndSlowTask();
	if (!bImportStatus)  // Problem with the file to be imported
	{
		UE_LOG(LogFbx, Error, TEXT("Call to FbxImporter::Initialize() failed."));
		UE_LOG(LogFbx, Warning, TEXT("Error returned: %s"), UTF8_TO_TCHAR(Importer->GetStatus().GetErrorString()));

		if (Importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			UE_LOG(LogFbx, Warning, TEXT("FBX version number for this FBX SDK is %d.%d.%d"),
				SDKMajor, SDKMinor, SDKRevision);
		}

		return false;
	}

	// Skip the version check if we are just parsing for information or scene info.
	if (!bParseStatistics && !bForSceneInfo)
	{
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

			const FText WarningText = FText::Format(
				NSLOCTEXT("UnrealEd", "Warning_OutOfDateFBX", "An out of date FBX has been detected.\nImporting different versions of FBX files than the SDK version can cause undesirable results.\n\nFile Version: {0}\nSDK Version: {1}"),
				FText::FromString(FileVerStr), FText::FromString(SDKVerStr));

		}
	}

	//Cache the current file hash
	Md5Hash = FMD5Hash::HashFile(*Filename);

	CurPhase = FILEOPENED;
	// Destroy the importer
	//Importer->Destroy();

	return Result;
}

int32 FFbxImporter::GetImportType(const FString& InFilename)
{
	int32 Result = -1; // Default to invalid
	FString Filename = InFilename;

	// Prioritized in the order of SkeletalMesh > StaticMesh > Animation (only if animation data is found)
	if (OpenFile(Filename, true))
	{
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
			UE_LOG(LogFbx, Log, TEXT("ItemName: %s, ItemCount : %d"), *NameBuffer, ItemCount);
		}

		FbxSceneInfo SceneInfo;
		if (GetSceneInfo(Filename, SceneInfo, true))
		{
			if (SceneInfo.SkinnedMeshNum > 0)
			{
				Result = 1;
			}
			else if (SceneInfo.TotalGeometryNum > 0)
			{
				Result = 0;
			}

			bHasAnimation = SceneInfo.bHasAnimation;
		}

		if (Importer)
		{
			Importer->Destroy();
		}

		Importer = NULL;
		CurPhase = NOTSTARTED;

		// In case no Geometry was found, check for animation (FBX can still contain mesh data though)
		if (bHasAnimation)
		{
			if (Result == -1)
			{
				Result = 2;
			}
			// by default detects as skeletalmesh since it has animation curves
			else if (Result == 0)
			{
				Result = 1;
			}
		}
	}

	return Result;
}

void FFbxImporter::FixMaterialClashName()
{
	FbxArray<FbxSurfaceMaterial*> MaterialArray;
	Scene->FillMaterialArray(MaterialArray);
	TSet<FString> AllMaterialName;
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialArray.Size(); ++MaterialIndex)
	{
		FbxSurfaceMaterial *Material = MaterialArray[MaterialIndex];
		FString MaterialName = UTF8_TO_TCHAR(Material->GetName());
		if (AllMaterialName.Contains(MaterialName))
		{
			FString OriginalMaterialName = MaterialName;
			//Use the fbx nameclash 1 convention: NAMECLASH1_KEY
			//This will add _ncl1_
			FString MaterialBaseName = MaterialName + TEXT(NAMECLASH1_KEY);
			int32 NameIndex = 1;
			MaterialName = MaterialBaseName + FString::FromInt(NameIndex++);
			while (AllMaterialName.Contains(MaterialName))
			{
				MaterialName = MaterialBaseName + FString::FromInt(NameIndex++);
			}
			//Rename the Material
			Material->SetName(TCHAR_TO_UTF8(*MaterialName));
			if (!GIsAutomationTesting)
			{
				AddTokenizedErrorMessage(
					FTokenizedMessage::Create(EMessageSeverity::Warning,
						FText::Format(LOCTEXT("FbxImport_MaterialNameClash", "FBX Scene Loading: Found material name clash, name clash can be wrongly reassign at reimport , material '{0}' was rename '{1}'"), FText::FromString(OriginalMaterialName), FText::FromString(MaterialName))),
					FFbxErrors::Generic_LoadingSceneFailed);
			}
		}
		AllMaterialName.Add(MaterialName);
	}
}

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(SdkManager->GetIOSettings()))
#endif

bool FFbxImporter::ImportFile(FString Filename, bool bPreventMaterialNameClash /*=false*/)
{
	bool Result = true;

	bool bStatus;

	FileBasePath = FPaths::GetPath(Filename);

	// Create the Scene
	Scene = FbxScene::Create(SdkManager, "");
	UE_LOG(LogFbx, Log, TEXT("Loading FBX Scene from %s"), *Filename);

	int32 FileMajor, FileMinor, FileRevision;

	IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
	IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
	IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
	IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
	IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
	IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
	IOS_REF.SetBoolProp(IMP_SKINS, true);
	IOS_REF.SetBoolProp(IMP_DEFORMATION, true);
	IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	IOS_REF.SetBoolProp(IMP_TAKE, true);

	// Import the scene.
	bStatus = Importer->Import(Scene);

	//Make sure we don't have name clash for materials
	if (bPreventMaterialNameClash)
	{
		FixMaterialClashName();
	}

	// Get the version number of the FBX file format.
	Importer->GetFileVersion(FileMajor, FileMinor, FileRevision);

	// output result
	if (bStatus)
	{
		UE_LOG(LogFbx, Log, TEXT("FBX Scene Loaded Succesfully"));
		CurPhase = IMPORTED;

		// Release importer now as it is unneeded
		Importer->Destroy();
		Importer = NULL;
	}
	else
	{
		ErrorMessage = UTF8_TO_TCHAR(Importer->GetStatus().GetErrorString());
		AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("FbxSkeletaLMeshimport_FileLoadingFailed", "FBX Scene Loading Failed : '{0}'"), FText::FromString(ErrorMessage))), FFbxErrors::Generic_LoadingSceneFailed);
		// ReleaseScene will also release the importer if it was initialized
		ReleaseScene();
		Result = false;
		CurPhase = NOTSTARTED;
	}

	return Result;
}


bool FFbxImporter::GetSceneInfo(FString Filename, FbxSceneInfo& SceneInfo, bool bPreventMaterialNameClash /*= false*/)
{
	bool Result = true;

	bool bSceneInfo = true;
	switch (CurPhase)
	{
	case NOTSTARTED:
		if (!OpenFile(Filename, false, bSceneInfo))
		{
			Result = false;
			break;
		}
	case FILEOPENED:
		if (!ImportFile(Filename, bPreventMaterialNameClash))
		{
			Result = false;
			break;
		}
	case IMPORTED:

	default:
		break;
	}

	if (Result)
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
							continue;
						}
					}
				}
				SceneInfo.TotalGeometryNum++;

				SceneInfo.MeshInfo.AddZeroed(1);
				FbxMeshInfo& MeshInfo = SceneInfo.MeshInfo.Last();
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
		FbxNodeInfo RootInfo;
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
	}

	return Result;
}
void FFbxImporter::TraverseHierarchyNodeRecursively(FbxSceneInfo& SceneInfo, FbxNode *ParentNode, FbxNodeInfo &ParentInfo)
{
	int32 NodeCount = ParentNode->GetChildCount();
	for (int32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		FbxNode* ChildNode = ParentNode->GetChild(NodeIndex);
		FbxNodeInfo ChildInfo;
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

FbxNode* FFbxImporter::FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind)
{
	check(NodeLodGroup->GetChildCount() >= LodIndex);
	FbxNode *ChildNode = NodeLodGroup->GetChild(LodIndex);

	return RecursiveGetFirstMeshNode(ChildNode, NodeToFind);
}


FbxNode *FFbxImporter::RecursiveGetFirstMeshNode(FbxNode* Node, FbxNode* NodeToFind)
{
	if (Node->GetMesh() != nullptr)
		return Node;
	for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
	{
		FbxNode *MeshNode = RecursiveGetFirstMeshNode(Node->GetChild(ChildIndex), NodeToFind);
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

void UnFbx::FFbxImporter::AddTokenizedErrorMessage(TSharedRef<FTokenizedMessage> Error, FName FbxErrorName)
{
	// check to see if Logger exists, this way, we guarantee only prints to FBX import
	// when we meant to print
	if (Logger)
	{
		Logger->TokenizedErrorMessages.Add(Error);

		if (FbxErrorName != NAME_None)
		{
			Error->AddToken(FFbxErrorToken::Create(FbxErrorName));
		}
	}
	else
	{
		// if not found, use normal log
		UE_LOG(LogFbx, Warning, TEXT("%s"), *(Error->ToText().ToString()));
	}
}

FbxNode *FFbxImporter::RecursiveFindParentLodGroup(FbxNode *ParentNode)
{
	if (ParentNode == nullptr)
		return nullptr;
	if (ParentNode->GetNodeAttribute() && ParentNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
		return ParentNode;
	return RecursiveFindParentLodGroup(ParentNode->GetParent());
}


ANSICHAR* FFbxImporter::MakeName(const ANSICHAR* Name)
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

FString FFbxImporter::MakeString(const ANSICHAR* Name)
{
	return FString(ANSI_TO_TCHAR(Name));
}

FName FFbxImporter::MakeNameForMesh(FString InName, FbxObject* FbxObject)
{
	FName OutputName;

	//Cant name the mesh if the object is null and there InName arguments is None.
	check(FbxObject != nullptr || InName != TEXT("None"))

		if ((ImportOptions->bUsedAsFullName || FbxObject == nullptr) && InName != TEXT("None"))
		{
			OutputName = *InName;
		}
		else
		{
			check(FbxObject);

			char Name[MAX_SPRINTF];
			int SpecialChars[] = { '.', ',', '/', '`', '%' };

			FCStringAnsi::Sprintf(Name, "%s", FbxObject->GetName());

			for (int32 i = 0; i < 5; i++)
			{
				char* CharPtr = Name;
				while ((CharPtr = FCStringAnsi::Strchr(CharPtr, SpecialChars[i])) != nullptr)
				{
					CharPtr[0] = '_';
				}
			}

			// for mesh, replace ':' with '_' because Unreal doesn't support ':' in mesh name
			char* NewName = nullptr;
			NewName = FCStringAnsi::Strchr(Name, ':');

			if (NewName)
			{
				char* Tmp;
				Tmp = NewName;
				while (Tmp)
				{

					// Always remove namespaces
					NewName = Tmp + 1;

					// there may be multiple namespace, so find the last ':'
					Tmp = FCStringAnsi::Strchr(NewName + 1, ':');
				}
			}
			else
			{
				NewName = Name;
			}

			int32 NameCount = 0;
			FString ComposeName;
			do
			{
				if (InName == FString("None"))
				{
					ComposeName = FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(NewName));
				}
				else
				{
					ComposeName = FString::Printf(TEXT("%s_%s"), *InName, UTF8_TO_TCHAR(NewName));
				}
				if (NameCount > 0)
				{
					ComposeName += TEXT("_") + FString::FromInt(NameCount);
				}
				NameCount++;
			} while (MeshNamesCache.Contains(ComposeName));
			OutputName = FName(*ComposeName);
		}

	MeshNamesCache.Add(OutputName.ToString());
	return OutputName;
}
void UnFbx::FFbxImporter::FlushToTokenizedErrorMessage(EMessageSeverity::Type Severity)
{
	if (!ErrorMessage.IsEmpty())
	{
		AddTokenizedErrorMessage(FTokenizedMessage::Create(Severity, FText::Format(FText::FromString("{0}"), FText::FromString(ErrorMessage))), NAME_None);
	}
}

void UnFbx::FFbxImporter::SetLogger(class FFbxLogger * InLogger)
{
	// this should be only called by top level functions
	// if you set it you can't set it again. Otherwise, you'll lose all log information
	check(Logger == NULL);
	Logger = InLogger;
}

// just in case if DeleteScene/CleanUp is getting called too late
void UnFbx::FFbxImporter::ClearLogger()
{
	Logger = NULL;
}

FFbxLogger::FFbxLogger()
{
}

FFbxLogger::~FFbxLogger()
{
	bool ShowLogMessage = !ShowLogMessageOnlyIfError;
	if (ShowLogMessageOnlyIfError)
	{
		for (TSharedRef<FTokenizedMessage> TokenMessage : TokenizedErrorMessages)
		{
			if (TokenMessage->GetSeverity() == EMessageSeverity::CriticalError || TokenMessage->GetSeverity() == EMessageSeverity::Error)
			{
				ShowLogMessage = true;
				break;
			}
		}
	}

	//Always clear the old message after an import or re-import
	const TCHAR* LogTitle = TEXT("FBXImport");
	//FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	//TSharedPtr<class IMessageLogListing> LogListing = MessageLogModule.GetLogListing(LogTitle);
	//LogListing->SetLabel(FText::FromString("FBX Import"));
	//LogListing->ClearMessages();

	//if (TokenizedErrorMessages.Num() > 0)
	//{
	//	LogListing->AddMessages(TokenizedErrorMessages);
	//	if (ShowLogMessage)
	//	{
	//		MessageLogModule.OpenMessageLog(LogTitle);
	//	}
	//}
}
#undef LOCTEXT_NAMESPACE