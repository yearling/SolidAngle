#pragma once
#include "Math\Vector.h"
#include "Math\Matrix.h"

struct YRenderSceneInfo
{
	FVector MainLightDir;
};

struct YRenderCameraInfo
{
	FMatrix View;
	FMatrix ViewInv;
	FMatrix Projection;
	FMatrix ViewProjection;
	FMatrix ViewProjectionInv;
};

struct YRenderInfo
{
	YRenderCameraInfo RenderCameraInfo;
	YRenderSceneInfo  SceneInfo;
	double			  CurrentTime;
	double			  TickTime;
	float			  FPS;
};