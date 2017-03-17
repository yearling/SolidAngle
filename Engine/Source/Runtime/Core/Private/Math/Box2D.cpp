// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	Box2D.cpp: Implements the FBox2D class.
=============================================================================*/

/* FBox2D structors
 *****************************************************************************/

#include "Math/Box2D.h"

YBox2D::YBox2D(const YVector2D* Points, const int32 Count)
	: Min(0.f, 0.f)
	, Max(0.f, 0.f)
	, bIsValid(false)
{
	for (int32 PointItr = 0; PointItr < Count; PointItr++)
	{
		*this += Points[PointItr];
	}
}


YBox2D::YBox2D(const TArray<YVector2D>& Points)
	: Min(0.f, 0.f)
	, Max(0.f, 0.f)
	, bIsValid(false)
{
	for(const YVector2D& EachPoint : Points)
	{
		*this += EachPoint;
	}
}
