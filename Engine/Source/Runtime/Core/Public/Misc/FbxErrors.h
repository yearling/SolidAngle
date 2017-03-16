// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "SObject/NameTypes.h"
#include "Templates/SharedPointer.h"
#include "Logging/TokenizedMessage.h"


/**
 * This file contains known map errors that can be referenced by name.
 * Documentation for these errors is assumed to lie in UDN at: Engine\Documentation\Source\Shared\Editor\MapErrors
 */
struct CORE_API FFbxErrors
{
	/** Generic */
	static YName Generic_ImportingNewObjectFailed;

	static YName Generic_ReimportingObjectFailed;

	static YName Generic_LoadingSceneFailed;

	static YName Generic_InvalidCharacterInName;

	static YName Generic_SameNameAssetExists;

	static YName Generic_SameNameAssetOverriding;

	static YName Generic_CannotDeleteReferenced;

	static YName Generic_FBXFileParseFailed;

	static YName Generic_MeshNotFound;

	static YName Generic_CannotDetectImportType;

	/** Mesh Generic **/

	static YName Generic_Mesh_NoGeometry;

	static YName Generic_Mesh_SmallGeometry;

	static YName Generic_Mesh_TriangulationFailed;

	static YName Generic_Mesh_ConvertSmoothingGroupFailed;

	static YName Generic_Mesh_UnsupportingSmoothingGroup;

	static YName Generic_Mesh_MaterialIndexInconsistency;

	static YName Generic_Mesh_MeshNotFound;

	static YName Generic_Mesh_NoSmoothingGroup;

	static YName Generic_Mesh_LOD_InvalidIndex;

	static YName Generic_Mesh_LOD_NoFileSelected;

	static YName Generic_Mesh_LOD_MultipleFilesSelected;

	static YName Generic_Mesh_SkinxxNameError;

	/** Static Mesh **/
	static YName StaticMesh_TooManyMaterials;

	static YName StaticMesh_UVSetLayoutProblem;

	static YName StaticMesh_NoTriangles;

	static YName StaticMesh_BuildError;

	static YName StaticMesh_AllTrianglesDegenerate;

	/** SkeletalMesh **/
	static YName SkeletalMesh_DifferentRoots;

	static YName SkeletalMesh_DuplicateBones;

	static YName SkeletalMesh_NoInfluences;

	static YName SkeletalMesh_TooManyInfluences;

	static YName SkeletalMesh_RestoreSortingMismatchedStrips;

	static YName SkeletalMesh_RestoreSortingNoSectionMatch;

	static YName SkeletalMesh_RestoreSortingForSectionNumber;

	static YName SkeletalMesh_NoMeshFoundOnRoot;
	
	static YName SkeletalMesh_ImportSubDSurface;

	static YName SkeletalMesh_InvalidRoot;

	static YName SkeletalMesh_InvalidBone;

	static YName SkeletalMesh_InvalidNode;

	static YName SkeletalMesh_NoWeightsOnDeformer;

	static YName SkeletalMesh_NoBindPoseInScene;

	static YName SkeletalMesh_NoAssociatedCluster;

	static YName SkeletalMesh_NoBoneFound;

	static YName SkeletalMesh_InvalidBindPose;

	static YName SkeletalMesh_MultipleRoots;

	static YName SkeletalMesh_BonesAreMissingFromBindPose;

	static YName SkeletalMesh_VertMissingInfluences;

	static YName SkeletalMesh_SectionWithNoTriangle;

	static YName SkeletalMesh_TooManyVertices;

	static YName SkeletalMesh_FailedToCreatePhyscisAsset;

	static YName SkeletalMesh_SkeletonRecreateError;

	static YName SkeletalMesh_ExceedsMaxBoneCount;

	static YName SkeletalMesh_NoUVSet;

	static YName SkeletalMesh_LOD_MissingBone;

	static YName SkeletalMesh_LOD_FailedToImport;

	static YName SkeletalMesh_LOD_RootNameIncorrect;

	static YName SkeletalMesh_LOD_BonesDoNotMatch;

	static YName SkeletalMesh_LOD_IncorrectParent;

	static YName SkeletalMesh_LOD_HasSoftVerts;

	static YName SkeletalMesh_LOD_MissingSocketBone;

	static YName SkeletalMesh_LOD_MissingMorphTarget;

	static YName SkeletalMesh_FillImportDataFailed;

	static YName SkeletalMesh_InvalidPosition;

	/** Animation **/
	static YName Animation_CouldNotFindRootTrack;

	static YName Animation_CouldNotBuildSkeleton;

	static YName Animation_CouldNotFindTrack;

	static YName Animation_ZeroLength;

	static YName Animation_RootTrackMismatch;

	static YName Animation_DuplicatedBone;

	static YName Animation_MissingBones;

	static YName Animation_InvalidData;

	static YName Animation_TransformError;
};

/**
 * Map error specific message token.
 */
class FFbxErrorToken : public FDocumentationToken
{
public:
	/** Factory method, tokens can only be constructed as shared refs */
	CORE_API static TSharedRef<FFbxErrorToken> Create( const YName& InErrorName )
	{
		return MakeShareable(new FFbxErrorToken(InErrorName));
	}

private:
	/** Private constructor */
	CORE_API FFbxErrorToken( const YName& InErrorName );
};
