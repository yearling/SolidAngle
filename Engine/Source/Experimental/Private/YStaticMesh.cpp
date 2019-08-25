#include "YStaticMesh.h"
#include "YRawMesh.h"

YStaticMesh::YStaticMesh()
{

}

bool operator==(const YMeshSectionInfo& A, const YMeshSectionInfo& B)
{
	return A.MaterialIndex == B.MaterialIndex
		&& A.bCastShadow == B.bCastShadow
		&& A.bEnableCollision == B.bEnableCollision;
}

bool operator!=(const YMeshSectionInfo& A, const YMeshSectionInfo& B)
{
	return !(A == B);
}

static uint32 GetMeshMaterialKey(int32 LODIndex, int32 SectionIndex)
{
	return ((LODIndex & 0xffff) << 16) | (SectionIndex & 0xffff);
}

void YMeshSectionInfoMap::Clear()
{
	Map.Empty();
}

int32 YMeshSectionInfoMap::GetSectionNumber(int32 LODIndex) const
{
	int32 SectionCount = 0;
	for (auto& Kvp : Map)
	{
		if (((Kvp.Key & 0xffff) >> 16) == LODIndex)
		{
			++SectionCount;
		}
	}
	return SectionCount;
}

YMeshSectionInfo YMeshSectionInfoMap::Get(int32 LODIndex, int32 SectionIndex) const
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	const YMeshSectionInfo* InfoPtr = Map.Find(Key);
	if (InfoPtr == nullptr)
	{
		Key = GetMeshMaterialKey(0, SectionIndex);
		InfoPtr = Map.Find(Key);
	}
	if (InfoPtr != nullptr)
	{
		return *InfoPtr;
	}
	return YMeshSectionInfo(SectionIndex);
}

void YMeshSectionInfoMap::Set(int32 LODIndex, int32 SectionIndex, const YMeshSectionInfo& Info)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.Add(Key, Info);
}

void YMeshSectionInfoMap::Remove(int32 LODIndex, int32 SectionIndex)
{
	uint32 Key = GetMeshMaterialKey(LODIndex, SectionIndex);
	Map.Remove(Key);
}

void YMeshSectionInfoMap::CopyFrom(const YMeshSectionInfoMap& Other)
{
	for (TMap<uint32, YMeshSectionInfo>::TConstIterator It(Other.Map); It; ++It)
	{
		Map.Add(It.Key(), It.Value());
	}
}

bool YMeshSectionInfoMap::AnySectionHasCollision() const
{
	for (TMap<uint32, YMeshSectionInfo>::TConstIterator It(Map); It; ++It)
	{
		uint32 Key = It.Key();
		int32 LODIndex = (int32)(Key >> 16);
		if (LODIndex == 0 && It.Value().bEnableCollision)
		{
			return true;
		}
	}
	return false;
}
FArchive& operator<<(FArchive& Ar, YMeshSectionInfo& Info)
{
	Ar << Info.MaterialIndex;
	Ar << Info.bEnableCollision;
	Ar << Info.bCastShadow;
	return Ar;
}
void YMeshSectionInfoMap::Serialize(FArchive& Ar)
{
	Ar << Map;
}

YStaticMeshSourceModel::YStaticMeshSourceModel()
{
	RawMeshBulkData = new YRawMeshBulkData();
	ScreenSize = 0.0f;
}

YStaticMeshSourceModel::~YStaticMeshSourceModel()
{
	if (RawMeshBulkData)
	{
		delete RawMeshBulkData;
		RawMeshBulkData = NULL;
	}
}

void YStaticMeshSourceModel::SerializeBulkData(FArchive& Ar, UObject* Owner)
{
	RawMeshBulkData->Serialize(Ar);
}
