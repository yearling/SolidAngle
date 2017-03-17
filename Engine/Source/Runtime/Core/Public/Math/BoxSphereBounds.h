// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/SolidAngleMathUtility.h"
#include "Containers/SolidAngleString.h"
#include "Logging/LogMacros.h"
#include "Math/Vector.h"
#include "Math/Sphere.h"
#include "Math/Box.h"

/**
 * Structure for a combined axis aligned bounding box and bounding sphere with the same origin. (28 bytes).
 */
struct YBoxSphereBounds
{
	/** Holds the origin of the bounding box and sphere. */
	YVector	Origin;

	/** Holds the extent of the bounding box. */
	YVector BoxExtent;

	/** Holds the radius of the bounding sphere. */
	float SphereRadius;

public:

	/** Default constructor. */
	YBoxSphereBounds() { }

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param EForceInit Force Init Enum.
	 */
	explicit FORCEINLINE YBoxSphereBounds( EForceInit ) 
		: Origin(ForceInit)
		, BoxExtent(ForceInit)
		, SphereRadius(0.f)
	{
		DiagnosticCheckNaN();
	}

	/**
	 * Creates and initializes a new instance from the specified parameters.
	 *
	 * @param InOrigin origin of the bounding box and sphere.
	 * @param InBoxExtent half size of box.
	 * @param InSphereRadius radius of the sphere.
	 */
	YBoxSphereBounds( const YVector& InOrigin, const YVector& InBoxExtent, float InSphereRadius )
		: Origin(InOrigin)
		, BoxExtent(InBoxExtent)
		, SphereRadius(InSphereRadius)
	{
		DiagnosticCheckNaN();
	}

	/**
	 * Creates and initializes a new instance from the given Box and Sphere.
	 *
	 * @param Box The bounding box.
	 * @param Sphere The bounding sphere.
	 */
	YBoxSphereBounds( const YBox& Box, const YSphere& Sphere )
	{
		Box.GetCenterAndExtents(Origin,BoxExtent);
		SphereRadius = YMath::Min(BoxExtent.Size(), (Sphere.Center - Origin).Size() + Sphere.W);

		DiagnosticCheckNaN();
	}
	
	/**
	 * Creates and initializes a new instance the given Box.
	 *
	 * The sphere radius is taken from the extent of the box.
	 *
	 * @param Box The bounding box.
	 */
	YBoxSphereBounds( const YBox& Box )
	{
		Box.GetCenterAndExtents(Origin,BoxExtent);
		SphereRadius = BoxExtent.Size();

		DiagnosticCheckNaN();
	}

	/**
	 * Creates and initializes a new instance for the given sphere.
	 */
	YBoxSphereBounds( const YSphere& Sphere )
	{
		Origin = Sphere.Center;
		BoxExtent = YVector(Sphere.W);
		SphereRadius = Sphere.W;

		DiagnosticCheckNaN();
	}

	/**
	 * Creates and initializes a new instance from the given set of points.
	 *
	 * The sphere radius is taken from the extent of the box.
	 *
	 * @param Points The points to be considered for the bounding box.
	 * @param NumPoints Number of points in the Points array.
	 */
	YBoxSphereBounds( const YVector* Points, uint32 NumPoints );


public:

	/**
	 * Constructs a bounding volume containing both this and B.
	 *
	 * @param Other The other bounding volume.
	 * @return The combined bounding volume.
	 */
	FORCEINLINE YBoxSphereBounds operator+( const YBoxSphereBounds& Other ) const;


public:

	/**
	 * Calculates the squared distance from a point to a bounding box
	 *
	 * @param Point The point.
	 * @return The distance.
	 */
	FORCEINLINE float ComputeSquaredDistanceFromBoxToPoint( const YVector& Point ) const
	{
		YVector Mins = Origin - BoxExtent;
		YVector Maxs = Origin + BoxExtent;

		return ::ComputeSquaredDistanceFromBoxToPoint(Mins, Maxs, Point);
	}

	/**
	 * Test whether the spheres from two BoxSphereBounds intersect/overlap.
	 * 
	 * @param  A First BoxSphereBounds to test.
	 * @param  B Second BoxSphereBounds to test.
	 * @param  Tolerance Error tolerance added to test distance.
	 * @return true if spheres intersect, false otherwise.
	 */
	FORCEINLINE static bool SpheresIntersect(const YBoxSphereBounds& A, const YBoxSphereBounds& B, float Tolerance = KINDA_SMALL_NUMBER)
	{
		return (A.Origin - B.Origin).SizeSquared() <= YMath::Square(YMath::Max(0.f, A.SphereRadius + B.SphereRadius + Tolerance));
	}

	/**
	 * Test whether the boxes from two BoxSphereBounds intersect/overlap.
	 * 
	 * @param  A First BoxSphereBounds to test.
	 * @param  B Second BoxSphereBounds to test.
	 * @return true if boxes intersect, false otherwise.
	 */
	FORCEINLINE static bool BoxesIntersect(const YBoxSphereBounds& A, const YBoxSphereBounds& B)
	{
		return A.GetBox().Intersect(B.GetBox());
	}

	/**
	 * Gets the bounding box.
	 *
	 * @return The bounding box.
	 */
	FORCEINLINE YBox GetBox() const
	{
		return YBox(Origin - BoxExtent,Origin + BoxExtent);
	}

	/**
	 * Gets the extrema for the bounding box.
	 *
	 * @param Extrema 1 for positive extrema from the origin, else negative
	 * @return The boxes extrema
	 */
	YVector GetBoxExtrema( uint32 Extrema ) const
	{
		if (Extrema)
		{
			return Origin + BoxExtent;
		}

		return Origin - BoxExtent;
	}

