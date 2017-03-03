#pragma once
//!!FIXME by zyx
//#include "Containers/UnrealString.h"
#include "HAL/Platform.h"


/**
* Structure for integer points in 2-d space.
*
* @todo Docs: The operators need better documentation, i.e. what does it mean to divide a point?
*/
struct YIntPoint
{
	/** Holds the point's x-coordinate. */
	int32 X;

	/** Holds the point's y-coordinate. */
	int32 Y;

public:

	/** An integer point with zeroed values. */
	CORE_API static const YIntPoint ZeroValue;

	/** An integer point with INDEX_NONE values. */
	CORE_API static const YIntPoint NoneValue;

public:

	/** Default constructor (no initialization). */
	YIntPoint();

	/**
	* Create and initialize a new instance with the specified coordinates.
	*
	* @param InX The x-coordinate.
	* @param InY The y-coordinate.
	*/
	YIntPoint(int32 InX, int32 InY);

	/**
	* Create and initialize a new instance to zero.
	*
	* @param EForceInit Force init enum
	*/
	FORCEINLINE explicit  YIntPoint(EForceInit);

public:

	/**
	* Get specific component of a point.
	*
	* @param PointIndex Index of point component.
	* @return const reference to component.
	*/
	const int32& operator()(int32 PointIndex) const;

	/**
	* Get specific component of a point.
	*
	* @param PointIndex Index of point component
	* @return reference to component.
	*/
	int32& operator()(int32 PointIndex);

	/**
	* Compare two points for equality.
	*
	* @param Other The other int point being compared.
	* @return true if the points are equal, false otherwise.
	*/
	bool operator==(const YIntPoint& Other) const;

	/**
	* Compare two points for inequality.
	*
	* @param Other The other int point being compared.
	* @return true if the points are not equal, false otherwise.
	*/
	bool operator!=(const YIntPoint& Other) const;

	/**
	* Scale this point.
	*
	* @param Scale What to multiply the point by.
	* @return Reference to this point after multiplication.
	*/
	YIntPoint& operator*=(int32 Scale);

	/**
	* Divide this point by a scalar.
	*
	* @param Divisor What to divide the point by.
	* @return Reference to this point after division.
	*/
	YIntPoint& operator/=(int32 Divisor);

	/**
	* Add another point component-wise to this point.
	*
	* @param Other The point to add to this point.
	* @return Reference to this point after addition.
	*/
	YIntPoint& operator+=(const YIntPoint& Other);

	/**
	* Subtract another point component-wise from this point.
	*
	* @param Other The point to subtract from this point.
	* @return Reference to this point after subtraction.
	*/
	YIntPoint& operator-=(const YIntPoint& Other);

	/**
	* Divide this point component-wise by another point.
	*
	* @param Other The point to divide with.
	* @return Reference to this point after division.
	*/
	YIntPoint& operator/=(const YIntPoint& Other);

	/**
	* Assign another point to this one.
	*
	* @param Other The point to assign this point from.
	* @return Reference to this point after assignment.
	*/
	YIntPoint& operator=(const YIntPoint& Other);

	/**
	* Get the result of scaling on this point.
	*
	* @param Scale What to multiply the point by.
	* @return A new scaled int point.
	*/
	YIntPoint operator*(int32 Scale) const;

	/**
	* Get the result of division on this point.
	*
	* @param Divisor What to divide the point by.
	* @return A new divided int point.
	*/
	YIntPoint operator/(int32 Divisor) const;

	/**
	* Get the result of addition on this point.
	*
	* @param Other The other point to add to this.
	* @return A new combined int point.
	*/
	YIntPoint operator+(const YIntPoint& Other) const;

	/**
	* Get the result of subtraction from this point.
	*
	* @param Other The other point to subtract from this.
	* @return A new subtracted int point.
	*/
	YIntPoint operator-(const YIntPoint& Other) const;

	/**
	* Get the result of division on this point.
	*
	* @param Other The other point to subtract from this.
	* @return A new subtracted int point.
	*/
	YIntPoint operator/(const YIntPoint& Other) const;

	/**
	* Get specific component of the point.
	*
	* @param Index the index of point component
	* @return reference to component.
	*/
	int32& operator[](int32 Index);

