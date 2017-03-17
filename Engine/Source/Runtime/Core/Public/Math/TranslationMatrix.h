// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"

class YTranslationMatrix
	: public YMatrix
{
public:

	/** Constructor translation matrix based on given vector */
	YTranslationMatrix(const YVector& Delta);

	/** Matrix factory. Return an FMatrix so we don't have type conversion issues in expressions. */
	static YMatrix Make(YVector const& Delta)
	{
		return YTranslationMatrix(Delta);
	}
};


FORCEINLINE YTranslationMatrix::YTranslationMatrix(const YVector& Delta)
	: YMatrix(
		YPlane(1.0f,	0.0f,	0.0f,	0.0f),
		YPlane(0.0f,	1.0f,	0.0f,	0.0f),
		YPlane(0.0f,	0.0f,	1.0f,	0.0f),
		YPlane(Delta.X,	Delta.Y,Delta.Z,1.0f)
	)
{ }
