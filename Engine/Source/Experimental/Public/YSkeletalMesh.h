#pragma once
#include "Core.h"
#include "MeshCommon.h"
#include "Skeleton.h"
#include "ReferenceSkeleton.h"
#include "SkeletalMeshTypes.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSkeletalMesh, Error, All);

struct FSkeletalMaterial
{

		FSkeletalMaterial()
		: MaterialInterface(NULL)
		, bEnableShadowCasting_DEPRECATED(true)
		, bRecomputeTangent_DEPRECATED(false)
		, MaterialSlotName(NAME_None)
	{

	}

	FSkeletalMaterial(class UMaterialInterface* InMaterialInterface
		, bool bInEnableShadowCasting = true
		, bool bInRecomputeTangent = false
		, FName InMaterialSlotName = NAME_None
		, FName InImportedMaterialSlotName = NAME_None)
		: MaterialInterface(InMaterialInterface)
		, bEnableShadowCasting_DEPRECATED(bInEnableShadowCasting)
		, bRecomputeTangent_DEPRECATED(bInRecomputeTangent)
		, MaterialSlotName(InMaterialSlotName)
	{

	}

	friend FArchive& operator<<(FArchive& Ar, FSkeletalMaterial& Elem);

	 friend bool operator==(const FSkeletalMaterial& LHS, const FSkeletalMaterial& RHS);
	 friend bool operator==(const FSkeletalMaterial& LHS, const UMaterialInterface& RHS);
	 friend bool operator==(const UMaterialInterface& LHS, const FSkeletalMaterial& RHS);

		class UMaterialInterface *	MaterialInterface;
		bool						bEnableShadowCasting_DEPRECATED;
		bool						bRecomputeTangent_DEPRECATED;

	/*This name should be use by the gameplay to avoid error if the skeletal mesh Materials array topology change*/
		FName						MaterialSlotName;
		FMeshUVChannelInfo			UVChannelData;
};


/** Enum specifying the importance of properties when simplifying skeletal meshes. */
enum SkeletalMeshOptimizationImportance
{
	SMOI_Off,
	SMOI_Lowest,
	SMOI_Low,
	SMOI_Normal,
	SMOI_High,
	SMOI_Highest,
	SMOI_MAX,
};

/** Enum specifying the reduction type to use when simplifying skeletal meshes. */
enum SkeletalMeshOptimizationType
{
	SMOT_NumOfTriangles,
	SMOT_MaxDeviation,
	SMOT_MAX,
};

struct FBoneMirrorInfo
{
		/** The bone to mirror. */
		int32 SourceIndex;

	/** Axis the bone is mirrored across. */
		TEnumAsByte<EAxis::Type> BoneFlipAxis;

	FBoneMirrorInfo()
		: SourceIndex(0)
		, BoneFlipAxis(0)
	{
	}

};

/** Structure to export/import bone mirroring information */
struct FBoneMirrorExport
{
		FName BoneName;
		FName SourceBoneName;
		TEnumAsByte<EAxis::Type> BoneFlipAxis;
	FBoneMirrorExport()
		: BoneFlipAxis(0)
	{
	}

};

/** Struct containing triangle sort settings for a particular section */
struct FTriangleSortSettings
{
		TEnumAsByte<enum ETriangleSortOption> TriangleSorting;
		TEnumAsByte<enum ETriangleSortAxis> CustomLeftRightAxis;
		FName CustomLeftRightBoneName;


	FTriangleSortSettings()
		: TriangleSorting(0)
		, CustomLeftRightAxis(0)
	{
	}

};

/**
* FSkeletalMeshOptimizationSettings - The settings used to optimize a skeletal mesh LOD.
*/
struct FSkeletalMeshOptimizationSettings
{
		/** The method to use when optimizing the skeletal mesh LOD */
		TEnumAsByte<enum SkeletalMeshOptimizationType> ReductionMethod;

	/** If ReductionMethod equals SMOT_NumOfTriangles this value is the ratio of triangles [0-1] to remove from the mesh */
		float NumOfTrianglesPercentage;

	/**If ReductionMethod equals SMOT_MaxDeviation this value is the maximum deviation from the base mesh as a percentage of the bounding sphere. */
		float MaxDeviationPercentage;

	/** The welding threshold distance. Vertices under this distance will be welded. */
		float WeldingThreshold;

	/** Whether Normal smoothing groups should be preserved. If false then NormalsThreshold is used **/
		bool bRecalcNormals;

