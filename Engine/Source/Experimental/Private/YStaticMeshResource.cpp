#include "YStaticMeshResource.h"
#include "YYUTHelper.h"
#include "YRHI.h"


YPositionVertexBuffer::YPositionVertexBuffer()
	:Data(nullptr),
	Stride(0),
	NumVertices(0)
{

}

YPositionVertexBuffer::~YPositionVertexBuffer()
{
	CleanUp();
}

void YPositionVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		VertexData = nullptr;
	}
}

void YPositionVertexBuffer::Init(const TArray<YStaticMeshBuildVertex>& InVertices)
{
	NumVertices = InVertices.Num();
	AllocateData(true);
	VertexData->ResizeBuffer(NumVertices);
	Data = VertexData->GetDataPointer();
	for (int32 VertexIndex = 0; VertexIndex < InVertices.Num(); ++VertexIndex)
	{
		const YStaticMeshBuildVertex& SourceVertex = InVertices[VertexIndex];
		const uint32 DestVertexIndex = VertexIndex;
		VertexPosition(DestVertexIndex) = SourceVertex.Position;
	}
}

void YPositionVertexBuffer::InitRHI()
{
	FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
	if (ResourceArray->GetResourceDataSize())
	{
		YRHIResourceCreateInfo RHICreateInfo(ResourceArray);
		VertexBufferRHI = CreateVertexBuffer(ResourceArray->GetResourceDataSize(), YBUF_Static, RHICreateInfo);
	}
}

void YPositionVertexBuffer::Serialize(FArchive& Ar, bool bNeedsCPUAccess)
{
	Ar << Stride << NumVertices;
	if (Ar.IsLoading())
	{
		AllocateData(bNeedsCPUAccess);
	}
	if (VertexData)
	{
		VertexData->Serialize(Ar);
		Data = VertexData->GetDataPointer();
	}
}

void YPositionVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{
	CleanUp();
	VertexData = MakeUnique<YPositionVertexData>(bNeedsCPUAccess);
	Stride = VertexData->GetStride();
}

YStaticMeshTangentUVVertexBuffer::YStaticMeshTangentUVVertexBuffer()
{

}

YStaticMeshTangentUVVertexBuffer::~YStaticMeshTangentUVVertexBuffer()
{

}

void YStaticMeshTangentUVVertexBuffer::CleanUp()
{
	if (VertexData)
	{
		VertexData = nullptr;
	}
}

