#pragma once
#include "YYUT.h"

struct FRenderSceneInfo
{
	FVector MainLightDir;
};

struct FRenderCameraInfo
{
	FMatrix View;
	FMatrix ViewInv;
	FMatrix Projection;
	FMatrix ViewProjection;
	FMatrix ViewProjectionInv;
};

struct FRenderInfo
{
	FRenderCameraInfo RenderCameraInfo;
	FRenderSceneInfo  SceneInfo;
};