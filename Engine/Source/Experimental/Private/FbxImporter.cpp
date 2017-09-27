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

void UnFbx::FFbxImporter::ValidateAllMeshesAreReferenceByNodeAttribute()
{
	for (int GeoIndex = 0; GeoIndex < Scene->GetGeometryCount(); ++GeoIndex)
	{
		bool FoundOneGeometryLinkToANode = false;
		FbxGeometry *Geometry = Scene->GetGeometry(GeoIndex);
		for (int NodeIndex = 0; NodeIndex < Scene->GetNodeCount(); ++NodeIndex)
		{
			FbxNode *SceneNode = Scene->GetNode(NodeIndex);
			FbxGeometry *NodeGeometry = static_cast<FbxGeometry*>(SceneNode->GetMesh());
			if (NodeGeometry && NodeGeometry->GetUniqueID() == Geometry->GetUniqueID())
			{
				FoundOneGeometryLinkToANode = true;
				break;
			}
		}
		if (!FoundOneGeometryLinkToANode)
		{
			FString GeometryName = (Geometry->GetName() && Geometry->GetName()[0] != '\0') ? UTF8_TO_TCHAR(Geometry->GetName()) : TEXT("[Geometry have no name]");
			AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning,
				FText::Format(LOCTEXT("FailedToImport_NoObjectLinkToNode", "Mesh {0} in the fbx file is not reference by any hierarchy node."), FText::FromString(GeometryName))),
				FFbxErrors::Generic_ImportingNewObjectFailed);
		}
	}
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

