#pragma once
#include "Core.h"
bool RayCastTriangle(const FVector &Origin, const FVector &RayDir,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t,float& u,float &v, float &w, bool bCullBackFace=true);

bool RayCastTriangleTriangleMethodGeometry(const FVector &Origin, const FVector &RayDir,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float &u, float &v, float& w, bool bCullBackFace= true);

bool RayCastTriangleTriangleMethodMatrix(const FVector &Origin, const FVector &RayDir,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float &u, float &v, float& w, bool bCullBackFace = true);
