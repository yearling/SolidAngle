// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once


/**
* A pair of 3D vectors.
*/
struct YTwoVectors
{
public:

	/** Holds the first vector. */
	YVector	v1;

	/** Holds the second vector. */
	YVector	v2;

public:

	/** Default constructor. */
	FORCEINLINE	YTwoVectors();

	/**
	* Creates and initializes a new instance with the specified vectors.
	*
	* @param In1 The first Vector.
	* @param In2 The second Vector.
	*/
	FORCEINLINE	YTwoVectors(YVector In1, YVector In2);

	/**
	* Constructor.
	*
	* @param EForceInit Force Init Enum
	*/
	explicit FORCEINLINE YTwoVectors(EForceInit);

public:

	/**
	* Gets result of addition of two pairs of vectors.
	*
	* @param V The pair to add.
	* @return Result of addition.
	*/
	FORCEINLINE YTwoVectors operator+(const YTwoVectors& V) const;

	/**
	* Gets result of subtraction of two pairs of vectors.
	*
	* @param V The pair to subtract.
	* @return Result of subtraction.
	*/
	FORCEINLINE YTwoVectors operator-(const YTwoVectors& V) const;

	/**
	* Gets result of scaling pair of vectors.
	*
	* @param Scale The scaling factor.
	* @return Result of Scaling.
	*/
	FORCEINLINE YTwoVectors operator*(float Scale) const;

	/**
	* Gets result of dividing pair of vectors.
	*
	* @param Scale What to divide by.
	* @return Result of division.
	*/
	YTwoVectors operator/(float Scale) const;

	/**
	* Gets result of multiplying two pairs of vectors.
	*
	* @param V The pair to multiply with.
	* @return Result of multiplication.
	*/
	FORCEINLINE YTwoVectors operator*(const YTwoVectors& V) const;

	/**
	* Gets result of division of two pairs of vectors.
	*
	* @param V The pair to divide by.
	* @return Result of division.
	*/
	FORCEINLINE YTwoVectors operator/(const YTwoVectors& V) const;

	// Binary comparison operators.

	/**
	* Checks two pairs for equality.
	*
	* @param V The other pair.
	* @return true if the two pairs are equal, false otherwise..
	*/
	bool operator==(const YTwoVectors& V) const;

	/**
	* Checks two pairs for inequality.
	*
	* @param V The other pair.
	* @return true if the two pairs are different, false otherwise..
	*/
	bool operator!=(const YTwoVectors& V) const;