void YStaticMeshTangentUVVertexBuffer::Init(const TArray<YStaticMeshBuildVertex>& InVertices)
{
	NumVertices = InVertices.Num();
	AllocateData();
	VertexData->ResizeBuffer(NumVertices);
	Data = VertexData->GetDataPointer();
	for (uint32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
	{
		const YStaticMeshBuildVertex & SourceVertex = InVertices[VertexIndex];
		const uint32 DesetVertexIndex = VertexIndex;
		SetVertexTangents(DesetVertexIndex, SourceVertex.TangentX, SourceVertex.TangentY, SourceVertex.TangentZ);
		for (uint32 UVIndex = 0; UVIndex < 2; UVIndex++)
		{
			SetVertexUV(DesetVertexIndex, UVIndex, SourceVertex.UVs[UVIndex]);
		}
	}
}

void YStaticMeshTangentUVVertexBuffer::Serialize(FArchive& Ar, bool bNeedsCPUAccess)
{
	Ar << Stride << NumVertices;
	if (Ar.IsLoading())
	{
		AllocateData(bNeedsCPUAccess);
	}
	if (VertexData)
	{
		VertexData->Serialize(Ar);
		Data = VertexData->GetDataPointer();
	}
}

void YStaticMeshTangentUVVertexBuffer::InitRHI()
{
	FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
	if (ResourceArray->GetResourceDataSize())
	{
		YRHIResourceCreateInfo RHICreateInfo(ResourceArray);
		VertexBufferRHI = CreateVertexBuffer(ResourceArray->GetResourceDataSize(), YBUF_Static, RHICreateInfo);
	}
}

void YStaticMeshTangentUVVertexBuffer::AllocateData(bool bNeedsCPUAccess /*= true*/)
{
	CleanUp();
	VertexData = MakeUnique<YStaticMeshTangentUVVertexData>(bNeedsCPUAccess);
	Stride = VertexData->GetStride();
}

YRawStaticIndexBuffer::YRawStaticIndexBuffer(bool InNeedsCPUAccess /*= false*/)
	:IndexStorage(InNeedsCPUAccess),b32Bit(false)
{

}

void YRawStaticIndexBuffer::SetIndices(const TArray<uint32>& InIndices, EYIndexBufferStride::Type DesiredStride)
{
	int32 NumIndices = InIndices.Num();
	bool bShouldUsed32Bit = false;
	if (DesiredStride == EYIndexBufferStride::Force32Bit)
	{
		bShouldUsed32Bit = true;
	}
	else if (DesiredStride == EYIndexBufferStride::AutoDetect)
	{
		int32 i = 0;
		while (!bShouldUsed32Bit && i < NumIndices)
		{
			bShouldUsed32Bit = InIndices[i] > MAX_uint16;
			i++;
		}
	}
	int32 IndexStride = bShouldUsed32Bit ? sizeof(uint32) : sizeof(uint16);
	IndexStorage.Empty(IndexStride*NumIndices);
	IndexStorage.AddUninitialized(IndexStride*NumIndices);
	if (bShouldUsed32Bit)
	{
		// If the indices are 32 bit we can just do a memcpy.
		check(IndexStorage.Num() == InIndices.Num() * InIndices.GetTypeSize());
		FMemory::Memcpy(IndexStorage.GetData(), InIndices.GetData(), IndexStorage.Num());
	}
	else
	{
		// Copy element by element demoting 32-bit integers to 16-bit.
		check(IndexStorage.Num() == InIndices.Num() * sizeof(uint16));
		uint16* DestIndices16Bit = (uint16*)IndexStorage.GetData();
		for (int32 i = 0; i < NumIndices; ++i)
		{
			DestIndices16Bit[i] = InIndices[i];
		}
		b32Bit = false;
	}
}

void YRawStaticIndexBuffer::GetCopy(TArray<uint32>& OutIndices) const
{
	int32 NumIndices = b32Bit ? (IndexStorage.Num() / 4) : (IndexStorage.Num() / 2);
	OutIndices.Empty(NumIndices);
	OutIndices.AddUninitialized(NumIndices);

	if (b32Bit)
	{
		// If the indices are 32 bit we can just do a memcpy.
		check(IndexStorage.Num() == OutIndices.Num() * OutIndices.GetTypeSize());
		FMemory::Memcpy(OutIndices.GetData(), IndexStorage.GetData(), IndexStorage.Num());
	}
	else
	{
		// Copy element by element promoting 16-bit integers to 32-bit.
		check(IndexStorage.Num() == OutIndices.Num() * sizeof(uint16));
		const uint16* SrcIndices16Bit = (const uint16*)IndexStorage.GetData();
		for (int32 i = 0; i < NumIndices; ++i)
		{
			OutIndices[i] = SrcIndices16Bit[i];
		}
	}
}

YIndexArrayView YRawStaticIndexBuffer::GetArrayView() const
{
	int32 NumIndices = b32Bit ? (IndexStorage.Num() / 4) : (IndexStorage.Num() / 2);
	return YIndexArrayView(IndexStorage.GetData(), NumIndices, b32Bit);
}

void YRawStaticIndexBuffer::Serialize(FArchive& Ar, bool bNeedsCPUAccess)
{
	IndexStorage.SetAllowCPUAccess(bNeedsCPUAccess);
	Ar << b32Bit;
	IndexStorage.BulkSerialize(Ar);
}

void YRawStaticIndexBuffer::InitRHI()
{
	uint32 IndexStride = b32Bit ? sizeof(uint32) : sizeof(uint16);
	uint32 SizeInBytes = IndexStorage.Num();
	if (SizeInBytes > 0)
	{
		YRHIResourceCreateInfo CreateInfo(&IndexStorage);
		IndexBufferRHI = CreateIndexBuffer(IndexStride, SizeInBytes, YBUF_Static, CreateInfo);
	}
}
