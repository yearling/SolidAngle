#pragma once
#include "Core.h"
/**
* Settings applied when building a mesh.
*/
struct YMeshBuildSettings
{

		/** If true, degenerate triangles will be removed. */
		bool bUseMikkTSpace;

	/** If true, normals in the raw mesh are ignored and recomputed. */
		bool bRecomputeNormals;

	/** If true, tangents in the raw mesh are ignored and recomputed. */
		bool bRecomputeTangents;

	/** If true, degenerate triangles will be removed. */
		bool bRemoveDegenerates;

	/** Required for PNT tessellation but can be slow. Recommend disabling for larger meshes. */
		bool bBuildAdjacencyBuffer;

	/** Required to optimize mesh in mirrored transform. Double index buffer size. */
		bool bBuildReversedIndexBuffer;

	/** If true, Tangents will be stored at 16 bit vs 8 bit precision. */
		bool bUseHighPrecisionTangentBasis;

	/** If true, UVs will be stored at full floating point precision. */
		bool bUseFullPrecisionUVs;

		bool bGenerateLightmapUVs;

		int32 MinLightmapResolution;

		int32 SrcLightmapIndex;

		int32 DstLightmapIndex;

		float BuildScale_DEPRECATED;

	/** The local scale applied when building the mesh */
		FVector BuildScale3D;

	/**
	* Scale to apply to the mesh when allocating the distance field volume texture.
	* The default scale is 1, which is assuming that the mesh will be placed unscaled in the world.
	*/
		float DistanceFieldResolutionScale;

	/**
	* Whether to generate the distance field treating every triangle hit as a front face.
	* When enabled prevents the distance field from being discarded due to the mesh being open, but also lowers Distance Field AO quality.
	*/
		bool bGenerateDistanceFieldAsIfTwoSided;

	/**
	* Adding a constant distance effectively shrinks the distance field representation.
	* This is useful for preventing self shadowing aritfacts when doing some minor ambient animation.
	* Thin walls will be affected more severely than large hollow objects, because thin walls don't have a large negative region.
	*/
		float DistanceFieldBias;

		//class UStaticMesh* DistanceFieldReplacementMesh;

	/** Default settings. */
		YMeshBuildSettings()
		: bUseMikkTSpace(true)
		, bRecomputeNormals(true)
		, bRecomputeTangents(true)
		, bRemoveDegenerates(true)
		, bBuildAdjacencyBuffer(true)
		, bBuildReversedIndexBuffer(true)
		, bUseHighPrecisionTangentBasis(false)
		, bUseFullPrecisionUVs(false)
		, bGenerateLightmapUVs(true)
		, MinLightmapResolution(64)
		, SrcLightmapIndex(0)
		, DstLightmapIndex(1)
		, BuildScale_DEPRECATED(1.0f)
		, BuildScale3D(1.0f, 1.0f, 1.0f)
		, DistanceFieldResolutionScale(1.0f)
		, bGenerateDistanceFieldAsIfTwoSided(false)
		, DistanceFieldBias(0.0f)
		//, DistanceFieldReplacementMesh(NULL)
	{ }

	/** Equality operator. */
	bool operator==(const YMeshBuildSettings& Other) const
	{
		return bRecomputeNormals == Other.bRecomputeNormals
			&& bRecomputeTangents == Other.bRecomputeTangents
			&& bUseMikkTSpace == Other.bUseMikkTSpace
			&& bRemoveDegenerates == Other.bRemoveDegenerates
			&& bBuildAdjacencyBuffer == Other.bBuildAdjacencyBuffer
			&& bBuildReversedIndexBuffer == Other.bBuildReversedIndexBuffer
			&& bUseHighPrecisionTangentBasis == Other.bUseHighPrecisionTangentBasis
			&& bUseFullPrecisionUVs == Other.bUseFullPrecisionUVs
			&& bGenerateLightmapUVs == Other.bGenerateLightmapUVs
			&& MinLightmapResolution == Other.MinLightmapResolution
			&& SrcLightmapIndex == Other.SrcLightmapIndex
			&& DstLightmapIndex == Other.DstLightmapIndex
			&& BuildScale3D == Other.BuildScale3D
			&& DistanceFieldResolutionScale == Other.DistanceFieldResolutionScale
			&& bGenerateDistanceFieldAsIfTwoSided == Other.bGenerateDistanceFieldAsIfTwoSided
			&& DistanceFieldBias == Other.DistanceFieldBias;
			//&& DistanceFieldReplacementMesh == Other.DistanceFieldReplacementMesh;
	}

	/** Inequality. */
	bool operator!=(const YMeshBuildSettings& Other) const
	{
		return !(*this == Other);
	}
};