void UnFbx::FFbxImporter::MainImport(const FString & FileToImport, EFBXImportType MeshTypeToImport)
{
	if (!ImportFromFile(FileToImport, TEXT("FBX"), true))
	{
		// Log the error message and fail the import.
		//Warn->Log(ELogVerbosity::Error, FbxImporter->GetErrorMessage());
		UE_LOG(LogFbx, Error, TEXT("%s"), GetErrorMessage());
	}
	else
	{
		const TCHAR* errorMessage = GetErrorMessage();
		if (errorMessage[0] != '\0')
		{
			//Warn->Log(errorMessage);
			UE_LOG(LogFbx, Error, TEXT("%s"), GetErrorMessage());
		}

		FbxNode* RootNodeToImport = NULL;
		RootNodeToImport = Scene->GetRootNode();

		// For animation and static mesh we assume there is at lease one interesting node by default
		int32 InterestingNodeCount = 1;
		TArray< TArray<FbxNode*>* > SkelMeshArray;

		bool bImportStaticMeshLODs = ImportOptions->bImportStaticMeshLODs;
		bool bCombineMeshes = ImportOptions->bCombineToSingle;
		bool bCombineMeshesLOD = false;

		if (MeshTypeToImport == FBXIT_SkeletalMesh)
		{
			FillFbxSkelMeshArrayInScene(RootNodeToImport, SkelMeshArray, false);
			InterestingNodeCount = SkelMeshArray.Num();
		}
#if 0
		else if (MeshTypeToImport == FBXIT_StaticMesh)
		{
			ApplyTransformSettingsToFbxNode(RootNodeToImport, ImportUI->StaticMeshImportData);

			if (bCombineMeshes && !bImportStaticMeshLODs)
			{
				// If Combine meshes and dont import mesh LODs, the interesting node count should be 1 so all the meshes are grouped together into one static mesh
				InterestingNodeCount = 1;
			}
			else
			{
				// count meshes in lod groups if we dont care about importing LODs
				bool bCountLODGroupMeshes = !bImportStaticMeshLODs;
				int32 NumLODGroups = 0;
				InterestingNodeCount = FbxImporter->GetFbxMeshCount(RootNodeToImport, bCountLODGroupMeshes, NumLODGroups);

				// if there were LODs in the file, do not combine meshes even if requested
				if (bImportStaticMeshLODs && bCombineMeshes && NumLODGroups > 0)
				{
					bCombineMeshes = false;
					//Combine all the LOD together and export one mesh with LODs
					bCombineMeshesLOD = true;
				}
			}
		}


		if (InterestingNodeCount > 1)
		{
			// the option only works when there are only one asset
			ImportOptions->bUsedAsFullName = false;
		}

		const FString Filename(FileToImport);
		if (RootNodeToImport && InterestingNodeCount > 0)
		{
			int32 NodeIndex = 0;

			int32 ImportedMeshCount = 0;
			if (MeshTypeToImport == FBXIT_StaticMesh)  // static mesh
			{
				UStaticMesh* NewStaticMesh = NULL;
				if (bCombineMeshes)
				{
					TArray<FbxNode*> FbxMeshArray;
					FbxImporter->FillFbxMeshArray(RootNodeToImport, FbxMeshArray, FbxImporter);
					if (FbxMeshArray.Num() > 0)
					{
						NewStaticMesh = FbxImporter->ImportStaticMeshAsSingle(InParent, FbxMeshArray, Name, Flags, ImportUI->StaticMeshImportData, NULL, 0);
						if (NewStaticMesh != nullptr)
						{
							FbxImporter->ReorderMaterialToFbxOrder(NewStaticMesh, FbxMeshArray);
						}
					}

					ImportedMeshCount = NewStaticMesh ? 1 : 0;
				}
				else if (bCombineMeshesLOD)
				{
					TArray<FbxNode*> FbxMeshArray;
					TArray<FbxNode*> FbxLodGroups;
					TArray<TArray<FbxNode*>> FbxMeshesLod;
					FbxImporter->FillFbxMeshAndLODGroupArray(RootNodeToImport, FbxLodGroups, FbxMeshArray);
					FbxMeshesLod.Add(FbxMeshArray);
					for (FbxNode* LODGroup : FbxLodGroups)
					{
						if (LODGroup->GetNodeAttribute() && LODGroup->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup && LODGroup->GetChildCount() > 0)
						{
							for (int32 GroupLodIndex = 0; GroupLodIndex < LODGroup->GetChildCount(); ++GroupLodIndex)
							{
								TArray<FbxNode*> AllNodeInLod;
								FbxImporter->FindAllLODGroupNode(AllNodeInLod, LODGroup, GroupLodIndex);
								if (AllNodeInLod.Num() > 0)
								{
									if (FbxMeshesLod.Num() <= GroupLodIndex)
									{
										FbxMeshesLod.Add(AllNodeInLod);
									}
									else
									{
										TArray<FbxNode*> &LODGroupArray = FbxMeshesLod[GroupLodIndex];
										for (FbxNode* NodeToAdd : AllNodeInLod)
										{
											LODGroupArray.Add(NodeToAdd);
										}
									}
								}
							}
						}
					}

					//Import the LOD root
					if (FbxMeshesLod.Num() > 0)
					{
						TArray<FbxNode*> &LODMeshesArray = FbxMeshesLod[0];
						NewStaticMesh = FbxImporter->ImportStaticMeshAsSingle(InParent, LODMeshesArray, Name, Flags, ImportUI->StaticMeshImportData, NULL, 0);
					}
					//Import all LODs
					for (int32 LODIndex = 1; LODIndex < FbxMeshesLod.Num(); ++LODIndex)
					{
						TArray<FbxNode*> &LODMeshesArray = FbxMeshesLod[LODIndex];
						FbxImporter->ImportStaticMeshAsSingle(InParent, LODMeshesArray, Name, Flags, ImportUI->StaticMeshImportData, NewStaticMesh, LODIndex);
					}
				}
				else
				{
					TArray<UObject*> AllNewAssets;
					UObject* Object = RecursiveImportNode(FbxImporter, RootNodeToImport, InParent, Name, Flags, NodeIndex, InterestingNodeCount, AllNewAssets);

					NewStaticMesh = Cast<UStaticMesh>(Object);

					// Make sure to notify the asset registry of all assets created other than the one returned, which will notify the asset registry automatically.
					for (auto AssetIt = AllNewAssets.CreateConstIterator(); AssetIt; ++AssetIt)
					{
						UObject* Asset = *AssetIt;
						if (Asset != NewStaticMesh)
						{
							FAssetRegistryModule::AssetCreated(Asset);
							Asset->MarkPackageDirty();
						}
					}

					ImportedMeshCount = AllNewAssets.Num();
				}

				// Importing static mesh global sockets only if one mesh is imported
				if (ImportedMeshCount == 1 && NewStaticMesh)
				{
					FbxImporter->ImportStaticMeshGlobalSockets(NewStaticMesh);
				}

				NewObject = NewStaticMesh;

			}
			else if (ImportUI->MeshTypeToImport == FBXIT_SkeletalMesh)// skeletal mesh
			{
				int32 TotalNumNodes = 0;

				for (int32 i = 0; i < SkelMeshArray.Num(); i++)
				{
					TArray<FbxNode*> NodeArray = *SkelMeshArray[i];

					TotalNumNodes += NodeArray.Num();
					// check if there is LODGroup for this skeletal mesh
					int32 MaxLODLevel = 1;
					for (int32 j = 0; j < NodeArray.Num(); j++)
					{
						FbxNode* Node = NodeArray[j];
						if (Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
						{
							// get max LODgroup level
							if (MaxLODLevel < Node->GetChildCount())
							{
								MaxLODLevel = Node->GetChildCount();
							}
						}
					}

					int32 LODIndex;
					int32 SuccessfulLodIndex = 0;
					bool bImportSkeletalMeshLODs = ImportUI->SkeletalMeshImportData->bImportMeshLODs;
					for (LODIndex = 0; LODIndex < MaxLODLevel; LODIndex++)
					{
						//We need to know what is the imported lod index when importing the morph targets
						int32 ImportedSuccessfulLodIndex = INDEX_NONE;
						if (!bImportSkeletalMeshLODs && LODIndex > 0) // not import LOD if UI option is OFF
						{
							break;
						}

						TArray<FbxNode*> SkelMeshNodeArray;
						for (int32 j = 0; j < NodeArray.Num(); j++)
						{
							FbxNode* Node = NodeArray[j];
							if (Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
							{
								TArray<FbxNode*> NodeInLod;
								if (Node->GetChildCount() > LODIndex)
								{
									FbxImporter->FindAllLODGroupNode(NodeInLod, Node, LODIndex);
								}
								else // in less some LODGroups have less level, use the last level
								{
									FbxImporter->FindAllLODGroupNode(NodeInLod, Node, Node->GetChildCount() - 1);
								}

								for (FbxNode *MeshNode : NodeInLod)
								{
									SkelMeshNodeArray.Add(MeshNode);
								}
							}
							else
							{
								SkelMeshNodeArray.Add(Node);
							}
						}

						if (LODIndex == 0 && SkelMeshNodeArray.Num() != 0)
						{
							FName OutputName = FbxImporter->MakeNameForMesh(Name.ToString(), SkelMeshNodeArray[0]);

							USkeletalMesh* NewMesh = FbxImporter->ImportSkeletalMesh(InParent, SkelMeshNodeArray, OutputName, Flags, ImportUI->SkeletalMeshImportData, LODIndex, &bOperationCanceled);
							NewObject = NewMesh;

							if (bOperationCanceled)
							{
								// User cancelled, clean up and return
								FbxImporter->ReleaseScene();
								Warn->EndSlowTask();
								bOperationCanceled = true;
								return nullptr;
							}

							if (NewMesh)
							{
								if (ImportUI->bImportAnimations)
								{
									// We need to remove all scaling from the root node before we set up animation data.
									// Othewise some of the global transform calculations will be incorrect.
									FbxImporter->RemoveTransformSettingsFromFbxNode(RootNodeToImport, ImportUI->SkeletalMeshImportData);
									FbxImporter->SetupAnimationDataFromMesh(NewMesh, InParent, SkelMeshNodeArray, ImportUI->AnimSequenceImportData, OutputName.ToString());

									// Reapply the transforms for the rest of the import
									FbxImporter->ApplyTransformSettingsToFbxNode(RootNodeToImport, ImportUI->SkeletalMeshImportData);
									ImportedSuccessfulLodIndex = SuccessfulLodIndex;
								}
								//Increment the LOD index
								SuccessfulLodIndex++;
							}
						}
						else if (NewObject) // the base skeletal mesh is imported successfully
						{
							USkeletalMesh* BaseSkeletalMesh = Cast<USkeletalMesh>(NewObject);
							FName LODObjectName = NAME_None;
							USkeletalMesh *LODObject = FbxImporter->ImportSkeletalMesh(BaseSkeletalMesh->GetOutermost(), SkelMeshNodeArray, LODObjectName, RF_Transient, ImportUI->SkeletalMeshImportData, SuccessfulLodIndex, &bOperationCanceled);
							bool bImportSucceeded = !bOperationCanceled && FbxImporter->ImportSkeletalMeshLOD(LODObject, BaseSkeletalMesh, SuccessfulLodIndex, false);

							if (bImportSucceeded)
							{
								BaseSkeletalMesh->LODInfo[SuccessfulLodIndex].ScreenSize = 1.0f / (MaxLODLevel * SuccessfulLodIndex);
								ImportedSuccessfulLodIndex = SuccessfulLodIndex;
								SuccessfulLodIndex++;
							}
							else
							{
								FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_SkeletalMeshLOD", "Failed to import Skeletal mesh LOD.")), FFbxErrors::SkeletalMesh_LOD_FailedToImport);
							}
						}

						// import morph target
						if (NewObject && ImportUI->SkeletalMeshImportData->bImportMorphTargets && ImportedSuccessfulLodIndex != INDEX_NONE)
						{
							// Disable material importing when importing morph targets
							uint32 bImportMaterials = ImportOptions->bImportMaterials;
							ImportOptions->bImportMaterials = 0;
							uint32 bImportTextures = ImportOptions->bImportTextures;
							ImportOptions->bImportTextures = 0;

							FbxImporter->ImportFbxMorphTarget(SkelMeshNodeArray, Cast<USkeletalMesh>(NewObject), InParent, ImportedSuccessfulLodIndex);

							ImportOptions->bImportMaterials = !!bImportMaterials;
							ImportOptions->bImportTextures = !!bImportTextures;
						}
					}

					if (NewObject)
					{
						NodeIndex++;
						FFormatNamedArguments Args;
						Args.Add(TEXT("NodeIndex"), NodeIndex);
						Args.Add(TEXT("ArrayLength"), SkelMeshArray.Num());
						GWarn->StatusUpdate(NodeIndex, SkelMeshArray.Num(), FText::Format(NSLOCTEXT("UnrealEd", "Importingf", "Importing ({NodeIndex} of {ArrayLength})"), Args));
					}
				}

				for (int32 i = 0; i < SkelMeshArray.Num(); i++)
				{
					delete SkelMeshArray[i];
				}

				// if total nodes we found is 0, we didn't find anything. 
				if (TotalNumNodes == 0)
				{
					FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_NoMeshFoundOnRoot", "Could not find any valid mesh on the root hierarchy. If you have mesh in the sub hierarchy, please enable option of [Import Meshes In Bone Hierarchy] when import.")),
						FFbxErrors::SkeletalMesh_NoMeshFoundOnRoot);
				}
			}
			else if (ImportUI->MeshTypeToImport == FBXIT_SubDSurface) // SubDSurface
			{
				USubDSurface* NewMesh = NULL;
				TArray<FbxNode*> FbxMeshArray;
				FbxImporter->FillFbxMeshArray(RootNodeToImport, FbxMeshArray, FbxImporter);
				if (FbxMeshArray.Num() > 0)
				{
					NewMesh = Cast<USubDSurface>(CreateOrOverwriteAsset(USubDSurface::StaticClass(), InParent, Name, Flags));

					bool bOk = FbxImporter->ImportSubDSurface(NewMesh, InParent, FbxMeshArray, Name, Flags, ImportUI->StaticMeshImportData);

					if (!bOk)
					{
						FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_ImportSubDSurface", "Could not import subdivision surface mesh (no quad mesh?).")),
							FFbxErrors::SkeletalMesh_ImportSubDSurface);

						ObjectTools::DeleteSingleObject(NewMesh);
					}
				}

				ImportedMeshCount = NewMesh ? 1 : 0;

				NewObject = NewMesh;
			}
			else if (ImportUI->MeshTypeToImport == FBXIT_Animation)// animation
			{
				if (ImportOptions->SkeletonForAnimation)
				{
					// will return the last animation sequence that were added
					NewObject = UEditorEngine::ImportFbxAnimation(ImportOptions->SkeletonForAnimation, InParent, ImportUI->AnimSequenceImportData, *Filename, *Name.ToString(), true);
				}
			}
		}
		else
		{
			if (RootNodeToImport == NULL)
			{
				FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_InvalidRoot", "Could not find root node.")), FFbxErrors::SkeletalMesh_InvalidRoot);
			}
			else if (ImportUI->MeshTypeToImport == FBXIT_SkeletalMesh)
			{
				FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_InvalidBone", "Failed to find any bone hierarchy. Try disabling the \"Import As Skeletal\" option to import as a rigid mesh. ")), FFbxErrors::SkeletalMesh_InvalidBone);
			}
			else
			{
				FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, LOCTEXT("FailedToImport_InvalidNode", "Could not find any node.")), FFbxErrors::SkeletalMesh_InvalidNode);
			}
		}