	/** If the angle between two triangles are above this value, the normals will not be
	smooth over the edge between those two triangles. Set in degrees. This is only used when PreserveNormals is set to false*/
		float NormalsThreshold;

	/** How important the shape of the geometry is. */
		TEnumAsByte<enum SkeletalMeshOptimizationImportance> SilhouetteImportance;

	/** How important texture density is. */
		TEnumAsByte<enum SkeletalMeshOptimizationImportance> TextureImportance;

	/** How important shading quality is. */
		TEnumAsByte<enum SkeletalMeshOptimizationImportance> ShadingImportance;

	/** How important skinning quality is. */
		TEnumAsByte<enum SkeletalMeshOptimizationImportance> SkinningImportance;

	/** The ratio of bones that will be removed from the mesh */
		float BoneReductionRatio;

	/** Maximum number of bones that can be assigned to each vertex. */
		int32 MaxBonesPerVertex;

		//TArray<FBoneReference> BonesToRemove_DEPRECATED;

	/** Maximum number of bones that can be assigned to each vertex. */
		int32 BaseLOD;

	FSkeletalMeshOptimizationSettings()
		: ReductionMethod(SMOT_MaxDeviation)
		, NumOfTrianglesPercentage(1.0f)
		, MaxDeviationPercentage(0)
		, WeldingThreshold(0.1f)
		, bRecalcNormals(true)
		, NormalsThreshold(60.0f)
		, SilhouetteImportance(SMOI_Normal)
		, TextureImportance(SMOI_Normal)
		, ShadingImportance(SMOI_Normal)
		, SkinningImportance(SMOI_Normal)
		, BoneReductionRatio(100.0f)
		, MaxBonesPerVertex(4)
		, BaseLOD(0)
	{
	}

	/** Equality operator. */
	bool operator==(const FSkeletalMeshOptimizationSettings& Other) const
	{
		return ReductionMethod == Other.ReductionMethod
			&& NumOfTrianglesPercentage == Other.NumOfTrianglesPercentage
			&& MaxDeviationPercentage == Other.MaxDeviationPercentage
			&& WeldingThreshold == Other.WeldingThreshold
			&& NormalsThreshold == Other.NormalsThreshold
			&& SilhouetteImportance == Other.SilhouetteImportance
			&& TextureImportance == Other.TextureImportance
			&& ShadingImportance == Other.ShadingImportance
			&& SkinningImportance == Other.SkinningImportance
			&& bRecalcNormals == Other.bRecalcNormals
			&& BoneReductionRatio == Other.BoneReductionRatio
			&& MaxBonesPerVertex == Other.MaxBonesPerVertex
			&& BaseLOD == Other.BaseLOD;
	}

	/** Inequality. */
	bool operator!=(const FSkeletalMeshOptimizationSettings& Other) const
	{
		return !(*this == Other);
	}
};


/** Struct containing information for a particular LOD level, such as materials and info for when to use it. */
struct FSkeletalMeshLODInfo
{

		/**
		* ScreenSize to display this LOD.
		* The screen size is based around the projected diameter of the bounding
		* sphere of the model. i.e. 0.5 means half the screen's maximum dimension.
		*/
		float ScreenSize;

	/**	Used to avoid 'flickering' when on LOD boundary. Only taken into account when moving from complex->simple. */
		float LODHysteresis;

	/** Mapping table from this LOD's materials to the USkeletalMesh materials array. */
		TArray<int32> LODMaterialMap;

	/** Per-section control over whether to enable shadow casting. */
		TArray<bool> bEnableShadowCasting_DEPRECATED;

		TArray<struct FTriangleSortSettings> TriangleSortSettings;

	/** Whether to disable morph targets for this LOD. */
		uint32 bHasBeenSimplified : 1;

	/** Reduction settings to apply when building render data. */
		FSkeletalMeshOptimizationSettings ReductionSettings;

	/** This has been removed in editor. We could re-apply this in import time or by mesh reduction utilities*/
		TArray<FName> RemovedBones;

	/** The filename of the file tha was used to import this LOD if it was not auto generated. */
		FString SourceImportFilename;

	FSkeletalMeshLODInfo()
		: ScreenSize(0)
		, LODHysteresis(0)
		, bHasBeenSimplified(false)
	{
	}

};


class YSkeletalMesh
{
public:
	YSkeletalMesh();
	virtual ~YSkeletalMesh();
private:
	/** Rendering resources created at import time. */
	TSharedPtr<FSkeletalMeshResource> ImportedResource;

public:
	/** Get the default resource for this skeletal mesh. */
	FORCEINLINE FSkeletalMeshResource* GetImportedResource() const { return ImportedResource.Get(); }

