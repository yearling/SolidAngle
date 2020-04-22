#include "YIntersection.h"
#include "Math/UnrealMathUtility.h"
bool RayCastTriangleTriangleMethodGeometry(const FVector &Origin, const FVector &RayDir, const FVector &Point0, const FVector &Point1, const FVector& Point2, float &t, float& u, float &v, float &w, bool bCullBackFace/*=true*/)
{
	FPlane TrianglePlane(Point0, Point1, Point2);
	if (FMath::Abs(RayDir | TrianglePlane) < SMALL_NUMBER) //parrallel
	{
		return false;
	}
	t = ((TrianglePlane.W - (Origin | TrianglePlane)) / (RayDir | TrianglePlane));
	// beind the ray origin
	if (t < 0)
		return false;

	FVector IntersectionPoint = Origin + t * RayDir;

	FVector BaryCentricCoordinate = FMath::ComputeBaryCentric2D(IntersectionPoint, Point0, Point1, Point2);
	if (BaryCentricCoordinate.X < 0 || BaryCentricCoordinate.X>1 ||
		BaryCentricCoordinate.Y < 0 || BaryCentricCoordinate.Y>1 ||
		BaryCentricCoordinate.Z < 0 || BaryCentricCoordinate.Z>1)
	{
		return false;
	}
	else
	{
		const FVector TriNorm = (Point1 - Point0) ^ (Point2 - Point0);
		//check collinearity of A,B,C
		check(TriNorm.SizeSquared() > SMALL_NUMBER && "Collinear points in FMath::ComputeBaryCentric2D()");
		const FVector N = TriNorm.GetSafeNormal();

		if (bCullBackFace && (N | RayDir) < 0)
		{
			return false;
		}
		u = BaryCentricCoordinate.X;
		v = BaryCentricCoordinate.Y;
		w = BaryCentricCoordinate.Z;
		return true;
	}
}

bool RayCastTriangle(const FVector &Origin, const FVector &RayDir, const FVector &Point0, const FVector &Point1, const FVector& Point2, float &t, float &u, float &v, float& w, bool bCullBackFace/*= true*/)
{
	FVector V01 = Point1 - Point0;
	FVector V02 = Point2 - Point0;
	FVector N = RayDir ^ V02;
	float det = V01 | N;
	if (bCullBackFace)
	{
		if (det > SMALL_NUMBER)
		{
			return false;
		}
	}
	else
	{
		if (FMath::Abs(det) < SMALL_NUMBER)
		{
			return false;
		}
	}
	float InvDet = 1.0 / det;
	FVector TVec = Origin - Point0;
	u = (TVec | N) * InvDet;
	if (u < 0 || u >1)
	{
		return false;
	}
	FVector QVec = TVec ^ V01;
	v = (RayDir | QVec)*InvDet;
	if (v < 0 || u + v>1)
	{
		return false;
	}
	t = (V02 | QVec)*InvDet;
	w = 1 - u - v;
	return true;
}

bool RayCastTriangleTriangleMethodMatrix(const FVector &Origin, const FVector &RayDir, const FVector &Point0, const FVector &Point1, const FVector& Point2, float &t, float &u, float &v, float& w, bool bCullBackFace /*= true*/)
{
	FVector V01 = Point1 - Point0;
	FVector V02 = Point2 - Point0;
	FMatrix RayMat(FPlane(FVector4(-RayDir, 0)), FPlane(FVector4(V01, 0)), FPlane(FVector4(V02, 0)), FPlane(FVector4(0, 0, 0, 1)));
	float det = RayMat.Determinant();
	if (bCullBackFace && det > SMALL_NUMBER)
	{
		return false;
	}
	else if (FMath::Abs(det) < SMALL_NUMBER)
	{
		return false;
	}
	FVector VOA = Origin - Point0;
	FMatrix InvRayMat = RayMat.Inverse();
	FVector4 Result = InvRayMat.TransformFVector4(FVector4(VOA, 0));
	t = Result.X;
	u = Result.Y;
	v = Result.Z;
	w = 1 - u - v;
	if (u < 0 || u >1)
	{
		return false;
	}
	if (v < 0 || u + v>1)
	{
		return false;
	}
	return true;
}

