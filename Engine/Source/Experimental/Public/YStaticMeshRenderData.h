#pragma once
#include "core.h"


struct YStaticMeshLODResources
{

};

class YStaticMeshRenderData
{
public:
	YStaticMeshRenderData();
	TIndirectArray<YStaticMeshLODResources> LODResources;
};