	/** Get the resource to use for rendering. */
	FORCEINLINE FSkeletalMeshResource* GetResourceForRendering() const { return GetImportedResource(); }

	/** Skeleton of this skeletal mesh **/
	YSkeleton* Skeleton;

private:
	/** Original imported mesh bounds */
		FBoxSphereBounds ImportedBounds;

	/** Bounds extended by user values below */
		FBoxSphereBounds ExtendedBounds;

protected:
	// The properties below are protected to force the use of the Set* methods for this data
	// in code so we can keep the extended bounds up to date after changing the data.
	// Property editors will trigger property events to correctly recalculate the extended bounds.

	/** Bound extension values in addition to imported bound in the positive direction of XYZ,
	*	positive value increases bound size and negative value decreases bound size.
	*	The final bound would be from [Imported Bound - Negative Bound] to [Imported Bound + Positive Bound]. */
		FVector PositiveBoundsExtension;

	/** Bound extension values in addition to imported bound in the negative direction of XYZ,
	*	positive value increases bound size and negative value decreases bound size.
	*	The final bound would be from [Imported Bound - Negative Bound] to [Imported Bound + Positive Bound]. */
		FVector NegativeBoundsExtension;

public:

	/** Get the extended bounds of this mesh (imported bounds plus bounds extension) */
		 FBoxSphereBounds GetBounds();

	/** Get the original imported bounds of the skel mesh */
		 FBoxSphereBounds GetImportedBounds();

	/** Set the original imported bounds of the skel mesh, will recalculate extended bounds */
	 void SetImportedBounds(const FBoxSphereBounds& InBounds);

	/** Set bound extension values in the positive direction of XYZ, positive value increases bound size */
	 void SetPositiveBoundsExtension(const FVector& InExtension);

	/** Set bound extension values in the negative direction of XYZ, positive value increases bound size */
	 void SetNegativeBoundsExtension(const FVector& InExtension);

	/** Calculate the extended bounds based on the imported bounds and the extension values */
	void CalculateExtendedBounds();

	/** Alters the bounds extension values to fit correctly into the current bounds (so negative values never extend the bounds etc.) */
	void ValidateBoundsExtension();

	/** List of materials applied to this mesh. */
		TArray<FSkeletalMaterial> Materials;

	/** List of bones that should be mirrored. */
		TArray<struct FBoneMirrorInfo> SkelMirrorTable;

		TEnumAsByte<EAxis::Type> SkelMirrorAxis;

		TEnumAsByte<EAxis::Type> SkelMirrorFlipAxis;

	/** Struct containing information for each LOD level, such as materials to use, and when use the LOD. */
		TArray<struct FSkeletalMeshLODInfo> LODInfo;

	/** If true, use 32 bit UVs. If false, use 16 bit UVs to save memory */
		uint32 bUseFullPrecisionUVs : 1;

	/** true if this mesh has ever been simplified with Simplygon. */
		uint32 bHasBeenSimplified : 1;

	/** Whether or not the mesh has vertex colors */
		uint32 bHasVertexColors : 1;


	/** Uses skinned data for collision data. Per poly collision cannot be used for simulation, in most cases you are better off using the physics asset */
		uint32 bEnablePerPolyCollision : 1;

	// Physics data for the per poly collision case. In 99% of cases you will not need this and are better off using simple ragdoll collision (physics asset)
		//class UBodySetup* BodySetup;

	/**
	*	Physics and collision information used for this USkeletalMesh, set up in PhAT.
	*	This is used for per-bone hit detection, accurate bounding box calculation and ragdoll physics for example.
	*/
		//class UPhysicsAsset* PhysicsAsset;

	/**
	* Physics asset whose shapes will be used for shadowing when components have bCastCharacterCapsuleDirectShadow or bCastCharacterCapsuleIndirectShadow enabled.
	* Only spheres and sphyl shapes in the physics asset can be supported.  The more shapes used, the higher the cost of the capsule shadows will be.
	*/
	//UPROPERTY(EditAnywhere, AssetRegistrySearchable, BlueprintReadOnly, Category = Lighting)
		//class UPhysicsAsset* ShadowPhysicsAsset;

#if WITH_EDITORONLY_DATA

	/** Importing data and options used for this mesh */
	//UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* AssetImportData;

