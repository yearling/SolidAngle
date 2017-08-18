// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Math/UnrealMathUtility.h"
#include "Containers/UnrealString.h"
#include "Math/Vector2D.h"

/**
 * Implements a rectangular 2D Box.
 */
struct FBox2D
{
public:
	/** Holds the box's minimum point. */
	YVector2D Min;

	/** Holds the box's maximum point. */
	YVector2D Max;

	/** Holds a flag indicating whether this box is valid. */
	bool bIsValid;

public:

	/** Default constructor (no initialization). */
	FBox2D() { }

	/**
	 * Creates and initializes a new box.
	 *
	 * The box extents are initialized to zero and the box is marked as invalid.
	 */
	FBox2D( int32 )
	{
		Init();
	}

	/**
	 * Creates and initializes a new box.
	 *
	 * The box extents are initialized to zero and the box is marked as invalid.
	 *
	 * @param EForceInit Force Init Enum.
	 */
	explicit FBox2D( EForceInit )
	{
		Init();
	}

	/**
	 * Creates and initializes a new box from the specified parameters.
	 *
	 * @param InMin The box's minimum point.
	 * @param InMax The box's maximum point.
	 */
	FBox2D( const YVector2D& InMin, const YVector2D& InMax )
		: Min(InMin)
		, Max(InMax)
		, bIsValid(true)
	{ }

	/**
	 * Creates and initializes a new box from the given set of points.
	 *
	 * @param Points Array of Points to create for the bounding volume.
	 * @param Count The number of points.
	 */
	CORE_API FBox2D( const YVector2D* Points, const int32 Count );

	/**
	 * Creates and initializes a new box from an array of points.
	 *
	 * @param Points Array of Points to create for the bounding volume.
	 */
	CORE_API FBox2D( const TArray<YVector2D>& Points );

public:

	/**
	 * Compares two boxes for equality.
	 *
	 * @param Other The other box to compare with.
	 * @return true if the boxes are equal, false otherwise.
	 */
	bool operator==( const FBox2D& Other ) const
	{
		return (Min == Other.Min) && (Max == Other.Max);
	}

	/**
	 * Adds to this bounding box to include a given point.
	 *
	 * @param Other The point to increase the bounding volume to.
	 * @return Reference to this bounding box after resizing to include the other point.
	 */
	FORCEINLINE FBox2D& operator+=( const YVector2D &Other );

	/**
	 * Gets the result of addition to this bounding volume.
	 *
	 * @param Other The other point to add to this.
	 * @return A new bounding volume.
	 */
	FBox2D operator+( const YVector2D& Other ) const
	{
		return FBox2D(*this) += Other;
	}

	/**
	 * Adds to this bounding box to include a new bounding volume.
	 *
	 * @param Other The bounding volume to increase the bounding volume to.
	 * @return Reference to this bounding volume after resizing to include the other bounding volume.
	 */
	FORCEINLINE FBox2D& operator+=( const FBox2D& Other );

	/**
	 * Gets the result of addition to this bounding volume.
	 *
	 * @param Other The other volume to add to this.
	 * @return A new bounding volume.
	 */
	FBox2D operator+( const FBox2D& Other ) const
	{
		return FBox2D(*this) += Other;
	}

	/**
	 * Gets reference to the min or max of this bounding volume.
	 *
	 * @param Index The index into points of the bounding volume.
	 * @return A reference to a point of the bounding volume.
	 */
   YVector2D& operator[]( int32 Index )
	{
		check((Index >= 0) && (Index < 2));

		if (Index == 0)
		{
			return Min;
		}

		return Max;
	}

public:

	/** 
	 * Calculates the distance of a point to this box.
	 *
	 * @param Point The point.
	 * @return The distance.
	 */
	FORCEINLINE float ComputeSquaredDistanceToPoint( const YVector2D& Point ) const
	{
		// Accumulates the distance as we iterate axis
		float DistSquared = 0.f;
		
		if (Point.X < Min.X)
		{
			DistSquared += YMath::Square(Point.X - Min.X);
		}
		else if (Point.X > Max.X)
		{
			DistSquared += YMath::Square(Point.X - Max.X);
		}
		
		if (Point.Y < Min.Y)
		{
			DistSquared += YMath::Square(Point.Y - Min.Y);
		}
		else if (Point.Y > Max.Y)
		{
			DistSquared += YMath::Square(Point.Y - Max.Y);
		}
		
		return DistSquared;
	}

	/** 
	 * Increase the bounding box volume.
	 *
	 * @param W The size to increase volume by.
	 * @return A new bounding box increased in size.
	 */
	FBox2D ExpandBy( const float W ) const
	{
		return FBox2D(Min - YVector2D(W, W), Max + YVector2D(W, W));
	}

	/**
	 * Gets the box area.
	 *
	 * @return Box area.
	 * @see GetCenter, GetCenterAndExtents, GetExtent, GetSize
	 */
	float GetArea() const
	{
		return (Max.X - Min.X) * (Max.Y - Min.Y);
	}

	/**
	 * Gets the box's center point.
	 *
	 * @return Th center point.
	 * @see GetArea, GetCenterAndExtents, GetExtent, GetSize
	 */
	YVector2D GetCenter() const
	{
		return YVector2D((Min + Max) * 0.5f);
	}

