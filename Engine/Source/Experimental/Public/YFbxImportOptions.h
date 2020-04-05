#pragma once
#include "Core.h"
#include <fbxsdk.h>
struct YFbxMeshInfo
{
	FString Name;
	uint64 UniqueId;
	int32 FaceNum;
	int32 VertexNum;
	bool bTriangulated;
	int32 MaterialNum;
	bool bIsSkelMesh;
	FString SkeletonRoot;
	int32 SkeletonElemNum;
	FString LODGroup;
	int32 LODLevel;
	int32 MorphNum;
};

//Node use to store the scene hierarchy transform will be relative to the parent
struct YFbxNodeInfo
{
	const char* ObjectName;
	uint64 UniqueId;
	FbxAMatrix Transform;
	FbxVector4 RotationPivot;
	FbxVector4 ScalePivot;

	const char* AttributeName;
	uint64 AttributeUniqueId;
	const char* AttributeType;

	const char* ParentName;
	uint64 ParentUniqueId;
};


struct YFbxSceneInfo
{
	// data for static mesh
	int32 NonSkinnedMeshNum;

	//data for skeletal mesh
	int32 SkinnedMeshNum;

	// common data
	int32 TotalGeometryNum;
	int32 TotalMaterialNum;
	int32 TotalTextureNum;

	TArray<YFbxMeshInfo> MeshInfo;
	TArray<YFbxNodeInfo> HierarchyInfo;

	/* true if it has animation */
	bool bHasAnimation;
	double FrameRate;
	double TotalTime;

	void Reset()
	{
		NonSkinnedMeshNum = 0;
		SkinnedMeshNum = 0;
		TotalGeometryNum = 0;
		TotalMaterialNum = 0;
		TotalTextureNum = 0;
		MeshInfo.Empty();
		HierarchyInfo.Empty();
		bHasAnimation = false;
		FrameRate = 0.0;
		TotalTime = 0.0;
	}
};
enum EYFBXNormalImportMethod
{
	YFBXNIM_ComputeNormals,
	YFBXNIM_ImportNormals,
	YFBXNIM_ImportNormalsAndTangents,
	YFBXNIM_MAX,
};

namespace EYFBXNormalGenerationMethod
{
	enum Type
	{
		/** Use the legacy built in method to generate normals (faster in some cases) */
		BuiltIn,
		/** Use MikkTSpace to generate normals and tangents */
		MikkTSpace,
	};
}

enum EYFBXAnimationLengthImportType
{
	/** This option imports animation frames based on what is defined at the time of export */
	YFBXALIT_ExportedTime,
	/** Will import the range of frames that have animation. Can be useful if the exported range is longer than the actual animation in the FBX file */
	YFBXALIT_AnimatedKey,
	/** This will enable the Start Frame and End Frame properties for you to define the frames of animation to import */
	YFBXALIT_SetRange,

	YFBXALIT_MAX,
};

namespace EYVertexColorImportOption
{
	enum Type
	{
		/** Import the static mesh using the vertex colors from the FBX file. */
		Replace,
		/** Ignore vertex colors from the FBX file, and keep the existing mesh vertex colors. */
		Ignore,
		/** Override all vertex colors with the specified color. */
		Override
	};
}

struct YFBXImportOptions
{
	// General options
	bool bImportScene = false;
	bool bImportMaterials = true;
	bool bInvertNormalMap = false;
	bool bImportTextures = true;
	bool bImportLOD = true;
	bool bUsedAsFullName = true;
	bool bConvertScene = true;
	bool bConvertSceneUnit = true;
	bool bRemoveNameSpace = true;
	FVector ImportTranslation = FVector::ZeroVector;
	FRotator ImportRotation = FRotator::ZeroRotator;
	float ImportUniformScale = 1.0f;
	EYFBXNormalImportMethod NormalImportMethod = YFBXNIM_ComputeNormals;
	EYFBXNormalGenerationMethod::Type NormalGenerationMethod = EYFBXNormalGenerationMethod::MikkTSpace;
	bool bTransformVertexToAbsolute = true;
	bool bBakePivotInVertex = false;
	// Static Mesh options
	bool bCombineToSingle = true;
	EYVertexColorImportOption::Type VertexColorImportOption;
	FColor VertexOverrideColor = FColor::White;
	bool bRemoveDegenerates = true;
	bool bBuildAdjacencyBuffer = true;
	bool bBuildReversedIndexBuffer = true;
	bool bGenerateLightmapUVs = true;
	bool bOneConvexHullPerUCX = true;
	bool bAutoGenerateCollision = true;
	FName StaticMeshLODGroup;
	bool bImportStaticMeshLODs = true;
	// Material import options
	class UMaterialInterface *BaseMaterial;
	FString BaseColorName;
	FString BaseDiffuseTextureName;
	FString BaseEmissiveColorName;
	FString BaseNormalTextureName;
	FString BaseEmmisiveTextureName;
	FString BaseSpecularTextureName;
	//EMaterialSearchLocation::Type MaterialSearchLocation;
	// Skeletal Mesh options
	bool bImportMorph = true;
	bool bImportAnimations = true;
	bool bUpdateSkeletonReferencePose = true;
	bool bResample = true;
	bool bImportRigidMesh = true;
	bool bUseT0AsRefPose = false;
	bool bPreserveSmoothingGroups = true;
	bool bKeepOverlappingVertices = false;
	bool bImportMeshesInBoneHierarchy = true;
	bool bCreatePhysicsAsset = true;
	//UPhysicsAsset *PhysicsAsset;
	bool bImportSkeletalMeshLODs = true;
	// Animation option
	//YSkeleton* SkeletonForAnimation;
	EYFBXAnimationLengthImportType AnimationLengthImportType;
	struct FIntPoint AnimationRange = FIntPoint(0, 0);;
	FString AnimationName;
	bool	bPreserveLocalTransform = false;
	bool	bDeleteExistingMorphTargetCurves = false;
	bool	bImportCustomAttribute = false;
	bool	bSetMaterialDriveParameterOnCustomAttribute = false;
	bool	bRemoveRedundantKeys = true;
	bool	bDoNotImportCurveWithZero = true;
	TArray<FString> MaterialCurveSuffixes;

	/** This allow to add a prefix to the material name when unreal material get created.
	*   This prefix can just modify the name of the asset for materials (i.e. TEXT("Mat"))
	*   This prefix can modify the package path for materials (i.e. TEXT("/Materials/")).
	*   Or both (i.e. TEXT("/Materials/Mat"))
	*/
	FName MaterialBasePath;

	//This data allow to override some fbx Material(point by the uint64 id) with existing unreal material asset
	TMap<uint64, class UMaterialInterface*> OverrideMaterials;

	bool ShouldImportNormals()
	{
		return NormalImportMethod == YFBXNIM_ImportNormals || NormalImportMethod == YFBXNIM_ImportNormalsAndTangents;
	}

	bool ShouldImportTangents()
	{
		return NormalImportMethod == YFBXNIM_ImportNormalsAndTangents;
	}

	void ResetForReimportAnimation()
	{
		bImportMorph = true;
		AnimationLengthImportType = YFBXALIT_ExportedTime;
	}

	static void ResetOptions(YFBXImportOptions *OptionsToReset)
	{
		check(OptionsToReset != nullptr);
		FMemory::Memzero(OptionsToReset, sizeof(OptionsToReset));
	}
};
