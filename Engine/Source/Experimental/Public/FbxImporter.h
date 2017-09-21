#pragma once
#include "CoreMinimal.h"
#include <fbxsdk.h>
#include "fbxsdk\core\math\fbxaffinematrix.h"
DECLARE_LOG_CATEGORY_EXTERN(LogFbx, Log, All);

class YStaticMesh;
#define DEBUG_FBX_NODE( Prepend, FbxNode ) FPlatformMisc::LowLevelOutputDebugStringf( TEXT("%s %s\n"), ANSI_TO_TCHAR(Prepend), ANSI_TO_TCHAR( FbxNode->GetName() ) )

enum EFBXNormalImportMethod
{
	FBXNIM_ComputeNormals ,
	FBXNIM_ImportNormals ,
	FBXNIM_ImportNormalsAndTangents ,
	FBXNIM_MAX,
};

namespace EFBXNormalGenerationMethod
{
	enum Type
	{
		/** Use the legacy built in method to generate normals (faster in some cases) */
		BuiltIn,
		/** Use MikkTSpace to generate normals and tangents */
		MikkTSpace,
	};
}

namespace EVertexColorImportOption
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

enum EFBXAnimationLengthImportType
{
	/** This option imports animation frames based on what is defined at the time of export */
	FBXALIT_ExportedTime,
	/** Will import the range of frames that have animation. Can be useful if the exported range is longer than the actual animation in the FBX file */
	FBXALIT_AnimatedKey,
	/** This will enable the Start Frame and End Frame properties for you to define the frames of animation to import */
	FBXALIT_SetRange,

	FBXALIT_MAX,
};

namespace UnFbx
{

	struct FBXImportOptions
	{
		// General options
		bool bImportScene;
		bool bImportMaterials;
		bool bInvertNormalMap;
		bool bImportTextures;
		bool bImportLOD;
		bool bUsedAsFullName;
		bool bConvertScene;
		bool bForceFrontXAxis;
		bool bConvertSceneUnit;
		bool bRemoveNameSpace;
		FVector ImportTranslation;
		FRotator ImportRotation;
		float ImportUniformScale;
		EFBXNormalImportMethod NormalImportMethod;
		EFBXNormalGenerationMethod::Type NormalGenerationMethod;
		bool bTransformVertexToAbsolute;
		bool bBakePivotInVertex;
		// Static Mesh options
		bool bCombineToSingle;
		EVertexColorImportOption::Type VertexColorImportOption;
		FColor VertexOverrideColor;
		bool bRemoveDegenerates;
		bool bBuildAdjacencyBuffer;
		bool bBuildReversedIndexBuffer;
		bool bGenerateLightmapUVs;
		bool bOneConvexHullPerUCX;
		bool bAutoGenerateCollision;
		FName StaticMeshLODGroup;
		bool bImportStaticMeshLODs;
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
		bool bImportMorph;
		bool bImportAnimations;
		bool bUpdateSkeletonReferencePose;
		bool bResample;
		bool bImportRigidMesh;
		bool bUseT0AsRefPose;
		bool bPreserveSmoothingGroups;
		bool bKeepOverlappingVertices;
		bool bImportMeshesInBoneHierarchy;
		bool bCreatePhysicsAsset;
		//UPhysicsAsset *PhysicsAsset;
		bool bImportSkeletalMeshLODs;
		// Animation option
		//USkeleton* SkeletonForAnimation;
		EFBXAnimationLengthImportType AnimationLengthImportType;
		struct FIntPoint AnimationRange;
		FString AnimationName;
		bool	bPreserveLocalTransform;
		bool	bDeleteExistingMorphTargetCurves;
		bool	bImportCustomAttribute;
		bool	bSetMaterialDriveParameterOnCustomAttribute;
		bool	bRemoveRedundantKeys;
		bool	bDoNotImportCurveWithZero;
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
			return NormalImportMethod == FBXNIM_ImportNormals || NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
		}

		bool ShouldImportTangents()
		{
			return NormalImportMethod == FBXNIM_ImportNormalsAndTangents;
		}

		void ResetForReimportAnimation()
		{
			bImportMorph = true;
			AnimationLengthImportType = FBXALIT_ExportedTime;
		}

		static void ResetOptions(FBXImportOptions *OptionsToReset)
		{
			check(OptionsToReset != nullptr);
			FMemory::Memzero(OptionsToReset, sizeof(OptionsToReset));
		}
	};


