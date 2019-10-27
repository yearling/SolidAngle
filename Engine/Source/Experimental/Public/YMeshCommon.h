#pragma once
#include "CoreMinimal.h"
enum { YMAX_TEXCOORDS =4, YMAX_STATIC_TEXCOORDS=8};
#define MAX_YTEXTURE_COORDS 8 
#define MAX_YSTATIC_MESH_LODS 8
/** The information used to build a static-mesh vertex. */
struct YStaticMeshBuildVertex
{
	FVector Position;

	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;

	FVector2D UVs[YMAX_STATIC_TEXCOORDS];
	FColor Color;
};
