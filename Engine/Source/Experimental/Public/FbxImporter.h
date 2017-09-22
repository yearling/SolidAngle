#pragma once
#include "CoreMinimal.h"
#include <fbxsdk.h>
#include "fbxsdk\core\math\fbxaffinematrix.h"
#include "StaticMeshImportData.h"
#include "Misc\SecureHash.h"
#include "Logging\TokenizedMessage.h"
DECLARE_LOG_CATEGORY_EXTERN(LogFbx, Log, All);

class YStaticMesh;
class YSkeletalMesh;
class YMaterialInterface;
class YRawMesh;
#define DEBUG_FBX_NODE( Prepend, FbxNode ) FPlatformMisc::LowLevelOutputDebugStringf( TEXT("%s %s\n"), ANSI_TO_TCHAR(Prepend), ANSI_TO_TCHAR( FbxNode->GetName() ) )


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

	struct FImportedMaterialData
	{
	public:
		void AddImportedMaterial(FbxSurfaceMaterial& FbxMaterial, YMaterialInterface& UnrealMaterial);
		bool IsUnique(FbxSurfaceMaterial& FbxMaterial, FName ImportedMaterialName) const;
		YMaterialInterface* GetUnrealMaterial(const FbxSurfaceMaterial& FbxMaterial) const;
		void Clear();
	private:
		/** Mapping of FBX material to Unreal material.  Some materials in FBX have the same name so we use this map to determine if materials are unique */
		TMap<FbxSurfaceMaterial*, TWeakPtr<YMaterialInterface> > FbxToUnrealMaterialMap;
		TSet<FName> ImportedMaterialNames;
	};

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
		* Fill the FbxNodeInfo structure recursively to reflect the FbxNode hierarchy. The result will be an array sorted with the parent first
		*
		* @param SceneInfo   The scene info to modify
		* @param Parent      The parent FbxNode
		* @param ParentInfo  The parent FbxNodeInfo
		*/
		void TraverseHierarchyNodeRecursively(FbxSceneInfo& SceneInfo, FbxNode *ParentNode, FbxNodeInfo &ParentInfo);

		/**
		* Recursive search for a node having a mesh attribute
		*
		* @param Node	The node from which we start the search for the first node containing a mesh attribute
		*/
		FbxNode *RecursiveGetFirstMeshNode(FbxNode* Node, FbxNode* NodeToFind = nullptr);
		/**
		* Find the first node containing a mesh attribute for the specified LOD index.
		*
		* @param NodeLodGroup	The LOD group fbx node
		* @param LodIndex		The index of the LOD we search the mesh node
		*/
		FbxNode* FindLODGroupNode(FbxNode* NodeLodGroup, int32 LodIndex, FbxNode *NodeToFind = nullptr);
		
		/**
		* Find the first parent node containing a eLODGroup attribute.
		*
		* @param ParentNode		The node where to start the search.
		*/
		FbxNode *RecursiveFindParentLodGroup(FbxNode *ParentNode);
		
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
		* When there is some materials with the same name we add a clash suffixe _ncl1_x.
		* Example, if we have 3 materials name shader we will get (shader, shader_ncl1_1, shader_ncl1_2).
		*/
		void FixMaterialClashName();

		/**
		* Retrieve the object inside the FBX scene from the name
		*
		* @param ObjectName	Fbx object name
		* @param Root	Root node, retrieve from it
		* @return FbxNode*	Fbx object node
		*/
		FbxNode* RetrieveObjectFromName(const TCHAR* ObjectName, FbxNode* Root = NULL);

		/**
		* Find the all the node containing a mesh attribute for the specified LOD index.
		*
		* @param OutNodeInLod   All the mesh node under the lod group
		* @param NodeLodGroup	The LOD group fbx node
		* @param LodIndex		The index of the LOD we search the mesh node
		*/
		void FindAllLODGroupNode(TArray<FbxNode*> &OutNodeInLod, FbxNode* NodeLodGroup, int32 LodIndex);

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
		YStaticMesh* ImportStaticMeshAsSingle(UObject* InParent, TArray<FbxNode*>& MeshNodeArray, const FName InName, UFbxStaticMeshImportData* TemplateImportData, YStaticMesh* InStaticMesh, int LODIndex = 0, void *ExistMeshDataPtr = nullptr);

		/**
		* Empties the FBX scene, releasing its memory.
		* Currently, we can't release KFbxSdkManager because Fbx Sdk2010.2 has a bug that FBX can only has one global sdkmanager.
		* From Fbx Sdk2011, we can create multiple KFbxSdkManager, then we can release it.
		*/
		void ReleaseScene();

		public:
			// current Fbx scene we are importing. Make sure to release it after import
			FbxScene* Scene;
			FBXImportOptions* ImportOptions;

			//We cache the hash of the file when we open the file. This is to avoid calculating the hash many time when importing many asset in one fbx file.
			FMD5Hash Md5Hash;

			//help
			ANSICHAR* MakeName(const ANSICHAR* name);
			FString MakeString(const ANSICHAR* Name);
			FName MakeNameForMesh(FString InName, FbxObject* FbxObject);
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
			YMaterialInterface* Material;

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
		//TWeakObjectPtr<UObject> Parent;
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
		/**
		* Set up the static mesh data from Fbx Mesh.
		*
		* @param StaticMesh Unreal static mesh object to fill data into
		* @param LODIndex	LOD level to set up for StaticMesh
		* @return bool true if set up successfully
		*/
		bool BuildStaticMeshFromGeometry(FbxNode* Node, YStaticMesh* StaticMesh, TArray<FFbxMaterial>& MeshMaterials, int LODIndex, YRawMesh& RawMesh,
			EVertexColorImportOption::Type VertexColorImportOption, const TMap<FVector, FColor>& ExistingVertexColorData, const FColor& VertexOverrideColor);

		/**
		* Clean up for destroy the Importer.
		*/
		void CleanUp();

		/**
		* Compute the global matrix for Fbx Node
		* If we import scene it will return identity plus the pivot if we turn the bake pivot option
		*
		* @param Node	Fbx Node
		* @return KFbxXMatrix*	The global transform matrix
		*/
		FbxAMatrix ComputeTotalMatrix(FbxNode* Node);

		/**
		* Compute the matrix for skeletal Fbx Node
		* If we import don't import a scene it will call ComputeTotalMatrix with Node as the parameter. If we import a scene
		* it will return the relative transform between the RootSkeletalNode and Node.
		*
		* @param Node	Fbx Node
		* @param Node	Fbx RootSkeletalNode
		* @return KFbxXMatrix*	The global transform matrix
		*/
		FbxAMatrix ComputeSkeletalMeshTotalMatrix(FbxNode* Node, FbxNode *RootSkeletalNode);
		/**
		* Check if there are negative scale in the transform matrix and its number is odd.
		* @return bool True if there are negative scale and its number is 1 or 3.
		*/
		bool IsOddNegativeScale(FbxAMatrix& TotalMatrix);

		public:
			/** Import and set up animation related data from mesh **/
			//void SetupAnimationDataFromMesh(YSkeletalMesh * SkeletalMesh, UObject* InParent, TArray<FbxNode*>& NodeArray, UFbxAnimSequenceImportData* ImportData, const FString& Filename);

			/** error message handler */
			void AddTokenizedErrorMessage(TSharedRef<FTokenizedMessage> Error, FName FbxErrorName);
			void ClearTokenizedErrorMessages();
			void FlushToTokenizedErrorMessage(enum EMessageSeverity::Type Severity);
		private:
			friend class FFbxLoggerSetter;

			// logger set/clear function
			class FFbxLogger * Logger;
			void SetLogger(class FFbxLogger * InLogger);
			void ClearLogger();

			FImportedMaterialData ImportedMaterialData;

			//Cache to create unique name for mesh. This is use to fix name clash
			TArray<FString> MeshNamesCache;

	};

	/** message Logger for FBX. Saves all the messages and prints when it's destroyed */
	class FFbxLogger
	{
		FFbxLogger();
		~FFbxLogger();

		/** Error messages **/
		TArray<TSharedRef<FTokenizedMessage>> TokenizedErrorMessages;

		/* The logger will show the LogMessage only if at least one TokenizedErrorMessage have a severity of Error or CriticalError*/
		bool ShowLogMessageOnlyIfError;

		friend class FFbxImporter;
		friend class FFbxLoggerSetter;
	};

	/**
	* This class is to make sure Logger isn't used by outside of purpose.
	* We add this only top level of functions where it needs to be handled
	* if the importer already has logger set, it won't set anymore
	*/
	class FFbxLoggerSetter
	{
		class FFbxLogger Logger;
		FFbxImporter * Importer;

	public:
		FFbxLoggerSetter(FFbxImporter * InImpoter, bool ShowLogMessageOnlyIfError = false)
			: Importer(InImpoter)
		{
			// if impoter doesn't have logger, sets it
			if (Importer->Logger == NULL)
			{
				Logger.ShowLogMessageOnlyIfError = ShowLogMessageOnlyIfError;
				Importer->SetLogger(&Logger);
			}
			else
			{
				// if impoter already has logger set
				// invalidated Importer to make sure it doesn't clear
				Importer = NULL;
			}
		}

		~FFbxLoggerSetter()
		{
			if (Importer)
			{
				Importer->ClearLogger();
			}
		}
	};
}