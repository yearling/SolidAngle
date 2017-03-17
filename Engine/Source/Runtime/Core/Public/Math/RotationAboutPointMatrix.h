// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/RotationTranslationMatrix.h"

/** Rotates about an Origin point. */
class YRotationAboutPointMatrix
	: public YRotationTranslationMatrix
{
public:

	/**
	 * Constructor.
	 *
	 * @param Rot rotation
	 * @param Origin about which to rotate.
	 */
	YRotationAboutPointMatrix(const YRotator& Rot, const YVector& Origin);

	/** Matrix factory. Return an YMatrix so we don't have type conversion issues in expressions. */
	static YMatrix Make(const YRotator& Rot, const YVector& Origin)
	{
		return YRotationAboutPointMatrix(Rot, Origin);
	}

	/** Matrix factory. Return an YMatrix so we don't have type conversion issues in expressions. */
	static CORE_API YMatrix Make(const YQuat& Rot, const YVector& Origin);
};


FORCEINLINE YRotationAboutPointMatrix::YRotationAboutPointMatrix(const YRotator& Rot, const YVector& Origin)
	: YRotationTranslationMatrix(Rot, Origin)
{
	// FRotationTranslationMatrix generates R * T.
	// We need -T * R * T, so prepend that translation:
	YVector XAxis(M[0][0], M[1][0], M[2][0]);
	YVector YAxis(M[0][1], M[1][1], M[2][1]);
	YVector ZAxis(M[0][2], M[1][2], M[2][2]);

	M[3][0]	-= XAxis | Origin;
	M[3][1]	-= YAxis | Origin;
	M[3][2]	-= ZAxis | Origin;
}
