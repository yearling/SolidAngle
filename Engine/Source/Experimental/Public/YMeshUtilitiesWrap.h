#pragma once
#include "Core.h"
#include "YMeshCommon.h"
#include "YStaticMeshResource.h"
#include "nvtess.h"
#include "NvTriStrip.h"
struct YMeshEdge
{
	int32	Vertices[2];
	int32	Faces[2];
};

/**
* This helper class builds the edge list for a mesh. It uses a hash of vertex
* positions to edges sharing that vertex to remove the n^2 searching of all
* previously added edges. This class is templatized so it can be used with
* either static mesh or skeletal mesh vertices
*/
template <class VertexClass> class YEdgeBuilder
{
protected:
	/**
	* The list of indices to build the edge data from
	*/
	const TArray<uint32>& Indices;
	/**
	* The array of verts for vertex position comparison
	*/
	const TArray<VertexClass>& Vertices;
	/**
	* The array of edges to create
	*/
	TArray<YMeshEdge>& Edges;
	/**
	* List of edges that start with a given vertex
	*/
	TMultiMap<FVector, YMeshEdge*> VertexToEdgeList;

	/**
	* This function determines whether a given edge matches or not. It must be
	* provided by derived classes since they have the specific information that
	* this class doesn't know about (vertex info, influences, etc)
	*
	* @param Index1 The first index of the edge being checked
	* @param Index2 The second index of the edge
	* @param OtherEdge The edge to compare. Was found via the map
	*
	* @return true if the edge is a match, false otherwise
	*/
	virtual bool DoesEdgeMatch(int32 Index1, int32 Index2, YMeshEdge* OtherEdge) = 0;

	/**
	* Searches the list of edges to see if this one matches an existing and
	* returns a pointer to it if it does
	*
	* @param Index1 the first index to check for
	* @param Index2 the second index to check for
	*
	* @return NULL if no edge was found, otherwise the edge that was found
	*/
	inline YMeshEdge* FindOppositeEdge(int32 Index1, int32 Index2)
	{
		YMeshEdge* Edge = NULL;
		TArray<YMeshEdge*> EdgeList;
		// Search the hash for a corresponding vertex
		VertexToEdgeList.MultiFind(Vertices[Index2].Position, EdgeList);
		// Now search through the array for a match or not
		for (int32 EdgeIndex = 0; EdgeIndex < EdgeList.Num() && Edge == NULL;
			EdgeIndex++)
		{
			YMeshEdge* OtherEdge = EdgeList[EdgeIndex];
			// See if this edge matches the passed in edge
			if (OtherEdge != NULL && DoesEdgeMatch(Index1, Index2, OtherEdge))
			{
				// We have a match
				Edge = OtherEdge;
			}
		}
		return Edge;
	}

	/**
	* Updates an existing edge if found or adds the new edge to the list
	*
	* @param Index1 the first index in the edge
	* @param Index2 the second index in the edge
	* @param Triangle the triangle that this edge was found in
	*/
	inline void AddEdge(int32 Index1, int32 Index2, int32 Triangle)
	{
		// If this edge matches another then just fill the other triangle
		// otherwise add it
		YMeshEdge* OtherEdge = FindOppositeEdge(Index1, Index2);
		if (OtherEdge == NULL)
		{
			// Add a new edge to the array
			int32 EdgeIndex = Edges.AddZeroed();
			Edges[EdgeIndex].Vertices[0] = Index1;
			Edges[EdgeIndex].Vertices[1] = Index2;
			Edges[EdgeIndex].Faces[0] = Triangle;
			Edges[EdgeIndex].Faces[1] = -1;
			// Also add this edge to the hash for faster searches
			// NOTE: This relies on the array never being realloced!
			VertexToEdgeList.Add(Vertices[Index1].Position, &Edges[EdgeIndex]);
		}
		else
		{
			OtherEdge->Faces[1] = Triangle;
		}
	}

public:
	/**
	* Initializes the values for the code that will build the mesh edge list
	*/
	YEdgeBuilder(const TArray<uint32>& InIndices,
		const TArray<VertexClass>& InVertices,
		TArray<YMeshEdge>& OutEdges) :
		Indices(InIndices), Vertices(InVertices), Edges(OutEdges)
	{
		// Presize the array so that there are no extra copies being done
		// when adding edges to it
		Edges.Empty(Indices.Num());
	}

	/**
	* Virtual dtor
	*/
	virtual ~YEdgeBuilder() {}


	/**
	* Uses a hash of indices to edge lists so that it can avoid the n^2 search
	* through the full edge list
	*/
	void FindEdges(void)
	{
		// @todo Handle something other than trilists when building edges
		int32 TriangleCount = Indices.Num() / 3;
		int32 EdgeCount = 0;
		// Work through all triangles building the edges
		for (int32 Triangle = 0; Triangle < TriangleCount; Triangle++)
		{
			// Determine the starting index
			int32 TriangleIndex = Triangle * 3;
			// Get the indices for the triangle
			int32 Index1 = Indices[TriangleIndex];
			int32 Index2 = Indices[TriangleIndex + 1];
			int32 Index3 = Indices[TriangleIndex + 2];
			// Add the first to second edge
			AddEdge(Index1, Index2, Triangle);
			// Now add the second to third
			AddEdge(Index2, Index3, Triangle);
			// Add the third to first edge
			AddEdge(Index3, Index1, Triangle);
		}
	}
};

