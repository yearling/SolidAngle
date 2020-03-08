#include "YStaticMeshRenderData.h"

YStaticMeshRenderData::YStaticMeshRenderData()
{

}

void YStaticMeshRenderData::AllocateLODResources(int32 NumLODs)
{
	check(LODResources.Num() == 0);
	while (LODResources.Num() < NumLODs)
	{
		new(LODResources) YStaticMeshLODResources();
	}
}