#define INVALID_UNIQUE_ID 0xFFFFFFFFFFFFFFFF

	class FFbxAnimCurveHandle
	{
	public:
		enum CurveTypeDescription
		{
			Transform_Translation_X,
			Transform_Translation_Y,
			Transform_Translation_Z,
			Transform_Rotation_X,
			Transform_Rotation_Y,
			Transform_Rotation_Z,
			Transform_Scaling_X,
			Transform_Scaling_Y,
			Transform_Scaling_Z,
			NotTransform,
		};

		FFbxAnimCurveHandle()
		{
			UniqueId = INVALID_UNIQUE_ID;
			Name.Empty();
			ChannelIndex = 0;
			CompositeIndex = 0;
			KeyNumber = 0;
			AnimationTimeSecond = 0.0f;
			AnimCurve = nullptr;
			CurveType = NotTransform;
		}

		FFbxAnimCurveHandle(const FFbxAnimCurveHandle &CurveHandle)
		{
			UniqueId = CurveHandle.UniqueId;
			Name = CurveHandle.Name;
			ChannelIndex = CurveHandle.ChannelIndex;
			CompositeIndex = CurveHandle.CompositeIndex;
			KeyNumber = CurveHandle.KeyNumber;
			AnimationTimeSecond = CurveHandle.AnimationTimeSecond;
			AnimCurve = CurveHandle.AnimCurve;
			CurveType = CurveHandle.CurveType;
		}

		//Identity Data
		uint64 UniqueId;
		FString Name;
		int32 ChannelIndex;
		int32 CompositeIndex;

		//Curve Information
		int32 KeyNumber;
		float AnimationTimeSecond;

		//Pointer to the curve data
		FbxAnimCurve* AnimCurve;

		CurveTypeDescription CurveType;
	};

	class FFbxAnimPropertyHandle
	{
	public:
		FFbxAnimPropertyHandle()
		{
			Name.Empty();
			DataType = eFbxFloat;
		}

		FFbxAnimPropertyHandle(const FFbxAnimPropertyHandle &PropertyHandle)
		{
			Name = PropertyHandle.Name;
			DataType = PropertyHandle.DataType;
			CurveHandles = PropertyHandle.CurveHandles;
		}

		FString Name;
		EFbxType DataType;
		TArray<FFbxAnimCurveHandle> CurveHandles;
	};

	class FFbxAnimNodeHandle
	{
	public:
		FFbxAnimNodeHandle()
		{
			UniqueId = INVALID_UNIQUE_ID;
			Name.Empty();
			AttributeUniqueId = INVALID_UNIQUE_ID;
			AttributeType = FbxNodeAttribute::eUnknown;
		}

		FFbxAnimNodeHandle(const FFbxAnimNodeHandle &NodeHandle)
		{
			UniqueId = NodeHandle.UniqueId;
			Name = NodeHandle.Name;
			AttributeUniqueId = NodeHandle.AttributeUniqueId;
			AttributeType = NodeHandle.AttributeType;
			NodeProperties = NodeHandle.NodeProperties;
			AttributeProperties = NodeHandle.AttributeProperties;
		}

		uint64 UniqueId;
		FString Name;
		TMap<FString, FFbxAnimPropertyHandle> NodeProperties;

		uint64 AttributeUniqueId;
		FbxNodeAttribute::EType AttributeType;
		TMap<FString, FFbxAnimPropertyHandle> AttributeProperties;
	};


	struct FbxMeshInfo
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
	struct FbxNodeInfo
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

	struct FbxSceneInfo
	{
		// data for static mesh
		int32 NonSkinnedMeshNum;

		//data for skeletal mesh
		int32 SkinnedMeshNum;

		// common data
		int32 TotalGeometryNum;
		int32 TotalMaterialNum;
		int32 TotalTextureNum;

		TArray<FbxMeshInfo> MeshInfo;
		TArray<FbxNodeInfo> HierarchyInfo;

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

	/**
	* FBX basic data conversion class.
	*/
	class FFbxDataConverter
	{
	public:
		static void SetJointPostConversionMatrix(FbxAMatrix ConversionMatrix) { JointPostConversionMatrix = ConversionMatrix; }
		static const FbxAMatrix &GetJointPostConversionMatrix() { return JointPostConversionMatrix; }

		static FVector ConvertPos(FbxVector4 Vector);
		static FVector ConvertDir(FbxVector4 Vector);
		static FRotator ConvertEuler(FbxDouble3 Euler);
		static FVector ConvertScale(FbxDouble3 Vector);
		static FVector ConvertScale(FbxVector4 Vector);
		static FRotator ConvertRotation(FbxQuaternion Quaternion);
		static FVector ConvertRotationToFVect(FbxQuaternion Quaternion, bool bInvertRot);
		static FQuat ConvertRotToQuat(FbxQuaternion Quaternion);
		static float ConvertDist(FbxDouble Distance);
		static bool ConvertPropertyValue(FbxProperty& FbxProperty, UProperty& UnrealProperty, union UPropertyValue& OutUnrealPropertyValue);
		static FTransform ConvertTransform(FbxAMatrix Matrix);
		static FMatrix ConvertMatrix(FbxAMatrix Matrix);

		/*
		* Convert fbx linear space color to sRGB FColor
		*/
		static FColor ConvertColor(FbxDouble3 Color);

		static FbxVector4 ConvertToFbxPos(FVector Vector);
		static FbxVector4 ConvertToFbxRot(FVector Vector);
		static FbxVector4 ConvertToFbxScale(FVector Vector);

		/*
		* Convert sRGB FColor to fbx linear space color
		*/
		static FbxDouble3   ConvertToFbxColor(FColor Color);
		static FbxString	ConvertToFbxString(FName Name);
		static FbxString	ConvertToFbxString(const FString& String);
	private:
		static FbxAMatrix JointPostConversionMatrix;
	};

	//struct FImportedMaterialData
	//{
	//public:
	//	void AddImportedMaterial(FbxSurfaceMaterial& FbxMaterial, UMaterialInterface& UnrealMaterial);
	//	bool IsUnique(FbxSurfaceMaterial& FbxMaterial, FName ImportedMaterialName) const;
	//	UMaterialInterface* GetUnrealMaterial(const FbxSurfaceMaterial& FbxMaterial) const;
	//	void Clear();
	//private:
	//	/** Mapping of FBX material to Unreal material.  Some materials in FBX have the same name so we use this map to determine if materials are unique */
	//	TMap<FbxSurfaceMaterial*, TWeakObjectPtr<UMaterialInterface> > FbxToUnrealMaterialMap;
	//	TSet<FName> ImportedMaterialNames;
	//};

	class FFbxImporter
	{
	public:
		~FFbxImporter();
		/**
		* Returns the importer singleton. It will be created on the first request.
		*/
		static FFbxImporter* GetInstance();
		static void DeleteInstance();

		/**
		* Detect if the FBX file has skeletal mesh model. If there is deformer definition, then there is skeletal mesh.
		* In this function, we don't need to import the scene. But the open process is time-consume if the file is large.
		*
		* @param InFilename	FBX file name.
		* @return int32 -1 if parse failed; 0 if geometry ; 1 if there are deformers; 2 otherwise
		*/
		int32 GetImportType(const FString& InFilename);

		/**
		* Get detail infomation in the Fbx scene
		*
		* @param Filename Fbx file name
		* @param SceneInfo return the scene info
		* @return bool true if get scene info successfully
		*/
		bool GetSceneInfo(FString Filename, FbxSceneInfo& SceneInfo, bool bPreventMaterialNameClash = false);

		/**
		* Initialize Fbx file for import.
		*
		* @param Filename
		* @param bParseStatistics
		* @return bool
		*/
		bool OpenFile(FString Filename, bool bParseStatistics, bool bForSceneInfo = false);

		/**
		* Import Fbx file.
		*
		* @param Filename
		* @return bool
		*/
		bool ImportFile(FString Filename, bool bPreventMaterialNameClash = false);

		/**
		* Convert the scene from the current options.
		* The scene will be converted to RH -Y or RH X depending if we force a front X axis or not
		*/
		void ConvertScene();

		/**
		* Attempt to load an FBX scene from a given filename.
		*
		* @param Filename FBX file name to import.
		* @returns true on success.
		*/
		bool ImportFromFile(const FString& Filename, const FString& Type, bool bPreventMaterialNameClash = false);

		/**
		* Retrieve the FBX loader's error message explaining its failure to read a given FBX file.
		* Note that the message should be valid even if the parser is successful and may contain warnings.
		*
		* @ return TCHAR*	the error message
		*/
		const TCHAR* GetErrorMessage() const
		{
			return *ErrorMessage;
		}

		/**
		* Retrieve the object inside the FBX scene from the name
		*
		* @param ObjectName	Fbx object name
		* @param Root	Root node, retrieve from it
		* @return FbxNode*	Fbx object node
		*/
		FbxNode* RetrieveObjectFromName(const TCHAR* ObjectName, FbxNode* Root = NULL);

		/**
		* Find the first node containing a mesh attribute for the specified LOD index.
		*
		* @param NodeLodGroup	The LOD group fbx node
		* @param LodIndex		The index of the LOD we search the mesh node
		*/
		FbxNode* FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind = nullptr);

		/**
		* Find the all the node containing a mesh attribute for the specified LOD index.
		*
		* @param OutNodeInLod   All the mesh node under the lod group
		* @param NodeLodGroup	The LOD group fbx node
		* @param LodIndex		The index of the LOD we search the mesh node
		*/
		void FindAllLODGroupNode(TArray<FbxNode*> &OutNodeInLod, FbxNode* NodeLodGroup, int32 LodIndex);

		/**
		* Find the first parent node containing a eLODGroup attribute.
		*
		* @param ParentNode		The node where to start the search.
		*/
		FbxNode *RecursiveFindParentLodGroup(FbxNode *ParentNode);

		/**
		* Creates a static mesh with the given name and flags, imported from within the FBX scene.
		* @param InParent
		* @param Node	Fbx Node to import
		* @param Name	the Unreal Mesh name after import
		* @param Flags
		* @param InStaticMesh	if LODIndex is not 0, this is the base mesh object. otherwise is NULL
		* @param LODIndex	 LOD level to import to
		*
		* @returns UObject*	the UStaticMesh object.
		*/
		YStaticMesh* ImportStaticMesh(UObject* InParent, FbxNode* Node, const FName& Names, UFbxStaticMeshImportData* ImportData, YStaticMesh* InStaticMesh = NULL, int LODIndex = 0, void *ExistMeshDataPtr = nullptr);

		/**
		* Creates a static mesh from all the meshes in FBX scene with the given name and flags.
		*
		* @param InParent
		* @param MeshNodeArray	Fbx Nodes to import
		* @param Name	the Unreal Mesh name after import
		* @param Flags
		* @param InStaticMesh	if LODIndex is not 0, this is the base mesh object. otherwise is NULL
		* @param LODIndex	 LOD level to import to
		* @param OrderedMaterialNames  If not null, the original fbx ordered materials name will be use to reorder the section of the mesh we currently import
		*
		* @returns UObject*	the UStaticMesh object.
		*/
		YStaticMesh* ImportStaticMeshAsSingle(UObject* InParent, TArray<FbxNode*>& MeshNodeArray, const FName InName, UFbxStaticMeshImportData* TemplateImportData, UStaticMesh* InStaticMesh, int LODIndex = 0, void *ExistMeshDataPtr = nullptr);
	protected:
		enum IMPORTPHASE
		{
			NOTSTARTED,
			FILEOPENED,
			IMPORTED
		};

		static TSharedPtr<FFbxImporter> StaticInstance;

		struct FFbxMaterial
		{
			FbxSurfaceMaterial* FbxMaterial;
			UMaterialInterface* Material;

			FString GetName() const { return FbxMaterial ? ANSI_TO_TCHAR(FbxMaterial->GetName()) : TEXT("None"); }
		};

		// scene management
		FFbxDataConverter Converter;
		FbxGeometryConverter* GeometryConverter;
		FbxManager* SdkManager;
		FbxImporter* Importer;
		IMPORTPHASE CurPhase;
		FString ErrorMessage;
		// base path of fbx file
		FString FileBasePath;
		TWeakObjectPtr<UObject> Parent;
		// Flag that the mesh is the first mesh to import in current FBX scene
		// FBX scene may contain multiple meshes, importer can import them at one time.
		// Initialized as true when start to import a FBX scene
		bool bFirstMesh;

		// Set when importing skeletal meshes if the merge bones step fails. Used to track
		// YesToAll and NoToAll for an entire scene
		EAppReturnType::Type LastMergeBonesChoice;

		/**
		* Collision model list. The key is fbx node name
		* If there is an collision model with old name format, the key is empty string("").
		*/
		FbxMap<FbxString, TSharedPtr< FbxArray<FbxNode* > > > CollisionModels;

		FFbxImporter();
	};
}