	/**
	* Get specific component of the point.
	*
	* @param Index the index of point component
	* @return copy of component value.
	*/
	int32 operator[](int32 Index) const;

public:

	/**
	* Get the component-wise min of two points.
	*
	* @see ComponentMax, GetMax
	*/
	FORCEINLINE YIntPoint		ComponentMin(const YIntPoint& Other) const;

	/**
	* Get the component-wise max of two points.
	*
	* @see ComponentMin, GetMin
	*/
	FORCEINLINE YIntPoint		ComponentMax(const YIntPoint& Other) const;

	/**
	* Get the larger of the point's two components.
	*
	* @return The maximum component of the point.
	* @see GetMin, Size, SizeSquared
	*/
	int32						GetMax() const;

	/**
	* Get the smaller of the point's two components.
	*
	* @return The minimum component of the point.
	* @see GetMax, Size, SizeSquared
	*/
	int32						GetMin() const;

	/**
	* Get the distance of this point from (0,0).
	*
	* @return The distance of this point from (0,0).
	* @see GetMax, GetMin, SizeSquared
	*/
	int32						Size() const;

	/**
	* Get the squared distance of this point from (0,0).
	*
	* @return The squared distance of this point from (0,0).
	* @see GetMax, GetMin, Size
	*/
	int32						SizeSquared() const;

	/**
	* Get a textual representation of this point.
	*
	* @return A string describing the point.
	*/
	//!!FIXME by zyx
	//YString ToString() const;

public:

	/**
	* Divide an int point and round up the result.
	*
	* @param lhs The int point being divided.
	* @param Divisor What to divide the int point by.
	* @return A new divided int point.
	* @see DivideAndRoundDown
	*/
	static YIntPoint DivideAndRoundUp(YIntPoint lhs, int32 Divisor);
	static YIntPoint DivideAndRoundUp(YIntPoint lhs, YIntPoint Divisor);

	/**
	* Divide an int point and round down the result.
	*
	* @param lhs The int point being divided.
	* @param Divisor What to divide the int point by.
	* @return A new divided int point.
	* @see DivideAndRoundUp
	*/
	static YIntPoint DivideAndRoundDown(YIntPoint lhs, int32 Divisor);

	/**
	* Get number of components point has.
	*
	* @return number of components point has.
	*/
	static int32 Num();

public:

	/**
	* Serialize the point.
	*
	* @param Ar The archive to serialize into.
	* @param Point The point to serialize.
	* @return Reference to the Archive after serialization.
	*/
	//!!FIXME by zyx
	//friend FArchive& operator<<(FArchive& Ar, YIntPoint& Point)
	//{
	//	return Ar << Point.X << Point.Y;
	//}

	/**
	* Serialize the point.
	*
	* @param Ar The archive to serialize into.
	* @return true on success, false otherwise.
	*/
	//!!FIXME by zyx
	//bool Serialize(FArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}
};


/* YIntPoint inline functions
*****************************************************************************/

FORCEINLINE YIntPoint::YIntPoint() { }


FORCEINLINE YIntPoint::YIntPoint(int32 InX, int32 InY)
	: X(InX)
	, Y(InY)
{ }


FORCEINLINE YIntPoint::YIntPoint(EForceInit)
	: X(0)
	, Y(0)
{ }


FORCEINLINE const int32& YIntPoint::operator()(int32 PointIndex) const
{
	return (&X)[PointIndex];
}


FORCEINLINE int32& YIntPoint::operator()(int32 PointIndex)
{
	return (&X)[PointIndex];
}


FORCEINLINE int32 YIntPoint::Num()
{
	return 2;
}


FORCEINLINE bool YIntPoint::operator==(const YIntPoint& Other) const
{
	return X == Other.X && Y == Other.Y;
}


FORCEINLINE bool YIntPoint::operator!=(const YIntPoint& Other) const
{
	return (X != Other.X) || (Y != Other.Y);
}


FORCEINLINE YIntPoint& YIntPoint::operator*=(int32 Scale)
{
	X *= Scale;
	Y *= Scale;

	return *this;
}


FORCEINLINE YIntPoint& YIntPoint::operator/=(int32 Divisor)
{
	X /= Divisor;
	Y /= Divisor;

	return *this;
}


