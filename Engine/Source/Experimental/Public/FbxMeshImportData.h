// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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


/**
* Import data and options used when importing any mesh from FBX
*/

struct UFbxAssetImportData 
{
		FVector ImportTranslation;

		FRotator ImportRotation;

		float ImportUniformScale;

		bool bConvertScene;

		bool bForceFrontXAxis;

		bool bConvertSceneUnit;

		bool bImportAsScene;
};


class UFbxMeshImportData : public UFbxAssetImportData
{
public:
		/** If this option is true the node absolute transform (transform, offset and pivot) will be apply to the mesh vertices. */
		bool bTransformVertexToAbsolute;

	/** - Experimental - If this option is true the inverse node rotation pivot will be apply to the mesh vertices. The pivot from the DCC will then be the origin of the mesh. Note: "TransformVertexToAbsolute" must be false.*/
		bool bBakePivotInVertex;

	/** Enables importing of mesh LODs from FBX LOD groups, if present in the FBX file */
		uint32 bImportMeshLODs : 1;

	/** Enabling this option will read the tangents(tangent,binormal,normal) from FBX file instead of generating them automatically. */
		TEnumAsByte<enum EFBXNormalImportMethod> NormalImportMethod;

	/** Use the MikkTSpace tangent space generator for generating normals and tangents on the mesh */
		TEnumAsByte<enum EFBXNormalGenerationMethod::Type> NormalGenerationMethod;

};
