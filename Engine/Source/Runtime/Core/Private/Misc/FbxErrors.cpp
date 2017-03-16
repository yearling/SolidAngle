// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/FbxErrors.h"
	
/** Generic */
YName FFbxErrors::Generic_ImportingNewObjectFailed(TEXT("Generic_ImportingNewObjectFailed"));

YName FFbxErrors::Generic_ReimportingObjectFailed(TEXT("Generic_ReimportingObjectFailed"));

YName FFbxErrors::Generic_LoadingSceneFailed(TEXT("Generic_LoadingSceneFailed"));

YName FFbxErrors::Generic_InvalidCharacterInName(TEXT("Generic_InvalidCharacterInName"));

YName FFbxErrors::Generic_SameNameAssetExists(TEXT("Generic_SameNameAssetExists"));

YName FFbxErrors::Generic_SameNameAssetOverriding(TEXT("Generic_SameNameAssetOverriding"));

YName FFbxErrors::Generic_CannotDeleteReferenced(TEXT("Generic_CannotDeleteReferenced"));

YName FFbxErrors::Generic_FBXFileParseFailed(TEXT("Generic_FBXFileParseFailed"));

YName FFbxErrors::Generic_MeshNotFound(TEXT("Generic_MeshNotFound"));

YName FFbxErrors::Generic_CannotDetectImportType(TEXT("Generic_CannotDetectImportType"));

/** Mesh Generic **/

YName FFbxErrors::Generic_Mesh_NoGeometry(TEXT("Generic_Mesh_NoGeometry"));

YName FFbxErrors::Generic_Mesh_SmallGeometry(TEXT("Generic_Mesh_SmallGeometry"));

YName FFbxErrors::Generic_Mesh_TriangulationFailed(TEXT("Generic_Mesh_TriangulationFailed"));

YName FFbxErrors::Generic_Mesh_ConvertSmoothingGroupFailed(TEXT("Generic_Mesh_ConvertSmoothingGroupFailed"));

YName FFbxErrors::Generic_Mesh_UnsupportingSmoothingGroup(TEXT("Generic_Mesh_UnsupportingSmoothingGroup"));

YName FFbxErrors::Generic_Mesh_MaterialIndexInconsistency(TEXT("Generic_Mesh_MaterialIndexInconsistency"));

YName FFbxErrors::Generic_Mesh_MeshNotFound(TEXT("Generic_Mesh_MeshNotFound"));

YName FFbxErrors::Generic_Mesh_NoSmoothingGroup(TEXT("Generic_Mesh_NoSmoothingGroup"));

YName FFbxErrors::Generic_Mesh_LOD_InvalidIndex(TEXT("Generic_Mesh_LOD_InvalidIndex"));

YName FFbxErrors::Generic_Mesh_LOD_NoFileSelected(TEXT("Generic_Mesh_LOD_NoFileSelected"));

YName FFbxErrors::Generic_Mesh_LOD_MultipleFilesSelected(TEXT("Generic_Mesh_LOD_MultipleFilesSelected"));

YName FFbxErrors::Generic_Mesh_SkinxxNameError(TEXT("Generic_Mesh_SkinxxNameError"));

/** Static Mesh **/
YName FFbxErrors::StaticMesh_TooManyMaterials(TEXT("StaticMesh_TooManyMaterials"));

YName FFbxErrors::StaticMesh_UVSetLayoutProblem(TEXT("StaticMesh_UVSetLayoutProblem"));

YName FFbxErrors::StaticMesh_NoTriangles(TEXT("StaticMesh_NoTriangles"));

YName FFbxErrors::StaticMesh_BuildError(TEXT("StaticMesh_BuildError"));

YName FFbxErrors::StaticMesh_AllTrianglesDegenerate(TEXT("StaticMesh_AllTrianglesDegenerate"));

/** SkeletalMesh **/
YName FFbxErrors::SkeletalMesh_DifferentRoots(TEXT("SkeletalMesh_DifferentRoot"));

YName FFbxErrors::SkeletalMesh_DuplicateBones(TEXT("SkeletalMesh_DuplicateBones"));

YName FFbxErrors::SkeletalMesh_NoInfluences(TEXT("SkeletalMesh_NoInfluences"));

YName FFbxErrors::SkeletalMesh_TooManyInfluences(TEXT("SkeletalMesh_TooManyInfluences"));

YName FFbxErrors::SkeletalMesh_RestoreSortingMismatchedStrips(TEXT("SkeletalMesh_RestoreSortingMismatchedStrips"));

YName FFbxErrors::SkeletalMesh_RestoreSortingNoSectionMatch(TEXT("SkeletalMesh_RestoreSortingNoSectionMatch"));

YName FFbxErrors::SkeletalMesh_RestoreSortingForSectionNumber(TEXT("SkeletalMesh_RestoreSortingForSectionNumber"));

YName FFbxErrors::SkeletalMesh_NoMeshFoundOnRoot(TEXT("SkeletalMesh_NoMeshFoundOnRoot"));

YName FFbxErrors::SkeletalMesh_ImportSubDSurface(TEXT("SkeletalMesh_ImportSubDSurface"));

