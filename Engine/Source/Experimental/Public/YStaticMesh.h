#pragma once
#include "Core.h"



struct YMeshSectionInfo
{
	//Index in to the materials array on YStaticMesh
	int32 MaterialIndex;
	bool bEnableCollision;
	bool bCastShadow;
	YMeshSectionInfo()
		:MaterialIndex(0)
		, bEnableCollision(true)
		, bCastShadow(true)
	{}
	explicit YMeshSectionInfo(int32 InMaterialIndex)
		: MaterialIndex(InMaterialIndex)
		, bEnableCollision(true)
		, bCastShadow(true)
	{

	}
};
bool operator==(const YMeshSectionInfo& A, const YMeshSectionInfo& B);
bool operator!=(const YMeshSectionInfo& A, const YMeshSectionInfo& B);

// Map containing per-section settings for each section of each LOD
// key is combine of LOD and Section
// value is YMeshSectionInfo
struct YMeshSectionInfoMap
{
	// Maps an lod+section to the material it should render with
	TMap<uint32, YMeshSectionInfo> Map;
	void Serialize(FArchive& Ar);
	void Clear();
	int32 GetSectionNumber(int32 LODIndex) const;
	YMeshSectionInfo Get(int32 LODIndex, int32 SectionIndex) const;
	void Set(int32 LODIndex, int32 SectionIndex, const YMeshSectionInfo &Info);
	void Remove(int32 LODIndex, int32 SectionIndex);
	void CopyFrom(const YMeshSectionInfoMap& Other);
	bool AnySectionHasCollision() const;

};

// Source model from which a renderable static mesh is built.
struct YStaticMeshSourceModel
{
	/** Imported raw mesh data. Optional for all but the first LOD. */
	class YRawMeshBulkData* RawMeshBulkData;
	/**
	 * ScreenSize to display this LOD.
	 * The screen size is based around the projected diameter of the bounding
	 * sphere of the model. i.e. 0.5 means half the screen's maximum dimension.
	 */
	float ScreenSize;
	YStaticMeshSourceModel();
	~YStaticMeshSourceModel();
	void SerializeBulkData(FArchive& Ar, UObject* Owner);
};
class YStaticMesh:public FRefCountedObject
{
public:
	YStaticMesh();
	/** Imported raw mesh bulk data. */
	TArray<YStaticMeshSourceModel> SourceModels;
	/** Map of LOD+Section index to per-section info. */
	YMeshSectionInfoMap SectionInfoMap;
	FBoxSphereBounds ExtendedBounds;
	/** Unique ID for tracking/caching this mesh during distributed lighting */
	FGuid LightingGuid;
	int32 LightMapResolution;
	int32 LightMapCoordinateIndex;
};
