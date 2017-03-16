#pragma once

#include "Vector.h"

/**
* Structure for three dimensional planes.
*
* Stores the coeffecients as Xx+Yy+Zz=W.
* Note that this is different from many other Plane classes that use Xx+Yy+Zz+W=0.
*/
MS_ALIGN(16) struct YPlane
	: public YVector
{
public:

	/** The w-component. */
	float W;

public:

	/** Default constructor (no initialization). */
	FORCEINLINE YPlane();

	/**
	* Copy Constructor.
	*
	* @param P Plane to copy from.
	*/
	FORCEINLINE YPlane(const YPlane& P);

	/**
	* Constructor.
	*
	* @param V 4D vector to set up plane.
	*/
	FORCEINLINE YPlane(const YVector4& V);

	/**
	* Constructor.
	*
	* @param InX X-coefficient.
	* @param InY Y-coefficient.
	* @param InZ Z-coefficient.
	* @param InW W-coefficient.
	*/
	FORCEINLINE YPlane(float InX, float InY, float InZ, float InW);

	/**
	* Constructor.
	*
	* @param InNormal Plane Normal Vector.
	* @param InW Plane W-coefficient.
	*/
	FORCEINLINE YPlane(YVector InNormal, float InW);

	/**
	* Constructor.
	*
	* @param InBase Base point in plane.
	* @param InNormal Plane Normal Vector.
	*/
	FORCEINLINE YPlane(YVector InBase, const YVector &InNormal);

	/**
	* Constructor.
	*
	* @param A First point in the plane.
	* @param B Second point in the plane.
	* @param C Third point in the plane.
	*/
	YPlane(YVector A, YVector B, YVector C);

	/**
	* Constructor
	*
	* @param EForceInit Force Init Enum.
	*/
	explicit FORCEINLINE YPlane(EForceInit);

	// Functions.

	/**
	* Calculates distance between plane and a point.
	*
	* @param P The other point.
	* @return >0: point is in front of the plane, <0: behind, =0: on the plane.
	*/
	FORCEINLINE float PlaneDot(const YVector &P) const;

	/**
	* Get a flipped version of the plane.
	*
	* @return A flipped version of the plane.
	*/
	YPlane Flip() const;

	/**
	* Get the result of transforming the plane by a Matrix.
	*
	* @param M The matrix to transform plane with.
	* @return The result of transform.
	*/
	YPlane TransformBy(const YMatrix& M) const;

	/**
	* You can optionally pass in the matrices transpose-adjoint, which save it recalculating it.
	* MSM: If we are going to save the transpose-adjoint we should also save the more expensive
	* determinant.
	*
	* @param M The Matrix to transform plane with.
	* @param DetM Determinant of Matrix.
	* @param TA Transpose-adjoint of Matrix.
	* @return The result of transform.
	*/
	YPlane TransformByUsingAdjointT(const YMatrix& M, float DetM, const YMatrix& TA) const;

	/**
	* Check if two planes are identical.
	*
	* @param V The other plane.
	* @return true if planes are identical, otherwise false.
	*/
	bool operator==(const YPlane& V) const;

	/**
	* Check if two planes are different.
	*
	* @param V The other plane.
	* @return true if planes are different, otherwise false.
	*/
	bool operator!=(const YPlane& V) const;

	/**
	* Checks whether two planes are equal within specified tolerance.
	*
	* @param V The other plane.
	* @param Tolerance Error Tolerance.
	* @return true if the two planes are equal within specified tolerance, otherwise false.
	*/
	bool Equals(const YPlane& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Calculates dot product of two planes.
	*
	* @param V The other plane.
	* @return The dot product.
	*/
	FORCEINLINE float operator|(const YPlane& V) const;

	/**
	* Gets result of adding a plane to this.
	*
	* @param V The other plane.
	* @return The result of adding a plane to this.
	*/
	YPlane operator+(const YPlane& V) const;

	/**
	* Gets result of subtracting a plane from this.
	*
	* @param V The other plane.
	* @return The result of subtracting a plane from this.
	*/
	YPlane operator-(const YPlane& V) const;

	/**
	* Gets result of dividing a plane.
	*
	* @param Scale What to divide by.
	* @return The result of division.
	*/
	YPlane operator/(float Scale) const;

	/**
	* Gets result of scaling a plane.
	*
	* @param Scale The scaling factor.
	* @return The result of scaling.
	*/
	YPlane operator*(float Scale) const;

	/**
	* Gets result of multiplying a plane with this.
	*
	* @param V The other plane.
	* @return The result of multiplying a plane with this.
	*/
	YPlane operator*(const YPlane& V);

	/**
	* Add another plane to this.
	*
	* @param V The other plane.
	* @return Copy of plane after addition.
	*/
	YPlane operator+=(const YPlane& V);

	/**
	* Subtract another plane from this.
	*
	* @param V The other plane.
	* @return Copy of plane after subtraction.
	*/
	YPlane operator-=(const YPlane& V);

	/**
	* Scale this plane.
	*
	* @param Scale The scaling factor.
	* @return Copy of plane after scaling.
	*/
	YPlane operator*=(float Scale);

	/**
	* Multiply another plane with this.
	*
	* @param V The other plane.
	* @return Copy of plane after multiplication.
	*/
	YPlane operator*=(const YPlane& V);

	/**
	* Divide this plane.
	*
	* @param V What to divide by.
	* @return Copy of plane after division.
	*/
	YPlane operator/=(float V);

	/**
	* Serializer.
	*
	* @param Ar Serialization Archive.
	* @param P Plane to serialize.
	* @return Reference to Archive after serialization.
	*/
	//!!FIXME by zyx
	//friend YArchive& operator<<(YArchive& Ar, YPlane &P)
	//{
	//	return Ar << (YVector&)P << P.W;
	//}

	//!!FIXME by zyx
	//bool Serialize(YArchive& Ar)
	//{
	//	if (Ar.UE4Ver() >= VER_UE4_ADDED_NATIVE_SERIALIZATION_FOR_IMMUTABLE_STRUCTURES)
	//	{
	//		Ar << *this;
	//		return true;
	//	}
	//	return false;
	//}

	/**
	* Serializes the vector compressed for e.g. network transmission.
	* @param Ar Archive to serialize to/ from.
	* @return false to allow the ordinary struct code to run (this never happens).
	*/

	//!!FIXME by zyx
//	bool NetSerialize(YArchive& Ar, class UPackageMap*, bool& bOutSuccess)
//	{
//		if (Ar.IsLoading())
//		{
//			int16 iX, iY, iZ, iW;
//			Ar << iX << iY << iZ << iW;
//			*this = YPlane(iX, iY, iZ, iW);
//		}
//		else
//		{
//			int16 iX(YLinearColor::RoundToInt(X));
//			int16 iY(YLinearColor::RoundToInt(Y));
//			int16 iZ(YLinearColor::RoundToInt(Z));
//			int16 iW(YLinearColor::RoundToInt(W));
//			Ar << iX << iY << iZ << iW;
//		}
//		bOutSuccess = true;
//		return true;
//	}
} GCC_ALIGN(16);
/* YLinearColor inline functions
*****************************************************************************/

inline YVector YMath::LinePlaneIntersection
(
	const YVector &Point1,
	const YVector &Point2,
	const YPlane  &Plane
)
{
	return
		Point1
		+ (Point2 - Point1)
		*	((Plane.W - (Point1 | Plane)) / ((Point2 - Point1) | Plane));
}

inline bool YMath::IntersectPlanes3(YVector& I, const YPlane& P1, const YPlane& P2, const YPlane& P3)
{
	// Compute determinant, the triple product P1|(P2^P3)==(P1^P2)|P3.
	const float Det = (P1 ^ P2) | P3;
	if (Square(Det) < Square(0.001f))
	{
		// Degenerate.
		I = YVector::ZeroVector;
		return 0;
	}
	else
	{
		// Compute the intersection point, guaranteed valid if determinant is nonzero.
		I = (P1.W*(P2^P3) + P2.W*(P3^P1) + P3.W*(P1^P2)) / Det;
	}
	return 1;
}

inline bool YMath::IntersectPlanes2(YVector& I, YVector& D, const YPlane& P1, const YPlane& P2)
{
	// Compute line direction, perpendicular to both plane normals.
	D = P1 ^ P2;
	const float DD = D.SizeSquared();
	if (DD < Square(0.001f))
	{
		// Parallel or nearly parallel planes.
		D = I = YVector::ZeroVector;
		return 0;
	}
	else
	{
		// Compute intersection.
		I = (P1.W*(P2^D) + P2.W*(D^P1)) / DD;
		D.Normalize();
		return 1;
	}
}

/* YVector inline functions
*****************************************************************************/

inline YVector YVector::MirrorByPlane(const YPlane& Plane) const
{
	return *this - Plane * (2.f * Plane.PlaneDot(*this));
}

inline YVector YVector::PointPlaneProject(const YVector& Point, const YPlane& Plane)
{
	//Find the distance of X from the plane
	//Add the distance back along the normal from the point
	return Point - Plane.PlaneDot(Point) * Plane;
}

inline YVector YVector::PointPlaneProject(const YVector& Point, const YVector& A, const YVector& B, const YVector& C)
{
	//Compute the plane normal from ABC
	YPlane Plane(A, B, C);

	//Find the distance of X from the plane
	//Add the distance back along the normal from the point
	return Point - Plane.PlaneDot(Point) * Plane;
}

/* YPlane inline functions
*****************************************************************************/

FORCEINLINE YPlane::YPlane()
{}


FORCEINLINE YPlane::YPlane(const YPlane& P)
	: YVector(P)
	, W(P.W)
{}


FORCEINLINE YPlane::YPlane(const YVector4& V)
	: YVector(V)
	, W(V.W)
{}


FORCEINLINE YPlane::YPlane(float InX, float InY, float InZ, float InW)
	: YVector(InX, InY, InZ)
	, W(InW)
{}


FORCEINLINE YPlane::YPlane(YVector InNormal, float InW)
	: YVector(InNormal), W(InW)
{}


FORCEINLINE YPlane::YPlane(YVector InBase, const YVector &InNormal)
	: YVector(InNormal)
	, W(InBase | InNormal)
{}


FORCEINLINE YPlane::YPlane(YVector A, YVector B, YVector C)
	: YVector(((B - A) ^ (C - A)).GetSafeNormal())
{
	W = A | (YVector)(*this);
}


FORCEINLINE YPlane::YPlane(EForceInit)
	: YVector(ForceInit), W(0.f)
{}


FORCEINLINE float YPlane::PlaneDot(const YVector &P) const
{
	return X * P.X + Y * P.Y + Z * P.Z - W;
}


FORCEINLINE YPlane YPlane::Flip() const
{
	return YPlane(-X, -Y, -Z, -W);
}

FORCEINLINE bool YPlane::operator==(const YPlane& V) const
{
	return (X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W);
}


FORCEINLINE bool YPlane::operator!=(const YPlane& V) const
{
	return (X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W);
}


FORCEINLINE bool YPlane::Equals(const YPlane& V, float Tolerance) const
{
	return (YMath::Abs(X - V.X) < Tolerance) && (YMath::Abs(Y - V.Y) < Tolerance) && (YMath::Abs(Z - V.Z) < Tolerance) && (YMath::Abs(W - V.W) < Tolerance);
}


FORCEINLINE float YPlane::operator|(const YPlane& V) const
{
	return X * V.X + Y * V.Y + Z * V.Z + W * V.W;
}


FORCEINLINE YPlane YPlane::operator+(const YPlane& V) const
{
	return YPlane(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
}


FORCEINLINE YPlane YPlane::operator-(const YPlane& V) const
{
	return YPlane(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
}


FORCEINLINE YPlane YPlane::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;
	return YPlane(X * RScale, Y * RScale, Z * RScale, W * RScale);
}


FORCEINLINE YPlane YPlane::operator*(float Scale) const
{
	return YPlane(X * Scale, Y * Scale, Z * Scale, W * Scale);
}


FORCEINLINE YPlane YPlane::operator*(const YPlane& V)
{
	return YPlane(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
}


FORCEINLINE YPlane YPlane::operator+=(const YPlane& V)
{
	X += V.X; Y += V.Y; Z += V.Z; W += V.W;
	return *this;
}


FORCEINLINE YPlane YPlane::operator-=(const YPlane& V)
{
	X -= V.X; Y -= V.Y; Z -= V.Z; W -= V.W;
	return *this;
}


FORCEINLINE YPlane YPlane::operator*=(float Scale)
{
	X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
	return *this;
}


FORCEINLINE YPlane YPlane::operator*=(const YPlane& V)
{
	X *= V.X; Y *= V.Y; Z *= V.Z; W *= V.W;
	return *this;
}


FORCEINLINE YPlane YPlane::operator/=(float V)
{
	const float RV = 1.f / V;
	X *= RV; Y *= RV; Z *= RV; W *= RV;
	return *this;
}

//!!FIXME by zyx
//template <> struct TIsPODType<YPlane> { enum { Value = true }; };