YName FFbxErrors::SkeletalMesh_InvalidRoot(TEXT("SkeletalMesh_InvalidRoot"));

YName FFbxErrors::SkeletalMesh_InvalidBone(TEXT("SkeletalMesh_InvalidBone"));

YName FFbxErrors::SkeletalMesh_InvalidNode(TEXT("SkeletalMesh_InvalidNode"));

YName FFbxErrors::SkeletalMesh_NoWeightsOnDeformer(TEXT("SkeletalMesh_NoWeightsOnDeformer"));

YName FFbxErrors::SkeletalMesh_NoBindPoseInScene(TEXT("SkeletalMesh_NoBindPoseInScene"));

YName FFbxErrors::SkeletalMesh_NoAssociatedCluster(TEXT("SkeletalMesh_NoAssociatedCluster"));

YName FFbxErrors::SkeletalMesh_NoBoneFound(TEXT("SkeletalMesh_NoBoneFound"));

YName FFbxErrors::SkeletalMesh_InvalidBindPose(TEXT("SkeletalMesh_InvalidBindPose"));

YName FFbxErrors::SkeletalMesh_MultipleRoots(TEXT("SkeletalMesh_MultipleRoots"));

YName FFbxErrors::SkeletalMesh_BonesAreMissingFromBindPose(TEXT("SkeletalMesh_BonesAreMissingFromBindPose"));

YName FFbxErrors::SkeletalMesh_VertMissingInfluences(TEXT("SkeletalMesh_VertMissingInfluences"));

YName FFbxErrors::SkeletalMesh_SectionWithNoTriangle(TEXT("SkeletalMesh_SectionWithNoTriangle"));

YName FFbxErrors::SkeletalMesh_TooManyVertices(TEXT("SkeletalMesh_TooManyVertices"));

YName FFbxErrors::SkeletalMesh_FailedToCreatePhyscisAsset(TEXT("SkeletalMesh_FailedToCreatePhyscisAsset"));

YName FFbxErrors::SkeletalMesh_SkeletonRecreateError(TEXT("SkeletalMesh_SkeletonRecreateError"));

YName FFbxErrors::SkeletalMesh_ExceedsMaxBoneCount(TEXT("SkeletalMesh_ExceedsMaxBoneCount"));

YName FFbxErrors::SkeletalMesh_NoUVSet(TEXT("SkeletalMesh_NoUVSet"));

YName FFbxErrors::SkeletalMesh_LOD_MissingBone(TEXT("SkeletalMesh_LOD_MissingBone"));

YName FFbxErrors::SkeletalMesh_LOD_FailedToImport(TEXT("SkeletalMesh_LOD_FailedToImport"));

YName FFbxErrors::SkeletalMesh_LOD_RootNameIncorrect(TEXT("SkeletalMesh_LOD_RootNameIncorrect"));

YName FFbxErrors::SkeletalMesh_LOD_BonesDoNotMatch(TEXT("SkeletalMesh_LOD_BonesDoNotMatch"));

YName FFbxErrors::SkeletalMesh_LOD_IncorrectParent(TEXT("SkeletalMesh_LOD_IncorrectParent"));

YName FFbxErrors::SkeletalMesh_LOD_HasSoftVerts(TEXT("SkeletalMesh_LOD_HasSoftVerts"));

YName FFbxErrors::SkeletalMesh_LOD_MissingSocketBone(TEXT("SkeletalMesh_LOD_MissingSocketBone"));

YName FFbxErrors::SkeletalMesh_LOD_MissingMorphTarget(TEXT("SkeletalMesh_LOD_MissingMorphTarget"));

YName FFbxErrors::SkeletalMesh_FillImportDataFailed(TEXT("SkeletalMesh_FillImportDataFailed"));

YName FFbxErrors::SkeletalMesh_InvalidPosition(TEXT("SkeletalMesh_InvalidPosition"));

/** Animation **/
YName FFbxErrors::Animation_CouldNotFindRootTrack(TEXT("Animation_CouldNotFindRootTrack"));

YName FFbxErrors::Animation_CouldNotBuildSkeleton(TEXT("Animation_CouldNotBuildSkeleton"));

YName FFbxErrors::Animation_CouldNotFindTrack(TEXT("Animation_CouldNotFindTrack"));

YName FFbxErrors::Animation_ZeroLength(TEXT("Animation_ZeroLength"));

YName FFbxErrors::Animation_RootTrackMismatch(TEXT("Animation_RootTrackMismatch"));

YName FFbxErrors::Animation_DuplicatedBone(TEXT("Animation_DuplicatedBone"));

YName FFbxErrors::Animation_MissingBones(TEXT("Animation_MissingBones"));

YName FFbxErrors::Animation_InvalidData(TEXT("Animation_InvalidData"));

YName FFbxErrors::Animation_TransformError(TEXT("Animation_TransformError"));

static const YString FbxErrorsPath = TEXT("Shared/Editor/FbxErrors");

FFbxErrorToken::FFbxErrorToken(const YName& InErrorName)
	: FDocumentationToken(FbxErrorsPath, FbxErrorsPath, InErrorName.ToString())
{
}
