// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/SolidAngleMathUtility.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Math/Transform.h"

/** Dual quaternion class */
class YDualQuat
{
public:

	/** rotation or real part */
	YQuat R;
	/** half trans or dual part */
	YQuat D;

	// Constructors
	YDualQuat(const YQuat &InR, const YQuat &InD)
		: R(InR)
		, D(InD)
	{}

	YDualQuat(const YTransform &T)
	{
		YVector V = T.GetTranslation()*0.5f;
		*this = YDualQuat(YQuat(0, 0, 0, 1), YQuat(V.X, V.Y, V.Z, 0.f)) * YDualQuat(T.GetRotation(), YQuat(0, 0, 0, 0));
	}

	/** Dual quat addition */
	YDualQuat operator+(const YDualQuat &B) const
	{
		return{ R + B.R, D + B.D };
	}

	/** Dual quat product */
	YDualQuat operator*(const YDualQuat &B) const
	{
		return{ R*B.R, D*B.R + B.D*R };
	}

	/** Scale dual quat */
	YDualQuat operator*(const float S) const
	{
		return{ R*S, D*S };
	}

	/** Return normalized dual quat */
	YDualQuat Normalized() const
	{
		float MinV = 1.0f / YMath::Sqrt(R | R);
		return{ R*MinV, D*MinV };
	}

	/** Convert dual quat to transform */
	YTransform AsFTransform(YVector Scale = YVector(1.0f, 1.0f, 1.0f))
	{
		YQuat TQ = D*YQuat(-R.X, -R.Y, -R.Z, R.W);
		return YTransform(R, YVector(TQ.X, TQ.Y, TQ.Z)*2.0f, Scale);
	}
};