/**
* This is the static mesh specific version for finding edges
*/
class FStaticMeshEdgeBuilder : public YEdgeBuilder<YStaticMeshBuildVertex>
{
public:
	/**
	* Constructor that passes all work to the parent class
	*/
	FStaticMeshEdgeBuilder(const TArray<uint32>& InIndices,
		const TArray<YStaticMeshBuildVertex>& InVertices,
		TArray<YMeshEdge>& OutEdges) :
		YEdgeBuilder<YStaticMeshBuildVertex>(InIndices, InVertices, OutEdges)
	{
	}

	/**
	* This function determines whether a given edge matches or not for a static mesh
	*
	* @param Index1 The first index of the edge being checked
	* @param Index2 The second index of the edge
	* @param OtherEdge The edge to compare. Was found via the map
	*
	* @return true if the edge is a match, false otherwise
	*/
	bool DoesEdgeMatch(int32 Index1, int32 Index2, YMeshEdge* OtherEdge)
	{
		return Vertices[OtherEdge->Vertices[1]].Position == Vertices[Index1].Position &&
			OtherEdge->Faces[1] == -1;
	}
};



/*------------------------------------------------------------------------------
NVTessLib for computing adjacency used for tessellation.
------------------------------------------------------------------------------*/

/**
* Provides static mesh render data to the NVIDIA tessellation library.
*/
class YStaticMeshNvRenderBuffer : public nv::RenderBuffer
{
public:

	/** Construct from static mesh render buffers. */
	YStaticMeshNvRenderBuffer(
		const YPositionVertexBuffer& InPositionVertexBuffer,
		const YStaticMeshTangentUVVertexBuffer& InVertexBuffer,
		const TArray<uint32>& Indices)
		: PositionVertexBuffer(InPositionVertexBuffer)
		, VertexBuffer(InVertexBuffer)
	{
		check(PositionVertexBuffer.GetNumVertices() == VertexBuffer.GetNumVertices());
		mIb = new nv::IndexBuffer((void*)Indices.GetData(), nv::IBT_U32, Indices.Num(), false);
	}

	/** Retrieve the position and first texture coordinate of the specified index. */
	virtual nv::Vertex getVertex(unsigned int Index) const;

private:

	/** The position vertex buffer for the static mesh. */
	const YPositionVertexBuffer& PositionVertexBuffer;

