// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	Sphere.cpp: Implements the YSphere class.
=============================================================================*/

#include "Math/Sphere.h"
#include "Math/Box.h"
#include "Math/Transform.h"


/* YSphere structors
 *****************************************************************************/

YSphere::YSphere(const YVector* Pts, int32 Count)
	: Center(0, 0, 0)
	, W(0)
{
	if (Count)
	{
		const YBox Box(Pts, Count);

		*this = YSphere((Box.Min + Box.Max) / 2, 0);

		for (int32 i = 0; i < Count; i++)
		{
			const float Dist = YVector::DistSquared(Pts[i], Center);

			if (Dist > W)
			{
				W = Dist;
			}
		}

		W = YMath::Sqrt(W) * 1.001f;
	}
}


/* YSphere interface
 *****************************************************************************/

YSphere YSphere::TransformBy(const YMatrix& M) const
{
	YSphere	Result;

	Result.Center = M.TransformPosition(this->Center);

	const YVector XAxis(M.M[0][0], M.M[0][1], M.M[0][2]);
	const YVector YAxis(M.M[1][0], M.M[1][1], M.M[1][2]);
	const YVector ZAxis(M.M[2][0], M.M[2][1], M.M[2][2]);

	Result.W = YMath::Sqrt(YMath::Max(XAxis | XAxis, YMath::Max(YAxis | YAxis, ZAxis | ZAxis))) * W;

	return Result;
}


YSphere YSphere::TransformBy(const YTransform& M) const
{
	YSphere	Result;

	Result.Center = M.TransformPosition(this->Center);
	Result.W = M.GetMaximumAxisScale() * W;

	return Result;
}

float YSphere::GetVolume() const
{
	return (4.f / 3.f) * PI * (W * W * W);
}

YSphere& YSphere::operator+=(const YSphere &Other)
{
	if (W == 0.f)
	{
		*this = Other;
	}
	else if (IsInside(Other))
	{
		*this = Other;
	}
	else if (Other.IsInside(*this))
	{
		// no change		
	}
	else
	{
		YSphere NewSphere;

		YVector DirToOther = Other.Center - Center;
		YVector UnitDirToOther = DirToOther;
		UnitDirToOther.Normalize();

		float NewRadius = (DirToOther.Size() + Other.W + W) * 0.5f;

		// find end point
		YVector End1 = Other.Center + UnitDirToOther*Other.W;
		YVector End2 = Center - UnitDirToOther*W;
		YVector NewCenter = (End1 + End2)*0.5f;

		NewSphere.Center = NewCenter; 
		NewSphere.W = NewRadius;

		// make sure both are inside afterwards
		checkSlow (Other.IsInside(NewSphere, 1.f));
		checkSlow (IsInside(NewSphere, 1.f));

		*this = NewSphere;
	}

	return *this;
}
