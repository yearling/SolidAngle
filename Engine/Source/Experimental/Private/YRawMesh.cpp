#include "YRawMesh.h"
void YRawMesh::Empty()
{
	FaceMaterialIndices.Empty();
	FaceSmoothingMasks.Empty();
	VertexPositions.Empty();
	WedgeIndices.Empty();
	WedgeTangentX.Empty();
	WedgeTangentY.Empty();
	WedgeTangentZ.Empty();
	WedgeColors.Empty();
	for (int32 i = 0; i < MAX_YTEXTURE_COORDS; ++i)
	{
		WedgeTexCoords[i].Empty();
	}
}
template <typename ArrayType>
bool ValidateArraySize(ArrayType const& Array, int32 ExpectedSize)
{
	return Array.Num() == 0 || Array.Num() == ExpectedSize;
}

bool YRawMesh::IsValid() const
{
	int32 NumVertices = VertexPositions.Num();
	int32 NumWedges = WedgeIndices.Num();
	int32 NumFaces = NumWedges / 3;
	bool bValid = NumVertices > 0
		&& NumWedges > 0
		&& NumFaces > 0
		&& (NumWedges / 3) == NumFaces
		&& ValidateArraySize(FaceMaterialIndices, NumFaces)
		&& ValidateArraySize(FaceSmoothingMasks, NumFaces)
		&& ValidateArraySize(WedgeTangentX, NumWedges)
		&& ValidateArraySize(WedgeTangentY, NumWedges)
		&& ValidateArraySize(WedgeTangentZ, NumWedges)
		&& ValidateArraySize(WedgeColors, NumWedges)
		&& WedgeTexCoords[0].Num() == NumWedges;
	for (int32 TexCoordIndex = 1; TexCoordIndex < MAX_YTEXTURE_COORDS; ++TexCoordIndex)
	{
		bValid = bValid && ValidateArraySize(WedgeTexCoords[TexCoordIndex], NumWedges);
	}
	int32 WedgeIndex = 0;
	while (bValid && WedgeIndex < NumWedges)
	{
		bValid = bValid && (WedgeIndices[WedgeIndex] < (uint32)NumVertices);
		WedgeIndex++;
	}
	return bValid;
}

bool YRawMesh::IsValidOrFixable() const
{
	int32 NumVertices = VertexPositions.Num();
	int32 NumWedges = WedgeIndices.Num();
	int32 NumFaces = NumWedges / 3;
	int32 NumTexCoords = WedgeTexCoords[0].Num();
	int32 NumFaceSmoothingMasks = FaceSmoothingMasks.Num();
	int32 NumFaceMaterialIndices = FaceMaterialIndices.Num();

	bool bValidOrFixable = NumVertices > 0
		&& NumWedges > 0
		&& NumFaces > 0
		&& (NumWedges / 3) == NumFaces
		&& NumFaceMaterialIndices == NumFaces
		&& NumFaceSmoothingMasks == NumFaces
		&& ValidateArraySize(WedgeColors, NumWedges)
		// All meshes must have a valid texture coordinate.
		&& NumTexCoords == NumWedges;

	for (int32 TexCoordIndex = 1; TexCoordIndex < MAX_YTEXTURE_COORDS; ++TexCoordIndex)
	{
		bValidOrFixable = bValidOrFixable && ValidateArraySize(WedgeTexCoords[TexCoordIndex], NumWedges);
	}

	int32 WedgeIndex = 0;
	while (bValidOrFixable && WedgeIndex < NumWedges)
	{
		bValidOrFixable = bValidOrFixable && (WedgeIndices[WedgeIndex] < (uint32)NumVertices);
		WedgeIndex++;
	}

	return bValidOrFixable;
}