	/**
	* Error-tolerant comparison.
	*
	* @param V The other pair.
	* @param Tolerance Error Tolerance.
	* @return true if two pairs are equal within specified tolerance, false otherwise..
	*/
	bool Equals(const YTwoVectors& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	// Unary operators.

	/**
	* Get a negated copy of the pair.
	*
	* @return A negated copy of the pair.
	*/
	FORCEINLINE YTwoVectors operator-() const;

	// Assignment operators.

	/**
	* Add a pair to this.
	*
	* @param The pair to add.
	* @return Copy of the pair after addition.
	*/
	FORCEINLINE YTwoVectors operator+=(const YTwoVectors& V);

	/**
	* Subtract a pair from this.
	*
	* @param The pair to subtract.
	* @return Copy of the pair after subtraction.
	*/
	FORCEINLINE YTwoVectors operator-=(const YTwoVectors& V);

	/**
	* Scale the pair.
	*
	* @param Scale What to scale by.
	* @return Copy of the pair after scaling.
	*/
	FORCEINLINE YTwoVectors operator*=(float Scale);

	/**
	* Divide the pair.
	*
	* @param What to divide by.
	* @return Copy of the pair after division.
	*/
	YTwoVectors operator/=(float V);

	/**
	* Multiply the pair by another.
	*
	* @param The other pair.
	* @return Copy of the pair after multiplication.
	*/
	YTwoVectors operator*=(const YTwoVectors& V);

	/**
	* Divide the pair by another.
	*
	* @param The other pair.
	* @return Copy of the pair after multiplication.
	*/
	YTwoVectors operator/=(const YTwoVectors& V);

	/**
	* Get a specific component from the pair.
	*
	* @param i The index of the component, even indices are for the first vector,
	*			odd ones are for the second. Returns index 5 if out of range.
	* @return Reference to the specified component.
	*/
	float& operator[](int32 i);

public:

	/**
	* Get the maximum value of all the vector coordinates.
	*
	* @return The maximum value of all the vector coordinates.
	*/
	float						GetMax() const;

	/**
	* Get the minimum value of all the vector coordinates.
	*
	* @return The minimum value of all the vector coordinates.
	*/
	float						GetMin() const;

	/**
	* Get a textual representation of this two-vector.
	*
	* @return A string describing the two-vector.
	*/
	//!!FIXME by zyx
	//YString ToString() const;

public:

	/**
	* Serializes the two-vector.
	*
	* @param Ar The archive to serialize into.
	* @param TwoVectors The two-vector to serialize.
	* @return Reference to the Archive after serialization.
	*/
	//!!FIXME by zyx
	//friend FArchive& operator<<(FArchive& Ar, YTwoVectors& TwoVectors)
	//{
	//	return Ar << TwoVectors.v1 << TwoVectors.v2;
	//}

	//!!FIXME by zyx
	//bool Serialize(FArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}
};


/* FTwoVectors inline functions
*****************************************************************************/

FORCEINLINE YTwoVectors operator*(float Scale, const YTwoVectors& V)
{
	return V.operator*(Scale);
}


FORCEINLINE	YTwoVectors::YTwoVectors() :
	v1(0.0f),
	v2(0.0f)
{ }


FORCEINLINE YTwoVectors::YTwoVectors(YVector In1, YVector In2)
	: v1(In1)
	, v2(In2)
{ }


FORCEINLINE YTwoVectors::YTwoVectors(EForceInit)
	: v1(ForceInit)
	, v2(ForceInit)
{ }


FORCEINLINE YTwoVectors YTwoVectors::operator+(const YTwoVectors& V) const
{
	return YTwoVectors(
		YVector(v1 + V.v1),
		YVector(v2 + V.v2)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator-(const YTwoVectors& V) const
{
	return YTwoVectors(
		YVector(v1 - V.v1),
		YVector(v2 - V.v2)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator*(float Scale) const
{
	return YTwoVectors(
		YVector(v1 * Scale),
		YVector(v2 * Scale)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;

	return YTwoVectors(
		YVector(v1 * RScale),
		YVector(v2 * RScale)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator*(const YTwoVectors& V) const
{
	return YTwoVectors(
		YVector(v1 * V.v1),
		YVector(v2 * V.v2)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator/(const YTwoVectors& V) const
{
	return YTwoVectors(
		YVector(v1 / V.v1),
		YVector(v2 / V.v2)
	);
}


FORCEINLINE bool YTwoVectors::operator==(const YTwoVectors& V) const
{
	return ((v1 == V.v1) && (v2 == V.v2));
}


FORCEINLINE bool YTwoVectors::operator!=(const YTwoVectors& V) const
{
	return ((v1 != V.v1) || (v2 != V.v2));
}


FORCEINLINE bool YTwoVectors::Equals(const YTwoVectors& V, float Tolerance) const
{
	return v1.Equals(V.v1, Tolerance) && v2.Equals(V.v2, Tolerance);
}


FORCEINLINE YTwoVectors YTwoVectors::operator-() const
{
	return YTwoVectors(
		YVector(-v1),
		YVector(-v2)
	);
}


FORCEINLINE YTwoVectors YTwoVectors::operator+=(const YTwoVectors& V)
{
	v1 += V.v1;
	v2 += V.v2;

	return *this;
}


FORCEINLINE YTwoVectors YTwoVectors::operator-=(const YTwoVectors& V)
{
	v1 -= V.v1;
	v2 -= V.v2;

	return *this;
}


FORCEINLINE YTwoVectors YTwoVectors::operator*=(float Scale)
{
	v1 *= Scale;
	v2 *= Scale;

	return *this;
}


FORCEINLINE YTwoVectors YTwoVectors::operator/=(float V)
{
	const float RV = 1.f / V;

	v1 *= RV;
	v2 *= RV;

	return *this;
}


FORCEINLINE YTwoVectors YTwoVectors::operator*=(const YTwoVectors& V)
{
	v1 *= V.v1;
	v2 *= V.v2;
	return *this;
}


FORCEINLINE YTwoVectors YTwoVectors::operator/=(const YTwoVectors& V)
{
	v1 /= V.v1;
	v2 /= V.v2;

	return *this;
}


FORCEINLINE float YTwoVectors::GetMax() const
{
	const float MaxMax = YMath::Max(YMath::Max(v1.X, v1.Y), v1.Z);
	const float MaxMin = YMath::Max(YMath::Max(v2.X, v2.Y), v2.Z);

	return YMath::Max(MaxMax, MaxMin);
}


FORCEINLINE float YTwoVectors::GetMin() const
{
	const float MinMax = YMath::Min(YMath::Min(v1.X, v1.Y), v1.Z);
	const float MinMin = YMath::Min(YMath::Min(v2.X, v2.Y), v2.Z);

	return YMath::Min(MinMax, MinMin);
}


FORCEINLINE float& YTwoVectors::operator[](int32 i)
{
	//!!FIXME by zyx
	//check(i > -1);
	//check(i < 6);

	switch (i)
	{
	case 0:		return v1.X;
	case 1:		return v2.X;
	case 2:		return v1.Y;
	case 3:		return v2.Y;
	case 4:		return v1.Z;
	default:	return v2.Z;
	}
}

//!!FIXME by zyx
//FORCEINLINE YString YTwoVectors::ToString() const
//{
//	return YString::Printf(TEXT("V1=(%s) V2=(%s)"), *v1.ToString(), *v2.ToString());
//}
//!!FIXME by zyx
//template <> struct TIsPODType<YTwoVectors> { enum { Value = true }; };