// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Matrix.h"

/** Combined Scale rotation and translation matrix */
class FScaleRotationTranslationMatrix
	: public FMatrix
{
public:

	/**
	 * Constructor.
	 *
	 * @param Scale scale to apply to matrix
	 * @param Rot rotation
	 * @param Origin translation to apply
	 */
	FScaleRotationTranslationMatrix(const FVector& Scale, const FRotator& Rot, const FVector& Origin);
};


FORCEINLINE FScaleRotationTranslationMatrix::FScaleRotationTranslationMatrix(const FVector& Scale, const FRotator& Rot, const FVector& Origin)
{

#if PLATFORM_ENABLE_VECTORINTRINSICS

	const VectorRegister Angles = MakeVectorRegister(Rot.Pitch, Rot.Yaw, Rot.Roll, 0.0f);
	const VectorRegister HalfAngles = VectorMultiply(Angles, GlobalVectorConstants::DEG_TO_RAD);

	union { VectorRegister v; float f[4]; } SinAngles, CosAngles;
	VectorSinCos(&SinAngles.v, &CosAngles.v, &HalfAngles);

	const float	SP = SinAngles.f[0];
	const float	SY = SinAngles.f[1];
	const float	SR = SinAngles.f[2];
	const float	CP = CosAngles.f[0];
	const float	CY = CosAngles.f[1];
	const float	CR = CosAngles.f[2];

#else

	float SP, SY, SR;
	float CP, CY, CR;
	FMath::SinCos(&SP, &CP, FMath::DegreesToRadians(Rot.Pitch));
	FMath::SinCos(&SY, &CY, FMath::DegreesToRadians(Rot.Yaw));
	FMath::SinCos(&SR, &CR, FMath::DegreesToRadians(Rot.Roll));
#endif
	

	M[0][0]	= (CY * CR) * Scale.X;
	M[0][1]	= (CP * SY) * Scale.X;
	M[0][2]	= (-SP) * Scale.X;
	M[0][3]	= 0.f;

	M[1][0]	= (-CP*SR + SP*SY*CR) * Scale.Y;
	M[1][1]	= (CP*CR + SP*SY*SR) * Scale.Y;
	M[1][2]	= (SP*CY) * Scale.Y;
	M[1][3]	= 0.f;

	M[2][0]	= (SP*SR + CP*SY*CR) * Scale.Z;
	M[2][1]	= (-SP*CR + CP*SY*SR) * Scale.Z;
	M[2][2]	= (CP * CY) * Scale.Z;
	M[2][3]	= 0.f;

	M[3][0]	= Origin.X;
	M[3][1]	= Origin.Y;
	M[3][2]	= Origin.Z;
	M[3][3]	= 1.f;
}
