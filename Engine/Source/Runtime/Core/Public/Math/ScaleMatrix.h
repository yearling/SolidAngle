// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"

/**
 * Scale matrix.
 */
class YScaleMatrix
	: public FMatrix
{
public:

	/**
	 * @param Scale uniform scale to apply to matrix.
	 */
	YScaleMatrix( float Scale );

	/**
	 * @param Scale Non-uniform scale to apply to matrix.
	 */
	YScaleMatrix( const FVector& Scale );

	/** Matrix factory. Return an FMatrix so we don't have type conversion issues in expressions. */
	static FMatrix Make(float Scale)
	{
		return YScaleMatrix(Scale);
	}

	/** Matrix factory. Return an FMatrix so we don't have type conversion issues in expressions. */
	static FMatrix Make(const FVector& Scale)
	{
		return YScaleMatrix(Scale);
	}
};


/* FScaleMatrix inline functions
 *****************************************************************************/

FORCEINLINE YScaleMatrix::YScaleMatrix( float Scale )
	: FMatrix(
		FPlane(Scale,	0.0f,	0.0f,	0.0f),
		FPlane(0.0f,	Scale,	0.0f,	0.0f),
		FPlane(0.0f,	0.0f,	Scale,	0.0f),
		FPlane(0.0f,	0.0f,	0.0f,	1.0f)
	)
{ }


FORCEINLINE YScaleMatrix::YScaleMatrix( const FVector& Scale )
	: FMatrix(
		FPlane(Scale.X,	0.0f,		0.0f,		0.0f),
		FPlane(0.0f,	Scale.Y,	0.0f,		0.0f),
		FPlane(0.0f,	0.0f,		Scale.Z,	0.0f),
		FPlane(0.0f,	0.0f,		0.0f,		1.0f)
	)
{ }
