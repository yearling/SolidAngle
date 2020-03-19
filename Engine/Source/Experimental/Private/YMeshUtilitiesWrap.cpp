#include "YMeshUtilitiesWrap.h"
#include "NvTriStrip.h"
#include "forsythtriangleorderoptimizer.h"


nv::Vertex YStaticMeshNvRenderBuffer::getVertex(unsigned int Index) const
{
	nv::Vertex Vertex;

	check(Index < PositionVertexBuffer.GetNumVertices());

	const FVector& Position = PositionVertexBuffer.VertexPosition(Index);
	Vertex.pos.x = Position.X;
	Vertex.pos.y = Position.Y;
	Vertex.pos.z = Position.Z;

	if (VertexBuffer.GetNumTexCoords())
	{
		const FVector2D UV = VertexBuffer.GetVertexUV(Index, 0);
		Vertex.uv.x = UV.X;
		Vertex.uv.y = UV.Y;
	}
	else
	{
		Vertex.uv.x = 0.0f;
		Vertex.uv.y = 0.0f;
	}

	return Vertex;
}

namespace NvTriStrip
{
	void GenerateStrips(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		PrimitiveGroup** PrimGroups,
		uint32* NumGroups
	)
	{
		if (Is32Bit)
		{
			GenerateStrips((uint32*)Indices, NumIndices, PrimGroups, NumGroups);
		}
		else
		{
			// convert to 32 bit
			uint32 Idx;
			TArray<uint32> NewIndices;
			NewIndices.AddUninitialized(NumIndices);
			for (Idx = 0; Idx < NumIndices; ++Idx)
			{
				NewIndices[Idx] = ((uint16*)Indices)[Idx];
			}
			GenerateStrips(NewIndices.GetData(), NumIndices, PrimGroups, NumGroups);
		}

	}
}

namespace Forsyth
{
	void OptimizeFaces(
		const uint8* Indices,
		bool Is32Bit,
		const uint32 NumIndices,
		uint32 NumVertices,
		uint32* OutIndices,
		uint16 CacheSize
	)
	{
		if (Is32Bit)
		{
			OptimizeFaces((uint32*)Indices, NumIndices, NumVertices, OutIndices, CacheSize);
		}
		else
		{
			// convert to 32 bit
			uint32 Idx;
			TArray<uint32> NewIndices;
			NewIndices.AddUninitialized(NumIndices);
			for (Idx = 0; Idx < NumIndices; ++Idx)
			{
				NewIndices[Idx] = ((uint16*)Indices)[Idx];
			}
			OptimizeFaces(NewIndices.GetData(), NumIndices, NumVertices, OutIndices, CacheSize);
		}

	}
}
