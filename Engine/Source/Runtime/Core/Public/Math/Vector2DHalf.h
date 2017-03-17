// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"
#include "Math/Vector2D.h"
#include "Math/Float16.h"

/**
 * Structure for two dimensional vectors with half floating point precision.
 */
struct YVector2DHalf 
{
	/** Holds the vector's X-component. */
	YFloat16 X;

	/** Holds the vector's Y-component. */
	YFloat16 Y;

public:

	/** Default Constructor (no initialization). */
	FORCEINLINE YVector2DHalf() { }

	/**
	 * Constructor.
	 *
	 * InX half float X value
	 * Iny half float Y value
	 */
 	FORCEINLINE YVector2DHalf( const YFloat16& InX,const YFloat16& InY );

	/** Constructor 
	 *
	 * InX float X value
	 * Iny float Y value
	 */
	FORCEINLINE YVector2DHalf( float InX,float InY );

	/** Constructor 
	 *
	 * Vector2D float vector
	 */
	FORCEINLINE YVector2DHalf( const YVector2D& Vector2D );

public:

	/**
	 * Assignment operator.
	 *
	 * @param Vector2D The value to assign.
	 */
 	YVector2DHalf& operator=( const YVector2D& Vector2D );

	/** Implicit conversion operator for conversion to YVector2D. */
	operator YVector2D() const;

public:

	/**
	 * Get a textual representation of the vector.
	 *
	 * @return Text describing the vector.
	 */
	YString ToString() const;

public:

	/**
	 * Serializes the FVector2DHalf.
	 *
	 * @param Ar Reference to the serialization archive.
	 * @param V Reference to the FVector2DHalf being serialized.
	 * @return Reference to the Archive after serialization.
	 */
	friend YArchive& operator<<( YArchive& Ar, YVector2DHalf& V )
	{
		return Ar << V.X << V.Y;
	}
};


/* FVector2DHalf inline functions
 *****************************************************************************/

FORCEINLINE YVector2DHalf::YVector2DHalf( const YFloat16& InX, const YFloat16& InY )
 	:	X(InX), Y(InY)
{ }


FORCEINLINE YVector2DHalf::YVector2DHalf( float InX, float InY )
	:	X(InX), Y(InY)
{ }


FORCEINLINE YVector2DHalf::YVector2DHalf( const YVector2D& Vector2D )
	:	X(Vector2D.X), Y(Vector2D.Y)
{ }


FORCEINLINE YVector2DHalf& YVector2DHalf::operator=( const YVector2D& Vector2D )
{
 	X = YFloat16(Vector2D.X);
 	Y = YFloat16(Vector2D.Y);

	return *this;
}


FORCEINLINE YString YVector2DHalf::ToString() const
{
	return YString::Printf(TEXT("X=%3.3f Y=%3.3f"), (float)X, (float)Y );
}


FORCEINLINE YVector2DHalf::operator YVector2D() const
{
	return YVector2D((float)X,(float)Y);
}