	/**
	 * Get the center and extents
	 *
	 * @param center[out] reference to center point
	 * @param Extents[out] reference to the extent around the center
	 * @see GetArea, GetCenter, GetExtent, GetSize
	 */
	void GetCenterAndExtents( YVector2D & center, YVector2D & Extents ) const
	{
		Extents = GetExtent();
		center = Min + Extents;
	}

	/**
	 * Calculates the closest point on or inside the box to a given point in space.
	 *
	 * @param Point The point in space.
	 *
	 * @return The closest point on or inside the box.
	 */
	FORCEINLINE YVector2D GetClosestPointTo( const YVector2D& Point ) const;

	/**
	 * Gets the box extents around the center.
	 *
	 * @return Box extents.
	 * @see GetArea, GetCenter, GetCenterAndExtents, GetSize
	 */
	YVector2D GetExtent() const
	{
		return 0.5f * (Max - Min);
	}


	/**
	 * Gets the box size.
	 *
	 * @return Box size.
	 * @see GetArea, GetCenter, GetCenterAndExtents, GetExtent
	 */
	YVector2D GetSize() const
	{
		return (Max - Min);
	}

	/**
	 * Set the initial values of the bounding box to Zero.
	 */
	void Init()
	{
		Min = Max = YVector2D::ZeroVector;
		bIsValid = false;
	}

	/**
	 * Checks whether the given box intersects this box.
	 *
	 * @param other bounding box to test intersection
	 * @return true if boxes intersect, false otherwise.
	 */
	FORCEINLINE bool Intersect( const FBox2D & other ) const;

	/**
	 * Checks whether the given point is inside this box.
	 *
	 * @param Point The point to test.
	 * @return true if the point is inside this box, otherwise false.
	 */
	bool IsInside( const YVector2D & TestPoint ) const
	{
		return ((TestPoint.X > Min.X) && (TestPoint.X < Max.X) && (TestPoint.Y > Min.Y) && (TestPoint.Y < Max.Y));
	}

	/** 
	 * Checks whether the given box is fully encapsulated by this box.
	 * 
	 * @param Other The box to test for encapsulation within the bounding volume.
	 * @return true if box is inside this volume, false otherwise.
	 */
	bool IsInside( const FBox2D& Other ) const
	{
		return (IsInside(Other.Min) && IsInside(Other.Max));
	}

	/** 
	 * Shift bounding box position.
	 *
	 * @param The offset vector to shift by.
	 * @return A new shifted bounding box.
	 */
	FBox2D ShiftBy( const YVector2D& Offset ) const
	{
		return FBox2D(Min + Offset, Max + Offset);
	}

	/**
	 * Get a textual representation of this box.
	 *
	 * @return A string describing the box.
	 */
	FString ToString() const;

public:

	/**
	 * Serializes the bounding box.
	 *
	 * @param Ar The archive to serialize into.
	 * @param Box The box to serialize.
	 *
	 * @return Reference to the Archive after serialization.
	 */
	friend FArchive& operator<<( FArchive& Ar, FBox2D& Box )
	{
		return Ar << Box.Min << Box.Max << Box.bIsValid;
	}
};


/* FBox2D inline functions
 *****************************************************************************/

FORCEINLINE FBox2D& FBox2D::operator+=( const YVector2D &Other )
{
	if (bIsValid)
	{
		Min.X = YMath::Min(Min.X, Other.X);
		Min.Y = YMath::Min(Min.Y, Other.Y);
	
		Max.X = YMath::Max(Max.X, Other.X);
		Max.Y = YMath::Max(Max.Y, Other.Y);
		
	}
	else
	{
		Min = Max = Other;
		bIsValid = true;
	}

	return *this;
}


FORCEINLINE FBox2D& FBox2D::operator+=( const FBox2D& Other )
{
	if (bIsValid && Other.bIsValid)
	{
		Min.X = YMath::Min(Min.X, Other.Min.X);
		Min.Y = YMath::Min(Min.Y, Other.Min.Y);

		Max.X = YMath::Max(Max.X, Other.Max.X);
		Max.Y = YMath::Max(Max.Y, Other.Max.Y);
	}
	else if (Other.bIsValid)
	{
		*this = Other;
	}

	return *this;
}


FORCEINLINE YVector2D FBox2D::GetClosestPointTo( const YVector2D& Point ) const
{
	// start by considering the point inside the box
	YVector2D ClosestPoint = Point;

	// now clamp to inside box if it's outside
	if (Point.X < Min.X)
	{
		ClosestPoint.X = Min.X;
	}
	else if (Point.X > Max.X)
	{
		ClosestPoint.X = Max.X;
	}

	// now clamp to inside box if it's outside
	if (Point.Y < Min.Y)
	{
		ClosestPoint.Y = Min.Y;
	}
	else if (Point.Y > Max.Y)
	{
		ClosestPoint.Y = Max.Y;
	}

	return ClosestPoint;
}


FORCEINLINE bool FBox2D::Intersect( const FBox2D & Other ) const
{
	if ((Min.X > Other.Max.X) || (Other.Min.X > Max.X))
	{
		return false;
	}

	if ((Min.Y > Other.Max.Y) || (Other.Min.Y > Max.Y))
	{
		return false;
	}

	return true;
}


FORCEINLINE FString FBox2D::ToString() const
{
	return FString::Printf(TEXT("bIsValid=%s, Min=(%s), Max=(%s)"), bIsValid ? TEXT("true") : TEXT("false"), *Min.ToString(), *Max.ToString());
}