	/** Path to the resource used to construct this skeletal mesh */
		FString SourceFilePath_DEPRECATED;

	/** Date/Time-stamp of the file from the last import */
		FString SourceFileTimestamp_DEPRECATED;

	/** Information for thumbnail rendering */
		//class UThumbnailInfo* ThumbnailInfo;

	/** Optimization settings used to simplify LODs of this mesh. */
		TArray<struct FSkeletalMeshOptimizationSettings> OptimizationSettings;

	/* Attached assets component for this mesh */
		//FPreviewAssetAttachContainer PreviewAttachedAssetContainer;

	/**
	* If true on post load we need to calculate resolution independent Display Factors from the
	* loaded LOD screen sizes.
	*/
	uint32 bRequiresLODScreenSizeConversion : 1;

	/**
	* If true on post load we need to calculate resolution independent LOD hysteresis from the
	* loaded LOD hysteresis.
	*/
	uint32 bRequiresLODHysteresisConversion : 1;

#endif // WITH_EDITORONLY_DATA

		TArray<UMorphTarget*> MorphTargets;

	/** A fence which is used to keep track of the rendering thread releasing the static mesh resources. */
	//FRenderCommandFence ReleaseResourcesFence;

	/** New Reference skeleton type **/
	FReferenceSkeleton RefSkeleton;

	/** Map of morph target name to index into USkeletalMesh::MorphTargets**/
	TMap<FName, int32> MorphTargetIndexMap;

	/** Reference skeleton precomputed bases. */
	TArray<FMatrix> RefBasesInvMatrix;

#if WITH_EDITORONLY_DATA
	/** The section currently selected in the Editor. Used for highlighting */
		int32 SelectedEditorSection;

	/** The section currently selected for clothing. need to remember this index for reimporting cloth */
		int32 SelectedClothingSection;

	/** Height offset for the floor mesh in the editor */
		float FloorOffset;

	/** This is buffer that saves pose that is used by retargeting*/
		TArray<FTransform> RetargetBasePose;

#endif

	/** Clothing asset data */
		//TArray<FClothingAssetData>		ClothingAssets;

	/** Animation Blueprint class to run as a post process for this mesh.
	*  This blueprint will be ran before physics, but after the main
	*  anim instance for any skeletal mesh component using this mesh.
	*/
		//TSubclassOf<UAnimInstance> PostProcessAnimBlueprint;

protected:

	/** Array of user data stored with the asset */
		//TArray<UAssetUserData*> AssetUserData;

private:
	/** Skeletal mesh source data */
	class FSkeletalMeshSourceData* SourceData;

	/**
	*	Array of named socket locations, set up in editor and used as a shortcut instead of specifying
	*	everything explicitly to AttachComponent in the SkeletalMeshComponent.
	*/
		TArray<class USkeletalMeshSocket*> Sockets;

	/** Cached matrices from GetComposedRefPoseMatrix */
	TArray<FMatrix> CachedComposedRefPoseMatrices;

public:
	/**
	* Initialize the mesh's render resources.
	*/
	 void InitResources();

	/**
	* Releases the mesh's render resources.
	*/
	 void ReleaseResources();


	/** Release CPU access version of buffer */
	void ReleaseCPUResources();

	/**
	* Update the material UV channel data used by the texture streamer.
	*
	* @param bResetOverrides		True if overridden values should be reset.
	*/
	 void UpdateUVChannelData(bool bResetOverrides);

	/**
	* Returns the UV channel data for a given material index. Used by the texture streamer.
	* This data applies to all lod-section using the same material.
	*
	* @param MaterialIndex		the material index for which to get the data for.
	* @return the data, or null if none exists.
	*/
	 const FMeshUVChannelInfo* GetUVChannelData(int32 MaterialIndex) const;

	/**
	* Gets the center point from which triangles should be sorted, if any.
	*/
	 bool GetSortCenterPoint(FVector& OutSortCenter) const;

	/**
	* Computes flags for building vertex buffers.
	*/
	 uint32 GetVertexBufferFlags() const;

	//~ Begin UObject Interface.
#if WITH_EDITOR
	//virtual void PreEditChange(UProperty* PropertyAboutToChange) ;
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) ;

	virtual void PostEditUndo() ;
	//virtual void GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const override;
