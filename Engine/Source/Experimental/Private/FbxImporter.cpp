#include "FbxImporter.h"
DEFINE_LOG_CATEGORY(LogFbx)
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