	/** The vertex buffer for the static mesh. */
	const YStaticMeshTangentUVVertexBuffer& VertexBuffer;

	/** Copying is forbidden. */
	YStaticMeshNvRenderBuffer(const YStaticMeshNvRenderBuffer&);
	YStaticMeshNvRenderBuffer& operator=(const YStaticMeshNvRenderBuffer&);
};
namespace NvTriStrip
{
	/**
	* Converts 16 bit indices to 32 bit prior to passing them into the real GenerateStrips util method
	*/
	void GenerateStrips(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		PrimitiveGroup** PrimGroups,
		uint32* NumGroups
	);

	/**
	* Orders a triangle list for better vertex cache coherency.
	*
	* *** WARNING: This is safe to call for multiple threads IF AND ONLY IF all
	* threads call SetListsOnly(true) and SetCacheSize(CACHESIZE_GEFORCE3). If
	* NvTriStrip is ever used with different settings the library will need
	* some modifications to be thread-safe. ***
	*/
	template<typename IndexDataType, typename Allocator>
	void CacheOptimizeIndexBuffer(TArray<IndexDataType, Allocator>& Indices)
	{
		static_assert(sizeof(IndexDataType) == 2 || sizeof(IndexDataType) == 4, "Indices must be short or int.");

		PrimitiveGroup*	PrimitiveGroups = NULL;
		uint32			NumPrimitiveGroups = 0;
		bool Is32Bit = sizeof(IndexDataType) == 4;

		SetListsOnly(true);
		SetCacheSize(CACHESIZE_GEFORCE3);

		GenerateStrips((uint8*)Indices.GetData(), Is32Bit, Indices.Num(), &PrimitiveGroups, &NumPrimitiveGroups);

		Indices.Empty();
		Indices.AddUninitialized(PrimitiveGroups->numIndices);

		if (Is32Bit)
		{
			FMemory::Memcpy(Indices.GetData(), PrimitiveGroups->indices, Indices.Num() * sizeof(IndexDataType));
		}
		else
		{
			for (uint32 I = 0; I < PrimitiveGroups->numIndices; ++I)
			{
				Indices[I] = (uint16)PrimitiveGroups->indices[I];
			}
		}

		delete[] PrimitiveGroups;
	}
}

namespace Forsyth
{
	/**
	* Converts 16 bit indices to 32 bit prior to passing them into the real OptimizeFaces util method
	*/
	void OptimizeFaces(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		uint32 NumVertices,
		uint32* OutIndices,
		uint16 CacheSize
	);
	//
		/**
		* Orders a triangle list for better vertex cache coherency.
		*/
	template<typename IndexDataType, typename Allocator>
	void CacheOptimizeIndexBuffer(TArray<IndexDataType, Allocator>& Indices)
	{
		static_assert(sizeof(IndexDataType) == 2 || sizeof(IndexDataType) == 4, "Indices must be short or int.");
		bool Is32Bit = sizeof(IndexDataType) == 4;

		// Count the number of vertices
		uint32 NumVertices = 0;
		for (int32 Index = 0; Index < Indices.Num(); ++Index)
		{
			if (Indices[Index] > NumVertices)
			{
				NumVertices = Indices[Index];
			}
		}
		NumVertices += 1;

		TArray<uint32> OptimizedIndices;
		OptimizedIndices.AddUninitialized(Indices.Num());
		uint16 CacheSize = 32;
		OptimizeFaces((uint8*)Indices.GetData(), Is32Bit, Indices.Num(), NumVertices, OptimizedIndices.GetData(), CacheSize);

		if (Is32Bit)
		{
			FMemory::Memcpy(Indices.GetData(), OptimizedIndices.GetData(), Indices.Num() * sizeof(IndexDataType));
		}
		else
		{
			for (int32 I = 0; I < OptimizedIndices.Num(); ++I)
			{
				Indices[I] = (uint16)OptimizedIndices[I];
			}
		}
	}
}