	/**
	 * Gets the bounding sphere.
	 *
	 * @return The bounding sphere.
	 */
	FORCEINLINE YSphere GetSphere() const
	{
		return YSphere(Origin,SphereRadius);
	}

	/**
	 * Increase the size of the box and sphere by a given size.
	 *
	 * @param ExpandAmount The size to increase by.
	 * @return A new box with the expanded size.
	 */
	FORCEINLINE YBoxSphereBounds ExpandBy( float ExpandAmount ) const
	{
		return YBoxSphereBounds(Origin, BoxExtent + ExpandAmount, SphereRadius + ExpandAmount);
	}

	/**
	 * Gets a bounding volume transformed by a matrix.
	 *
	 * @param M The matrix.
	 * @return The transformed volume.
	 */
	CORE_API YBoxSphereBounds TransformBy( const YMatrix& M ) const;

	/**
	 * Gets a bounding volume transformed by a FTransform object.
	 *
	 * @param M The FTransform object.
	 * @return The transformed volume.
	 */
	CORE_API YBoxSphereBounds TransformBy( const YTransform& M ) const;

	/**
	 * Get a textual representation of this bounding box.
	 *
	 * @return Text describing the bounding box.
	 */
	YString ToString() const;

	/**
	 * Constructs a bounding volume containing both A and B.
	 *
	 * This is a legacy version of the function used to compute primitive bounds, to avoid the need to rebuild lighting after the change.
	 */
	friend YBoxSphereBounds Union( const YBoxSphereBounds& A,const YBoxSphereBounds& B )
	{
		YBox BoundingBox(0);

		BoundingBox += (A.Origin - A.BoxExtent);
		BoundingBox += (A.Origin + A.BoxExtent);
		BoundingBox += (B.Origin - B.BoxExtent);
		BoundingBox += (B.Origin + B.BoxExtent);

		// Build a bounding sphere from the bounding box's origin and the radii of A and B.
		YBoxSphereBounds Result(BoundingBox);

		Result.SphereRadius = YMath::Min(Result.SphereRadius,YMath::Max((A.Origin - Result.Origin).Size() + A.SphereRadius,(B.Origin - Result.Origin).Size() + B.SphereRadius));
		Result.DiagnosticCheckNaN();

		return Result;
	}

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		if (Origin.ContainsNaN())
		{
			logOrEnsureNanError(TEXT("Origin contains NaN: %s"), *Origin.ToString());
			const_cast<YBoxSphereBounds*>(this)->Origin = YVector::ZeroVector;
		}
		if (BoxExtent.ContainsNaN())
		{
			logOrEnsureNanError(TEXT("BoxExtent contains NaN: %s"), *BoxExtent.ToString());
			const_cast<YBoxSphereBounds*>(this)->BoxExtent = YVector::ZeroVector;
		}
		if (YMath::IsNaN(SphereRadius) || !YMath::IsFinite(SphereRadius))
		{
			logOrEnsureNanError(TEXT("SphereRadius contains NaN: %f"), SphereRadius);
			const_cast<YBoxSphereBounds*>(this)->SphereRadius = 0.f;
		}
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const {}
#endif

	inline bool ContainsNaN() const
	{
		return Origin.ContainsNaN() || BoxExtent.ContainsNaN() || !YMath::IsFinite(SphereRadius);
	}

public:

	/**
	 * Serializes the given bounding volume from or into the specified archive.
	 *
	 * @param Ar The archive to serialize from or into.
	 * @param Bounds The bounding volume to serialize.
	 * @return The archive..
	 */
	friend YArchive& operator<<( YArchive& Ar, YBoxSphereBounds& Bounds )
	{
		return Ar << Bounds.Origin << Bounds.BoxExtent << Bounds.SphereRadius;
	}
};


/* FBoxSphereBounds inline functions
 *****************************************************************************/

FORCEINLINE YBoxSphereBounds::YBoxSphereBounds( const YVector* Points, uint32 NumPoints )
{
	YBox BoundingBox(0);

	// find an axis aligned bounding box for the points.
	for (uint32 PointIndex = 0; PointIndex < NumPoints; PointIndex++)
	{
		BoundingBox += Points[PointIndex];
	}

	BoundingBox.GetCenterAndExtents(Origin, BoxExtent);

	// using the center of the bounding box as the origin of the sphere, find the radius of the bounding sphere.
	SphereRadius = 0.0f;

	for (uint32 PointIndex = 0; PointIndex < NumPoints; PointIndex++)
	{
		SphereRadius = YMath::Max(SphereRadius,(Points[PointIndex] - Origin).Size());
	}

	DiagnosticCheckNaN();
}


FORCEINLINE YBoxSphereBounds YBoxSphereBounds::operator+( const YBoxSphereBounds& Other ) const
{
	YBox BoundingBox(0);

	BoundingBox += (this->Origin - this->BoxExtent);
	BoundingBox += (this->Origin + this->BoxExtent);
	BoundingBox += (Other.Origin - Other.BoxExtent);
	BoundingBox += (Other.Origin + Other.BoxExtent);

	// build a bounding sphere from the bounding box's origin and the radii of A and B.
	YBoxSphereBounds Result(BoundingBox);

	Result.SphereRadius = YMath::Min(Result.SphereRadius, YMath::Max((Origin - Result.Origin).Size() + SphereRadius, (Other.Origin - Result.Origin).Size() + Other.SphereRadius));
	Result.DiagnosticCheckNaN();

	return Result;
}


FORCEINLINE YString YBoxSphereBounds::ToString() const
{
	return YString::Printf(TEXT("Origin=%s, BoxExtent=(%s), SphereRadius=(%f)"), *Origin.ToString(), *BoxExtent.ToString(), SphereRadius);
}

template <> struct TIsPODType<YBoxSphereBounds> { enum { Value = true }; };