#endif
	}
}

void UnFbx::FFbxImporter::ConvertScene()
{
	if (GetImportOptions()->bConvertScene)
	{
		// we use -Y as forward axis here when we import. This is odd considering our forward axis is technically +X
		// but this is to mimic Maya/Max behavior where if you make a model facing +X facing, 
		// when you import that mesh, you want +X facing in engine. 
		// only thing that doesn't work is hand flipping because Max/Maya is RHS but UE is LHS
		// On the positive note, we now have import transform set up you can do to rotate mesh if you don't like default setting
		FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eRightHanded;
		FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eZAxis;
		FbxAxisSystem::EFrontVector FrontVector = (FbxAxisSystem::EFrontVector) - FbxAxisSystem::eParityOdd;
		if (GetImportOptions()->bForceFrontXAxis)
		{
			FrontVector = FbxAxisSystem::eParityEven;
		}


		FbxAxisSystem UnrealImportAxis(UpVector, FrontVector, CoordSystem);

		FbxAxisSystem SourceSetup = Scene->GetGlobalSettings().GetAxisSystem();

		if (SourceSetup != UnrealImportAxis)
		{
			FbxRootNodeUtility::RemoveAllFbxRoots(Scene);
			UnrealImportAxis.ConvertScene(Scene);
			FbxAMatrix JointOrientationMatrix;
			JointOrientationMatrix.SetIdentity();
			if (GetImportOptions()->bForceFrontXAxis)
			{
				JointOrientationMatrix.SetR(FbxVector4(-90.0, -90.0, 0.0));
			}
			FFbxDataConverter::SetJointPostConversionMatrix(JointOrientationMatrix);
		}
	}
	// Convert the scene's units to what is used in this program, if needed.
	// The base unit used in both FBX and Unreal is centimeters.  So unless the units 
	// are already in centimeters (ie: scalefactor 1.0) then it needs to be converted
	if (GetImportOptions()->bConvertSceneUnit && Scene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::cm)
	{
		FbxSystemUnit::cm.ConvertScene(Scene);
	}
}

