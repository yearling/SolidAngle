// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FbxMeshImportData.h"

class USkeletalMesh;

/**
 * Import data and options used when importing a static mesh from fbx
 */
class UFbxSkeletalMeshImportData : public UFbxMeshImportData
{
public:
	/** Enable this option to update Skeleton (of the mesh)'s reference pose. Mesh's reference pose is always updated.  */
	uint32 bUpdateSkeletonReferencePose:1;

	/** Enable this option to use frame 0 as reference pose */
	uint32 bUseT0AsRefPose:1;

	/** If checked, triangles with non-matching smoothing groups will be physically split. */
	uint32 bPreserveSmoothingGroups:1;

	/** If checked, meshes nested in bone hierarchies will be imported instead of being converted to bones. */
	uint32 bImportMeshesInBoneHierarchy:1;

	/** True to import morph target meshes from the FBX file */
	uint32 bImportMorphTargets:1;

	/** If checked, do not filter same vertices. Keep all vertices even if they have exact same properties*/
	uint32 bKeepOverlappingVertices:1;

	/** Gets or creates fbx import data for the specified skeletal mesh */
};
