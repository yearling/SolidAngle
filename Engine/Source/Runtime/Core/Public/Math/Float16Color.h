// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/Color.h"
#include "Math/Float16.h"

/**
 *	RGBA Color made up of FFloat16
 */
class YFloat16Color
{
public:

	YFloat16 R;
	YFloat16 G;
	YFloat16 B;
	YFloat16 A;

	/** Default constructor */
	YFloat16Color();

	/** Copy constructor. */
	YFloat16Color(const YFloat16Color& Src);

	/** Constructor from a linear color. */
	YFloat16Color(const YLinearColor& Src);

	/** assignment operator */
	YFloat16Color& operator=(const YFloat16Color& Src);

 	/**
	 * Checks whether two colors are identical.
	 *
	 * @param Src The other color.
	 * @return true if the two colors are identical, otherwise false.
	 */
	bool operator==(const YFloat16Color& Src);
};


FORCEINLINE YFloat16Color::YFloat16Color() { }


FORCEINLINE YFloat16Color::YFloat16Color(const YFloat16Color& Src)
{
	R = Src.R;
	G = Src.G;
	B = Src.B;
	A = Src.A;
}


FORCEINLINE YFloat16Color::YFloat16Color(const YLinearColor& Src) :
	R(Src.R),
	G(Src.G),
	B(Src.B),
	A(Src.A)
{ }


FORCEINLINE YFloat16Color& YFloat16Color::operator=(const YFloat16Color& Src)
{
	R = Src.R;
	G = Src.G;
	B = Src.B;
	A = Src.A;
	return *this;
}


FORCEINLINE bool YFloat16Color::operator==(const YFloat16Color& Src)
{
	return (
		(R == Src.R) &&
		(G == Src.G) &&
		(B == Src.B) &&
		(A == Src.A)
		);
}
