#pragma once
#include "Core.h"
class YRay
{
public:
	YRay() {};
	YRay(const FVector& InOrigin, const FVector& InDirection) :Origin(InOrigin), Direction(InDirection) {}
	YRay(const YRay&) = default;
	YRay(YRay&&) = default;
	YRay& operator=(const YRay&) = default;
	YRay& operator=(YRay&&) = default;

	FVector Origin;
	FVector Direction;
};

struct YRayCastElement
{
	FVector TrianglePoints[3];
	FVector RaycastPoint;
	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;
	FVector2D UVs[2];
	FLinearColor Color;
	float t, u, v, w;
	bool bBackFace;
};
bool operator < (const YRayCastElement&lhs, const YRayCastElement& rhs);
bool RayCastTriangle(const YRay &Ray,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float& u, float &v, float &w, bool bCullBackFace = true);

bool RayCastTriangleReturnBackFaceResult(const YRay &Ray,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float& u, float &v, float &w, bool &bBackFace, bool bCullBackFace = true);

bool RayCastTriangleTriangleMethodGeometry(const YRay &Ray,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float &u, float &v, float& w, bool bCullBackFace = true);

bool RayCastTriangleTriangleMethodMatrix(const YRay &Ray,
	const FVector &Point0, const FVector &Point1, const FVector& Point2,
	float &t, float &u, float &v, float& w, bool bCullBackFace = true);


