#pragma once
#include "Core.h"
#include "BulkData.h"

enum 
{
	MAX_TEXTURE_COORDS =8 
};

/**
 * Raw mesh data used to construct optimized runtime rendering streams.
 *
 * A note on terminology. Information is stored at various frequencies as defined here:
 *     Face - A single polygon in the mesh. Currently all code assumes this is a triangle but
 *            conceptually any polygon would do.
 *     Corner - Each face has N corners. As all faces are currently triangles, N=3.
 *     Wedge - Properties stored for each corner of each face. Index with FaceIndex * NumCorners + CornerIndex.
 *     Vertex - Properties shared by overlapping wedges of adjacent polygons. Typically these properties
 *              relate to position. Index with VertexIndices[WedgeIndex].
 *
 * Additionally, to ease in backwards compatibility all properties should use only primitive types!
 */

struct YRawMesh
{
	// material index, Array[FaceId] = int32
	TArray<int32> FaceMaterialIndices;
	// smooth mask, Array[FaceId] = uint32
	TArray<uint32> FaceSmoothingMasks;
	// position in local space. Array[VertexID] = float3(x,y,z)
	TArray<FVector> VertexPositions;
	// Index of the vertex at this wedge. Array[WedgeId] = VertexId
	TArray<uint32> WedgeIndices;
	// Tangent, U direction. Array[WedgeId] = float3(x,y,z)
	TArray<FVector> WedgeTangentX;
	// Tangent, V direction. Array[WedgeId] = float3(x,y,z)
	TArray<FVector> WedgeTangentY;
	// Normal. Array[WedgeId] = float3(x,y,z)
	TArray<FVector> WedgeTangentZ;
	// Texture coordinates. Array[UVId][WedgeId]=float2(u,v)
	TArray<FVector2D> WedgeTexCoords[MAX_TEXTURE_COORDS];
	// color, Array[WedgeId] = float4(r,g,b,a)
	TArray<FColor> WedgeColors;

	/**
	* Map from material index -> original material index at import time. It's
	 * valid for this to be empty in which case material index == original
	 * material index.
	 */
	TArray<int32> MaterialIndexToImportIndex;
	void Empty();
	/**
	* Returns true if the mesh contains valid information.
	*  - Validates that stream sizes match.
	*  - Validates that there is at least one texture coordinate.
	*  - Validates that indices are valid positions in the vertex stream.
	*/
	bool IsValid() const;

	/**
	 * Returns true if the mesh contains valid information or slightly invalid information that we can fix.
	 *  - Validates that stream sizes match.
	 *  - Validates that there is at least one texture coordinate.
	 *  - Validates that indices are valid positions in the vertex stream.
	 */
	bool IsValidOrFixable() const;

	FORCEINLINE FVector GetWedgePosition(int32 WedgeIndex) const
	{
		return VertexPositions[WedgeIndices[WedgeIndex]];
	}
	/**
	* Compacts materials by removing any that have no associated triangles.
	* Also updates the material index map.
	 */
	void CompactMaterialIndices();
};

// Bulk data storage for raw meshes
class YRawMeshBulkData
{
	FByteBulkData BulkData;
	FGuid Guid;
	bool bGuidIsHash;
public:
	void Serialize(class FArchive& Ar);
	void SaveRawMesh(struct YRawMesh& InMesh);
	void LoadRawMesh(struct YRawMesh& OutMesh);
	FString GetIdString() const;
	FORCEINLINE bool IsEmpty() { return BulkData.GetBulkDataSize() == 0; }
};