FBXImportOptions* UnFbx::FFbxImporter::GetImportOptions() const
{
	return ImportOptions;
}

void UnFbx::FFbxImporter::RecursiveFixSkeleton(FbxNode* Node, TArray<FbxNode*> &SkelMeshes, bool bImportNestedMeshes)
{
	for (int32 i = 0; i < Node->GetChildCount(); i++)
	{
		RecursiveFixSkeleton(Node->GetChild(i), SkelMeshes, bImportNestedMeshes);
	}

	FbxNodeAttribute* Attr = Node->GetNodeAttribute();
	if (Attr && (Attr->GetAttributeType() == FbxNodeAttribute::eMesh || Attr->GetAttributeType() == FbxNodeAttribute::eNull))
	{
		if (bImportNestedMeshes  && Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			// for leaf mesh, keep them as mesh
			int32 ChildCount = Node->GetChildCount();
			int32 ChildIndex;
			for (ChildIndex = 0; ChildIndex < ChildCount; ChildIndex++)
			{
				FbxNode* Child = Node->GetChild(ChildIndex);
				if (Child->GetMesh() == NULL)
				{
					break;
				}
			}

			if (ChildIndex != ChildCount)
			{
				// Remove from the mesh list it is no longer a mesh
				SkelMeshes.Remove(Node);

				//replace with skeleton
				FbxSkeleton* lSkeleton = FbxSkeleton::Create(SdkManager, "");
				Node->SetNodeAttribute(lSkeleton);
				lSkeleton->SetSkeletonType(FbxSkeleton::eLimbNode);
			}
			else // this mesh may be not in skeleton mesh list. If not, add it.
			{
				if (!SkelMeshes.Contains(Node))
				{
					SkelMeshes.Add(Node);
				}
			}
		}
		else
		{
			// Remove from the mesh list it is no longer a mesh
			SkelMeshes.Remove(Node);

			//replace with skeleton
			FbxSkeleton* lSkeleton = FbxSkeleton::Create(SdkManager, "");
			Node->SetNodeAttribute(lSkeleton);
			lSkeleton->SetSkeletonType(FbxSkeleton::eLimbNode);
		}
	}
}