void YRawMesh::CompactMaterialIndices()
{
	MaterialIndexToImportIndex.Reset();
	if (IsValidOrFixable())
	{
		// Count the number of triangles per section. store in NumTrianglePerSection[ FaceMaterialIndices[FaceIndex] ] 
		TArray<int32, TInlineAllocator<8>> NumTrianglePerSection;
		int32 NumFaces = FaceMaterialIndices.Num();
		for (int32 FaceIndex = 0; FaceIndex < NumFaces; ++FaceIndex)
		{
			int32 MaterialIndex = FaceMaterialIndices[FaceIndex];
			if (MaterialIndex >= NumTrianglePerSection.Num())
			{
				NumTrianglePerSection.AddZeroed(MaterialIndex - NumTrianglePerSection.Num() + 1);
			}
			if (MaterialIndex >= 0)
			{
				NumTrianglePerSection[MaterialIndex]++;
			}
		}
		// Identify non-zero sections and assign new materials.
		TArray<int32, TInlineAllocator<8>> ImportIndexToMaterialIndex;
		for (int32 SectionIndex = 0; SectionIndex < NumTrianglePerSection.Num(); ++SectionIndex)
		{
			int32 NewMaterialIndex = INDEX_NONE;
			if (NumTrianglePerSection[SectionIndex] > 0)
			{
				NewMaterialIndex = MaterialIndexToImportIndex.Add(SectionIndex);
			}
			ImportIndexToMaterialIndex.Add(NewMaterialIndex);
		}
		// If some sections will be removed, remap material indices for each face.
		if (MaterialIndexToImportIndex.Num() != ImportIndexToMaterialIndex.Num())
		{
			for (int32 FaceIndex = 0; FaceIndex < NumFaces; ++FaceIndex)
			{
				FaceMaterialIndices[FaceIndex] = ImportIndexToMaterialIndex[FaceMaterialIndices[FaceIndex]];
			}
		}
		else
		{
			MaterialIndexToImportIndex.Reset();
		}
	}
}
enum 
{
	RAW_MESH_VERSION = 0
};
FArchive& operator<<(FArchive& Ar, YRawMesh& RawMesh)
{
	int32 Version = RAW_MESH_VERSION;
	Ar << Version;
	/**
	* Serialization should use the raw mesh version not the archive version.
	* Additionally, stick to serializing basic types and arrays of basic types.
	*/
	Ar << RawMesh.FaceMaterialIndices;
	Ar << RawMesh.FaceSmoothingMasks;
	Ar << RawMesh.VertexPositions;
	Ar << RawMesh.WedgeIndices;
	Ar << RawMesh.WedgeTangentX;
	Ar << RawMesh.WedgeTangentY;
	Ar << RawMesh.WedgeTangentZ;
	for (int32 i = 0; i < MAX_YTEXTURE_COORDS; ++i)
	{
		Ar << RawMesh.WedgeTexCoords[i];
	}
	Ar << RawMesh.WedgeColors;
	Ar << RawMesh.MaterialIndexToImportIndex;
	return Ar;
}

void YRawMeshBulkData::Serialize(class FArchive& Ar)
{
	BulkData.Serialize(Ar, nullptr);
	Ar << Guid;
	Ar << bGuidIsHash;
}

void YRawMeshBulkData::SaveRawMesh(struct YRawMesh& InMesh)
{
	TArray<uint8> TempBytes;
	FMemoryWriter Ar(TempBytes, true);
	Ar << InMesh;
	BulkData.Lock(LOCK_READ_WRITE);
	uint8* Dest = (uint8*)BulkData.Realloc(TempBytes.Num());
	FMemory::Memcpy(Dest, TempBytes.GetData(), TempBytes.Num());
	BulkData.Unlock();
	FPlatformMisc::CreateGuid(Guid);
}

void YRawMeshBulkData::LoadRawMesh(struct YRawMesh& OutMesh)
{
	OutMesh.Empty();
	if (BulkData.GetElementCount() > 0)
	{
		FBufferReader Ar(BulkData.Lock(LOCK_READ_ONLY), BulkData.GetElementCount(), false, true);
		Ar << OutMesh;
		BulkData.Unlock();
	}
}

FString YRawMeshBulkData::GetIdString() const
{
	FString GuidString = Guid.ToString();
	if (bGuidIsHash)
	{
		GuidString += TEXT("X");
	}
	return GuidString;
}