FORCEINLINE YIntPoint& YIntPoint::operator+=(const YIntPoint& Other)
{
	X += Other.X;
	Y += Other.Y;

	return *this;
}


FORCEINLINE YIntPoint& YIntPoint::operator-=(const YIntPoint& Other)
{
	X -= Other.X;
	Y -= Other.Y;

	return *this;
}


FORCEINLINE YIntPoint& YIntPoint::operator/=(const YIntPoint& Other)
{
	X /= Other.X;
	Y /= Other.Y;

	return *this;
}


FORCEINLINE YIntPoint& YIntPoint::operator=(const YIntPoint& Other)
{
	X = Other.X;
	Y = Other.Y;

	return *this;
}


FORCEINLINE YIntPoint YIntPoint::operator*(int32 Scale) const
{
	return YIntPoint(*this) *= Scale;
}


FORCEINLINE YIntPoint YIntPoint::operator/(int32 Divisor) const
{
	return YIntPoint(*this) /= Divisor;
}


FORCEINLINE int32& YIntPoint::operator[](int32 Index)
{
	//!!FIXME by zyx
	//check(Index >= 0 && Index < 2);
	return ((Index == 0) ? X : Y);
}


FORCEINLINE int32 YIntPoint::operator[](int32 Index) const
{
	//!!FIXME by zyx
	//check(Index >= 0 && Index < 2);
	return ((Index == 0) ? X : Y);
}


FORCEINLINE YIntPoint YIntPoint::ComponentMin(const YIntPoint& Other) const
{
	return YIntPoint(YMath::Min(X, Other.X), YMath::Min(Y, Other.Y));
}


FORCEINLINE YIntPoint YIntPoint::ComponentMax(const YIntPoint& Other) const
{
	return YIntPoint(YMath::Max(X, Other.X), YMath::Max(Y, Other.Y));
}

FORCEINLINE YIntPoint YIntPoint::DivideAndRoundUp(YIntPoint lhs, int32 Divisor)
{
	return YIntPoint(YMath::DivideAndRoundUp(lhs.X, Divisor), YMath::DivideAndRoundUp(lhs.Y, Divisor));
}

FORCEINLINE YIntPoint YIntPoint::DivideAndRoundUp(YIntPoint lhs, YIntPoint Divisor)
{
	return YIntPoint(YMath::DivideAndRoundUp(lhs.X, Divisor.X), YMath::DivideAndRoundUp(lhs.Y, Divisor.Y));
}

FORCEINLINE YIntPoint YIntPoint::DivideAndRoundDown(YIntPoint lhs, int32 Divisor)
{
	return YIntPoint(YMath::DivideAndRoundDown(lhs.X, Divisor), YMath::DivideAndRoundDown(lhs.Y, Divisor));
}


FORCEINLINE YIntPoint YIntPoint::operator+(const YIntPoint& Other) const
{
	return YIntPoint(*this) += Other;
}


FORCEINLINE YIntPoint YIntPoint::operator-(const YIntPoint& Other) const
{
	return YIntPoint(*this) -= Other;
}


FORCEINLINE YIntPoint YIntPoint::operator/(const YIntPoint& Other) const
{
	return YIntPoint(*this) /= Other;
}


FORCEINLINE int32 YIntPoint::GetMax() const
{
	return YMath::Max(X, Y);
}


FORCEINLINE int32 YIntPoint::GetMin() const
{
	return YMath::Min(X, Y);
}

//!!FIXME by zyx
//FORCEINLINE uint32 GetTypeHash(const YIntPoint& InPoint)
//{
//	return HashCombine(GetTypeHash(InPoint.X), GetTypeHash(InPoint.Y));
//}


FORCEINLINE int32 YIntPoint::Size() const
{
	int64 X64 = (int64)X;
	int64 Y64 = (int64)Y;
	return int32(YMath::Sqrt(float(X64 * X64 + Y64 * Y64)));
}

FORCEINLINE int32 YIntPoint::SizeSquared() const
{
	return X*X + Y*Y;
}


//!!FIXME by zyx
//FORCEINLINE FString YIntPoint::ToString() const
//{
//	return FString::Printf(TEXT("X=%d Y=%d"), X, Y);
//}
//
//!!FIXME by zyx
//template <> struct TIsPODType<YIntPoint> { enum { Value = true }; };