/**
* Get all Fbx skeletal mesh objects in the scene. these meshes are grouped by skeleton they bind to
*
* @param Node Root node to find skeletal meshes
* @param outSkelMeshArray return Fbx meshes they are grouped by skeleton
*/
void FFbxImporter::FillFbxSkelMeshArrayInScene(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, bool ExpandLOD, bool bForceFindRigid /*= false*/)
{
	TArray<FbxNode*> SkeletonArray;

	// a) find skeletal meshes

	RecursiveFindFbxSkelMesh(Node, outSkelMeshArray, SkeletonArray, ExpandLOD);
	// for skeletal mesh, we convert the skeleton system to skeleton
	// in less we recognize bone mesh as rigid mesh if they are textured
	for (int32 SkelIndex = 0; SkelIndex < SkeletonArray.Num(); SkelIndex++)
	{
		RecursiveFixSkeleton(SkeletonArray[SkelIndex], *outSkelMeshArray[SkelIndex], ImportOptions->bImportMeshesInBoneHierarchy);
	}

	// if it doesn't want group node (or null node) for root, remove them
	/*
	if (ImportOptions->bImportGroupNodeAsRoot == false)
	{
	// find the last node
	for ( int32 SkelMeshIndex=0; SkelMeshIndex < outSkelMeshArray.Num() ; ++SkelMeshIndex )
	{
	auto SkelMesh = outSkelMeshArray[SkelMeshIndex];
	if ( SkelMesh->Num() > 0 )
	{
	auto Node = SkelMesh->Last();

	if(Node)
	{
	DumpFBXNode(Node);

	FbxNodeAttribute* Attr = Node->GetNodeAttribute();
	if(Attr && Attr->GetAttributeType() == FbxNodeAttribute::eNull)
	{
	// if root is null, just remove
	SkelMesh->Remove(Node);
	// SkelMesh is still valid?
	if ( SkelMesh->Num() == 0 )
	{
	// we remove this from outSkelMeshArray
	outSkelMeshArray.RemoveAt(SkelMeshIndex);
	--SkelMeshIndex;
	}
	}
	}
	}
	}
	}*/

	// b) find rigid mesh

	// If we are attempting to import a skeletal mesh but we have no hierarchy attempt to find a rigid mesh.
	if (bForceFindRigid || outSkelMeshArray.Num() == 0)
	{
		RecursiveFindRigidMesh(Node, outSkelMeshArray, SkeletonArray, ExpandLOD);
		if (bForceFindRigid)
		{
			//Cleanup the rigid mesh, We want to remove any real static mesh from the outSkelMeshArray
			//Any non skinned mesh that contain no animation should be part of this array.
			int32 AnimStackCount = Scene->GetSrcObjectCount<FbxAnimStack>();
			TArray<int32> SkeletalMeshArrayToRemove;
			for (int32 i = 0; i < outSkelMeshArray.Num(); i++)
			{
				bool bIsValidSkeletal = false;
				TArray<FbxNode*> NodeArray = *outSkelMeshArray[i];
				for (FbxNode *InspectedNode : NodeArray)
				{
					FbxMesh* Mesh = InspectedNode->GetMesh();
					if (Mesh == nullptr)
					{
						continue;
					}
					if (Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
					{
						bIsValidSkeletal = true;
						break;
					}
					//If there is some anim object we count this as a valid skeletal mesh imported as rigid mesh
					for (int32 AnimStackIndex = 0; AnimStackIndex < AnimStackCount; AnimStackIndex++)
					{
						FbxAnimStack* CurAnimStack = Scene->GetSrcObject<FbxAnimStack>(AnimStackIndex);
						// set current anim stack

						Scene->SetCurrentAnimationStack(CurAnimStack);

						FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
						InspectedNode->GetAnimationInterval(AnimTimeSpan, CurAnimStack);

						if (AnimTimeSpan.GetDuration() > 0)
						{
							bIsValidSkeletal = true;
							break;
						}
					}
					if (bIsValidSkeletal)
					{
						break;
					}
				}
				if (!bIsValidSkeletal)
				{
					SkeletalMeshArrayToRemove.Add(i);
				}
			}
			for (int32 i = SkeletalMeshArrayToRemove.Num() - 1; i >= 0; --i)
			{
				if (!SkeletalMeshArrayToRemove.IsValidIndex(i) || !outSkelMeshArray.IsValidIndex(SkeletalMeshArrayToRemove[i]))
					continue;
				int32 IndexToRemove = SkeletalMeshArrayToRemove[i];
				outSkelMeshArray[IndexToRemove]->Empty();
				outSkelMeshArray.RemoveAt(IndexToRemove);
			}
		}
	}
	//Empty the skeleton array
	SkeletonArray.Empty();


}
/**
* Get all Fbx skeletal mesh objects which are grouped by skeleton they bind to
*
* @param Node Root node to find skeletal meshes
* @param outSkelMeshArray return Fbx meshes they are grouped by skeleton
* @param SkeletonArray
* @param ExpandLOD flag of expanding LOD to get each mesh
*/
void FFbxImporter::RecursiveFindFbxSkelMesh(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, TArray<FbxNode*>& SkeletonArray, bool ExpandLOD)
{
	FbxNode* SkelMeshNode = nullptr;
	FbxNode* NodeToAdd = Node;

	DumpFBXNode(Node);

	if (Node->GetMesh() && Node->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		SkelMeshNode = Node;
	}
	else if (Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
	{
		// for LODgroup, add the LODgroup to OutSkelMeshArray according to the skeleton that the first child bind to
		SkelMeshNode = FindLODGroupNode(Node, 0);
		// check if the first child is skeletal mesh
		if (SkelMeshNode != nullptr && !(SkelMeshNode->GetMesh() && SkelMeshNode->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) > 0))
		{
			SkelMeshNode = nullptr;
		}
		else if (ExpandLOD)
		{
			// if ExpandLOD is true, only add the first LODGroup level node
			NodeToAdd = SkelMeshNode;
		}
		// else NodeToAdd = Node;
	}

	if (SkelMeshNode)
	{
		// find root skeleton

		check(SkelMeshNode->GetMesh() != nullptr);
		const int32 fbxDeformerCount = SkelMeshNode->GetMesh()->GetDeformerCount();
		FbxSkin* Deformer = static_cast<FbxSkin*>(SkelMeshNode->GetMesh()->GetDeformer(0, FbxDeformer::eSkin));

		if (Deformer != NULL)
		{
			int32 ClusterCount = Deformer->GetClusterCount();
			bool bFoundCorrectLink = false;
			for (int32 ClusterId = 0; ClusterId < ClusterCount; ++ClusterId)
			{
				FbxNode* Link = Deformer->GetCluster(ClusterId)->GetLink(); //Get the bone influences by this first cluster
				Link = GetRootSkeleton(Link); // Get the skeleton root itself

				if (Link)
				{
					int32 i;
					for (i = 0; i < SkeletonArray.Num(); i++)
					{
						if (Link == SkeletonArray[i])
						{
							// append to existed outSkelMeshArray element
							TArray<FbxNode*>* TempArray = outSkelMeshArray[i];
							TempArray->Add(NodeToAdd);
							break;
						}
					}

					// if there is no outSkelMeshArray element that is bind to this skeleton
					// create new element for outSkelMeshArray
					if (i == SkeletonArray.Num())
					{
						TArray<FbxNode*>* TempArray = new TArray<FbxNode*>();
						TempArray->Add(NodeToAdd);
						outSkelMeshArray.Add(TempArray);
						SkeletonArray.Add(Link);

						if (ImportOptions->bImportScene && !ImportOptions->bTransformVertexToAbsolute)
						{
							FbxVector4 NodeScaling = NodeToAdd->EvaluateLocalScaling();
							FbxVector4 NoScale(1.0, 1.0, 1.0);
							if (NodeScaling != NoScale)
							{
								//Scene import cannot import correctly a skeletal mesh with a root node containing scale
								//Warn the user is skeletal mesh can be wrong
								AddTokenizedErrorMessage(
									FTokenizedMessage::Create(
										EMessageSeverity::Warning,
										FText::Format(LOCTEXT("FBX_ImportSceneSkeletalMeshRootNodeScaling", "Importing skeletal mesh {0} that dont have a mesh node with no scale is not supported when doing an import scene."), FText::FromString(UTF8_TO_TCHAR(NodeToAdd->GetName())))
									),
									FFbxErrors::SkeletalMesh_InvalidRoot
								);
							}
						}
					}

					bFoundCorrectLink = true;
					break;
				}
			}

			// we didn't find the correct link
			if (!bFoundCorrectLink)
			{
				AddTokenizedErrorMessage(
					FTokenizedMessage::Create(
						EMessageSeverity::Warning,
						FText::Format(LOCTEXT("FBX_NoWeightsOnDeformer", "Ignoring mesh {0} because it but no weights."), FText::FromString(UTF8_TO_TCHAR(SkelMeshNode->GetName())))
					),
					FFbxErrors::SkeletalMesh_NoWeightsOnDeformer
				);
			}
		}
	}

	//Skeletalmesh node can have child so let's always iterate trough child
	{
		int32 ChildIndex;
		TArray<FbxNode*> ChildNoScale;
		TArray<FbxNode*> ChildScale;
		//Sort the node to have the one with no scaling first so we have more chance
		//to have a root skeletal mesh with no scale. Because scene import do not support
		//root skeletal mesh containing scale
		for (ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
		{
			FbxNode *ChildNode = Node->GetChild(ChildIndex);

			if (!Node->GetNodeAttribute() || Node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eLODGroup)
			{
				FbxVector4 ChildScaling = ChildNode->EvaluateLocalScaling();
				FbxVector4 NoScale(1.0, 1.0, 1.0);
				if (ChildScaling == NoScale)
				{
					ChildNoScale.Add(ChildNode);
				}
				else
				{
					ChildScale.Add(ChildNode);
				}
			}
		}
		for (FbxNode *ChildNode : ChildNoScale)
		{
			RecursiveFindFbxSkelMesh(ChildNode, outSkelMeshArray, SkeletonArray, ExpandLOD);
		}
		for (FbxNode *ChildNode : ChildScale)
		{
			RecursiveFindFbxSkelMesh(ChildNode, outSkelMeshArray, SkeletonArray, ExpandLOD);
		}
	}
}

void FFbxImporter::RecursiveFindRigidMesh(FbxNode* Node, TArray< TArray<FbxNode*>* >& outSkelMeshArray, TArray<FbxNode*>& SkeletonArray, bool ExpandLOD)
{
	bool bRigidNodeFound = false;
	FbxNode* RigidMeshNode = nullptr;

	DEBUG_FBX_NODE("", Node);

	if (Node->GetMesh())
	{
		// ignore skeletal mesh
		if (Node->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) == 0)
		{
			RigidMeshNode = Node;
			bRigidNodeFound = true;
		}
	}
	else if (Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup)
	{
		// for LODgroup, add the LODgroup to OutSkelMeshArray according to the skeleton that the first child bind to
		FbxNode* FirstLOD = FindLODGroupNode(Node, 0);
		// check if the first child is skeletal mesh
		if (FirstLOD != nullptr && FirstLOD->GetMesh())
		{
			if (FirstLOD->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) == 0)
			{
				bRigidNodeFound = true;
			}
		}

		if (bRigidNodeFound)
		{
			if (ExpandLOD)
			{
				RigidMeshNode = FirstLOD;
			}
			else
			{
				RigidMeshNode = Node;
			}

		}
	}

	if (bRigidNodeFound)
	{
		// find root skeleton
		FbxNode* Link = GetRootSkeleton(RigidMeshNode);

		int32 i;
		for (i = 0; i < SkeletonArray.Num(); i++)
		{
			if (Link == SkeletonArray[i])
			{
				// append to existed outSkelMeshArray element
				TArray<FbxNode*>* TempArray = outSkelMeshArray[i];
				TempArray->Add(RigidMeshNode);
				break;
			}
		}

		// if there is no outSkelMeshArray element that is bind to this skeleton
		// create new element for outSkelMeshArray
		if (i == SkeletonArray.Num())
		{
			TArray<FbxNode*>* TempArray = new TArray<FbxNode*>();
			TempArray->Add(RigidMeshNode);
			outSkelMeshArray.Add(TempArray);
			SkeletonArray.Add(Link);
		}
	}

	// for LODGroup, we will not deep in.
	if (!(Node->GetNodeAttribute() && Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eLODGroup))
	{
		int32 ChildIndex;
		for (ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
		{
			RecursiveFindRigidMesh(Node->GetChild(ChildIndex), outSkelMeshArray, SkeletonArray, ExpandLOD);
		}
	}
}


FbxNode* FFbxImporter::GetRootSkeleton(FbxNode* Link)
{
	FbxNode* RootBone = Link;

	// get Unreal skeleton root
	// mesh and dummy are used as bone if they are in the skeleton hierarchy
	while (RootBone && RootBone->GetParent())
	{
		FbxNodeAttribute* Attr = RootBone->GetParent()->GetNodeAttribute();
		if (Attr &&
			(Attr->GetAttributeType() == FbxNodeAttribute::eMesh ||
				Attr->GetAttributeType() == FbxNodeAttribute::eNull ||
				Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton) &&
			RootBone->GetParent() != Scene->GetRootNode())
		{
			// in some case, skeletal mesh can be ancestor of bones
			// this avoids this situation
			if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FbxMesh* Mesh = (FbxMesh*)Attr;
				if (Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
				{
					break;
				}
			}

			RootBone = RootBone->GetParent();
		}
		else
		{
			break;
		}
	}

	return RootBone;
}

void FFbxImporter::DumpFBXNode(FbxNode* Node)
{
	FbxMesh* Mesh = Node->GetMesh();
	const FString NodeName(Node->GetName());

	if (Mesh)
	{
		UE_LOG(LogFbx, Log, TEXT("================================================="));
		UE_LOG(LogFbx, Log, TEXT("Dumping Node START [%s] "), *NodeName);
		int DeformerCount = Mesh->GetDeformerCount();
		UE_LOG(LogFbx, Log, TEXT("\tTotal Deformer Count %d."), *NodeName, DeformerCount);
		for (int i = 0; i < DeformerCount; i++)
		{
			FbxDeformer* Deformer = Mesh->GetDeformer(i);
			const FString DeformerName(Deformer->GetName());
			const FString DeformerTypeName(Deformer->GetTypeName());
			UE_LOG(LogFbx, Log, TEXT("\t\t[Node %d] %s (Type %s)."), i + 1, *DeformerName, *DeformerTypeName);
			UE_LOG(LogFbx, Log, TEXT("================================================="));
		}

		FbxNodeAttribute* NodeAttribute = Node->GetNodeAttribute();
		if (NodeAttribute)
		{
			FString NodeAttributeName(NodeAttribute->GetName());
			FbxNodeAttribute::EType Type = NodeAttribute->GetAttributeType();
			UE_LOG(LogFbx, Log, TEXT("\tAttribute (%s) Type (%d)."), *NodeAttributeName, (int32)Type);

			for (int i = 0; i < NodeAttribute->GetNodeCount(); ++i)
			{
				FbxNode * Child = NodeAttribute->GetNode(i);

				if (Child)
				{
					const FString ChildName(Child->GetName());
					const FString ChildTypeName(Child->GetTypeName());
					UE_LOG(LogFbx, Log, TEXT("\t\t[Node Attribute Child %d] %s (Type %s)."), i + 1, *ChildName, *ChildTypeName);
				}
			}

		}

		UE_LOG(LogFbx, Log, TEXT("Dumping Node END [%s]"), *NodeName);
	}

	for (int ChildIdx = 0; ChildIdx < Node->GetChildCount(); ChildIdx++)
	{
		FbxNode* ChildNode = Node->GetChild(ChildIdx);
		DumpFBXNode(ChildNode);
	}

}
bool FFbxImporter::ImportFromFile(const FString& Filename, const FString& Type, bool bPreventMaterialNameClash /*= false*/)
{
	bool Result = true;


	switch (CurPhase)
	{
	case NOTSTARTED:
		if (!OpenFile(FString(Filename), false))
		{
			Result = false;
			break;
		}
	case FILEOPENED:
		if (!ImportFile(FString(Filename), bPreventMaterialNameClash))
		{
			Result = false;
			CurPhase = NOTSTARTED;
			break;
		}
	case IMPORTED:
		{
			static const FString Obj(TEXT("obj"));

			// The imported axis system is unknown for obj files
			if( !Type.Equals( Obj, ESearchCase::IgnoreCase ) )
			{
				//Convert the scene
				ConvertScene();

				// do analytics on getting Fbx data
				FbxDocumentInfo* DocInfo = Scene->GetSceneInfo();
				if (DocInfo)
				{
				//	if( FEngineAnalytics::IsAvailable() )
				//	{
				//		TArray<FAnalyticsEventAttribute> Attribs;

				//		FString OriginalVendor(UTF8_TO_TCHAR(DocInfo->Original_ApplicationVendor.Get().Buffer()));
				//		FString OriginalAppName(UTF8_TO_TCHAR(DocInfo->Original_ApplicationName.Get().Buffer()));
				//		FString OriginalAppVersion(UTF8_TO_TCHAR(DocInfo->Original_ApplicationVersion.Get().Buffer()));

				//		FString LastSavedVendor(UTF8_TO_TCHAR(DocInfo->LastSaved_ApplicationVendor.Get().Buffer()));
				//		FString LastSavedAppName(UTF8_TO_TCHAR(DocInfo->LastSaved_ApplicationName.Get().Buffer()));
				//		FString LastSavedAppVersion(UTF8_TO_TCHAR(DocInfo->LastSaved_ApplicationVersion.Get().Buffer()));

				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("Original Application Vendor"), OriginalVendor));
				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("Original Application Name"), OriginalAppName));
				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("Original Application Version"), OriginalAppVersion));

				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("LastSaved Application Vendor"), LastSavedVendor));
				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("LastSaved Application Name"), LastSavedAppName));
				//		Attribs.Add(FAnalyticsEventAttribute(TEXT("LastSaved Application Version"), LastSavedAppVersion));

				//		FString EventString = FString::Printf(TEXT("Editor.Usage.FBX.Import"));
				//		FEngineAnalytics::GetProvider().RecordEvent(EventString, Attribs);
				//	}
				}
			}

			//Warn the user if there is some geometry that cannot be imported because they are not reference by any scene node attribute
			ValidateAllMeshesAreReferenceByNodeAttribute();

			MeshNamesCache.Empty();
		}
		
	default:
		break;
	}
	
	return Result;
}
#undef LOCTEXT_NAMESPACE