#endif // WITH_EDITOR
	virtual void BeginDestroy() ;
	virtual bool IsReadyForFinishDestroy() ;
	virtual void PreSave(const class ITargetPlatform* TargetPlatform);
	virtual void Serialize(FArchive& Ar);
	virtual void PostInitProperties();
	virtual void PostLoad() ;
	//virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual FString GetDesc() ;
	virtual FString GetDetailedInfoInternal() const ;
	virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) ;
	//static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
	//~ End UObject Interface.

	/** Setup-only routines - not concerned with the instance. */

	 void CalculateInvRefMatrices();

	/** Calculate the required bones for a Skeletal Mesh LOD, including possible extra influences */
	 static void CalculateRequiredBones(class FStaticLODModel& LODModel, const struct FReferenceSkeleton& RefSkeleton, const TMap<FBoneIndexType, FBoneIndexType> * BonesToRemove);

	/**
	*	Find a socket object in this SkeletalMesh by name.
	*	Entering NAME_None will return NULL. If there are multiple sockets with the same name, will return the first one.
	*/
	//USkeletalMeshSocket* FindSocket(FName InSocketName) const;

	/**
	*	Find a socket object in this SkeletalMesh by name.
	*	Entering NAME_None will return NULL. If there are multiple sockets with the same name, will return the first one.
	*   Also returns the index for the socket allowing for future fast access via GetSocketByIndex()
	*/
		 //USkeletalMeshSocket* FindSocketAndIndex(FName InSocketName, int32& OutIndex) const;

	/** Returns the number of sockets available. Both on this mesh and it's skeleton. */
	int32 NumSockets() const;

	/** Returns a socket by index. Max index is NumSockets(). The meshes sockets are accessed first, then the skeletons.  */
	//UFUNCTION(BlueprintCallable, Category = "Animation")
		 //USkeletalMeshSocket* GetSocketByIndex(int32 Index) const;

	// @todo document
	 FMatrix GetRefPoseMatrix(int32 BoneIndex) const;

	/**
	*	Get the component orientation of a bone or socket. Transforms by parent bones.
	*/
	 FMatrix GetComposedRefPoseMatrix(FName InBoneName) const;
	 FMatrix GetComposedRefPoseMatrix(int32 InBoneIndex) const;

	/** Allocate and initialise bone mirroring table for this skeletal mesh. Default is source = destination for each bone. */
	void InitBoneMirrorInfo();

	/** Utility for copying and converting a mirroring table from another USkeletalMesh. */
	 void CopyMirrorTableFrom(YSkeletalMesh* SrcMesh);
	 void ExportMirrorTable(TArray<FBoneMirrorExport> &MirrorExportInfo);
	 void ImportMirrorTable(TArray<FBoneMirrorExport> &MirrorExportInfo);

	/**
	*	Utility for checking that the bone mirroring table of this mesh is good.
	*	Return true if mirror table is OK, false if there are problems.
	*	@param	ProblemBones	Output string containing information on bones that are currently bad.
	*/
	 bool MirrorTableIsGood(FString& ProblemBones);

	/**
	* Returns the mesh only socket list - this ignores any sockets in the skeleton
	* Return value is a non-const reference so the socket list can be changed
	*/
	 TArray<USkeletalMeshSocket*>& GetMeshOnlySocketList();

	/**
	* Const version
	* Returns the mesh only socket list - this ignores any sockets in the skeleton
	* Return value is a non-const reference so the socket list can be changed
	*/
	 const TArray<USkeletalMeshSocket*>& GetMeshOnlySocketList() const;

	/**
	* Returns the "active" socket list - all sockets from this mesh plus all non-duplicates from the skeleton
	* Const ref return value as this cannot be modified externally
	*/
	 TArray<USkeletalMeshSocket*> GetActiveSocketList() const;

#if WITH_EDITOR
	/** Retrieves the source model for this skeletal mesh. */
	 FStaticLODModel& GetSourceModel();

	/**
	* Copies off the source model for this skeletal mesh if necessary and returns it. This function should always be called before
	* making destructive changes to the mesh's geometry, e.g. simplification.
	*/
	 FStaticLODModel& PreModifyMesh();

	/**
	* Makes sure all attached objects are valid and removes any that aren't.
	*
	* @return		NumberOfBrokenAssets
	*/
	 int32 ValidatePreviewAttachedObjects();

