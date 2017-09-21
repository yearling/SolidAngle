#pragma once
#include "FbxMeshImportData.h"

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

class UFbxStaticMeshImportData : public UFbxMeshImportData
{

		/** The LODGroup to associate with this mesh when it is imported */
		FName StaticMeshLODGroup;

	/** Specify how vertex colors should be imported */
		TEnumAsByte<EVertexColorImportOption::Type> VertexColorImportOption;

	/** Specify override color in the case that VertexColorImportOption is set to Override */
		FColor VertexOverrideColor;

	/** Disabling this option will keep degenerate triangles found.  In general you should leave this option on. */
		uint32 bRemoveDegenerates : 1;

	/** Required for PNT tessellation but can be slow. Recommend disabling for larger meshes. */
		uint32 bBuildAdjacencyBuffer : 1;

		uint32 bBuildReversedIndexBuffer : 1;

		uint32 bGenerateLightmapUVs : 1;

	/** If checked, one convex hull per UCX_ prefixed collision mesh will be generated instead of decomposing into multiple hulls */
		uint32 bOneConvexHullPerUCX : 1;

	/** If checked, collision will automatically be generated (ignored if custom collision is imported or used). */
		uint32 bAutoGenerateCollision : 1;

	/** For static meshes, enabling this option will combine all meshes in the FBX into a single monolithic mesh in Unreal */
		uint32 bCombineMeshes : 1;

	/** Gets or creates fbx import data for the specified static mesh */
	//static UFbxStaticMeshImportData* GetImportDataForStaticMesh(UStaticMesh* StaticMesh, UFbxStaticMeshImportData* TemplateForCreation);

};


