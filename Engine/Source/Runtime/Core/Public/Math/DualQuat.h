// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Math/Transform.h"

/** Dual quaternion class */
class YDualQuat
{
public:

	/** rotation or real part */
	FQuat R;
	/** half trans or dual part */
	FQuat D;

	// Constructors
	YDualQuat(const FQuat &InR, const FQuat &InD)
		: R(InR)
		, D(InD)
	{}

	YDualQuat(const FTransform &T)
	{
		FVector V = T.GetTranslation()*0.5f;
		*this = YDualQuat(FQuat(0, 0, 0, 1), FQuat(V.X, V.Y, V.Z, 0.f)) * YDualQuat(T.GetRotation(), FQuat(0, 0, 0, 0));
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
	FTransform AsFTransform(FVector Scale = FVector(1.0f, 1.0f, 1.0f))
	{
		FQuat TQ = D*FQuat(-R.X, -R.Y, -R.Z, R.W);
		return FTransform(R, FVector(TQ.X, TQ.Y, TQ.Z)*2.0f, Scale);
	}
};