#endif // #if WITH_EDITOR

	/**
	* Verify SkeletalMeshLOD is set up correctly
	*/
	void DebugVerifySkeletalMeshLOD();

	/**
	* Find a named MorphTarget from the MorphSets array in the SkinnedMeshComponent.
	* This searches the array in the same way as FindAnimSequence
	*
	* @param MorphTargetName Name of MorphTarget to look for.
	*
	* @return Pointer to found MorphTarget. Returns NULL if could not find target with that name.
	*/
	 UMorphTarget* FindMorphTarget(FName MorphTargetName) const;
	 UMorphTarget* FindMorphTargetAndIndex(FName MorphTargetName, int32& OutIndex) const;

	/** if name conflicts, it will overwrite the reference */
	 void RegisterMorphTarget(UMorphTarget* MorphTarget);

	 void UnregisterMorphTarget(UMorphTarget* MorphTarget);

	/** Initialize MorphSets look up table : MorphTargetIndexMap */
	 void InitMorphTargets();

#if WITH_APEX_CLOTHING
	 bool  HasClothSectionsInAllLODs(int AssetIndex);
	 bool	 HasClothSections(int32 LODIndex, int AssetIndex);
	 void	 GetOriginSectionIndicesWithCloth(int32 LODIndex, TArray<uint32>& OutSectionIndices);
	 void	 GetOriginSectionIndicesWithCloth(int32 LODIndex, int32 AssetIndex, TArray<uint32>& OutSectionIndices);
	 void	 GetClothSectionIndices(int32 LODIndex, int32 AssetIndex, TArray<uint32>& OutSectionIndices);
	//moved from ApexClothingUtils because of compile issues
	 void  LoadClothCollisionVolumes(int32 AssetIndex, nvidia::apex::ClothingAsset* ClothingAsset);
	 bool IsMappedClothingLOD(int32 LODIndex, int32 AssetIndex);
	 int32 GetClothAssetIndex(int32 LODIndex, int32 SectionIndex);
	 void BuildApexToUnrealBoneMapping();
#endif

	/**
	* Checks whether the provided section is using APEX cloth. if bCheckCorrespondingSections is true
	* disabled sections will defer to correspond sections to see if they use cloth (non-cloth sections
	* are disabled and another section added when cloth is enabled, using this flag allows for a check
	* on the original section to succeed)
	* @param InSectionIndex Index to check
	* @param bCheckCorrespondingSections Whether to check corresponding sections for disabled sections
	*/
		 bool IsSectionUsingCloth(int32 InSectionIndex, bool bCheckCorrespondingSections = true) const;

	 void CreateBodySetup();
	 //UBodySetup* GetBodySetup();

#if WITH_EDITOR
	/** Trigger a physics build to ensure per poly collision is created */
	 void BuildPhysicsData();
	 void AddBoneToReductionSetting(int32 LODIndex, const TArray<FName>& BoneNames);
	 void AddBoneToReductionSetting(int32 LODIndex, FName BoneName);
#endif

#if WITH_EDITORONLY_DATA
	/** Convert legacy screen size (based on fixed resolution) into screen size (diameter in screen units) */
	void ConvertLegacyLODScreenSize();
#endif


	//~ Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) ;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const ;
	virtual bool WantsNegXTriMesh() 
	{
		return true;
	}
	//~ End Interface_CollisionDataProvider Interface

	//~ Begin IInterface_AssetUserData Interface
	/*virtual void AddAssetUserData(UAssetUserData* InUserData) override;
	virtual void RemoveUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	virtual UAssetUserData* GetAssetUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	virtual const TArray<UAssetUserData*>* GetAssetUserDataArray() const override;
	*///~ End IInterface_AssetUserData Interface

private:

	/** Utility function to help with building the combined socket list */
	bool IsSocketOnMesh(const FName& InSocketName) const;

	/**
	* Flush current render state
	*/
	void FlushRenderState();
	/**
	* Restart render state.
	*/
	void RestartRenderState();

	/**
	* In older data, the bEnableShadowCasting flag was stored in LODInfo
	* so it needs moving over to materials
	*/
	void MoveDeprecatedShadowFlagToMaterials();

	/**
	* Test whether all the flags in an array are identical (could be moved to Array.h?)
	*/
	bool AreAllFlagsIdentical(const TArray<bool>& BoolArray) const;

	/*
	* Ask the reference skeleton to rebuild the NameToIndexMap array. This is use to load old package before this array was created.
	*/
	void RebuildRefSkeletonNameToIndexMap();

	/*
	* In version prior to FEditorObjectVersion::RefactorMeshEditorMaterials
	* The material slot is containing the "Cast Shadow" and the "Recompute Tangent" flag
	* We move those flag to sections to allow artist to control those flag at section level
	* since its a section flag.
	*/
	void MoveMaterialFlagsToSections();
};

