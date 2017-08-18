// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"

class YPerspectiveMatrix
	: public FMatrix
{
public:

// Note: the value of this must match the mirror in Common.usf!
#define Z_PRECISION	0.0f

	/**
	 * Constructor
	 *
	 * @param HalfFOVX Half FOV in the X axis
	 * @param HalfFOVY Half FOV in the Y axis
	 * @param MultFOVX multiplier on the X axis
	 * @param MultFOVY multiplier on the y axis
	 * @param MinZ distance to the near Z plane
	 * @param MaxZ distance to the far Z plane
	 */
	YPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);

	/**
	 * Constructor
	 *
	 * @param HalfFOV half Field of View in the Y direction
	 * @param Width view space width
	 * @param Height view space height
	 * @param MinZ distance to the near Z plane
	 * @param MaxZ distance to the far Z plane
	 * @note that the FOV you pass in is actually half the FOV, unlike most perspective matrix functions (D3DXMatrixPerspectiveFovLH).
	 */
	YPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);

	/**
	 * Constructor
	 *
	 * @param HalfFOV half Field of View in the Y direction
	 * @param Width view space width
	 * @param Height view space height
	 * @param MinZ distance to the near Z plane
	 * @note that the FOV you pass in is actually half the FOV, unlike most perspective matrix functions (D3DXMatrixPerspectiveFovLH).
	 */
	YPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ);
};


class FReversedZPerspectiveMatrix : public FMatrix
{
public:
	FReversedZPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ);
	FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ);
	FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ);
};


#ifdef _MSC_VER
#pragma warning (push)
// Disable possible division by 0 warning
#pragma warning (disable : 4723)
#endif


FORCEINLINE YPerspectiveMatrix::YPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(MultFOVX / YMath::Tan(HalfFOVX),	0.0f,								0.0f,																	0.0f),
		FPlane(0.0f,							MultFOVY / YMath::Tan(HalfFOVY),	0.0f,																	0.0f),
		FPlane(0.0f,							0.0f,								((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),			1.0f),
		FPlane(0.0f,							0.0f,								-MinZ * ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),	0.0f)
	)
{ }


FORCEINLINE YPerspectiveMatrix::YPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(1.0f / YMath::Tan(HalfFOV),	0.0f,									0.0f,							0.0f),
		FPlane(0.0f,						Width / YMath::Tan(HalfFOV) / Height,	0.0f,							0.0f),
		FPlane(0.0f,						0.0f,									((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),			1.0f),
		FPlane(0.0f,						0.0f,									-MinZ * ((MinZ == MaxZ) ? (1.0f - Z_PRECISION) : MaxZ / (MaxZ - MinZ)),	0.0f)
	)
{ }


FORCEINLINE YPerspectiveMatrix::YPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ)
	: FMatrix(
		FPlane(1.0f / YMath::Tan(HalfFOV),	0.0f,									0.0f,							0.0f),
		FPlane(0.0f,						Width / YMath::Tan(HalfFOV) / Height,	0.0f,							0.0f),
		FPlane(0.0f,						0.0f,									(1.0f - Z_PRECISION),			1.0f),
		FPlane(0.0f,						0.0f,									-MinZ * (1.0f - Z_PRECISION),	0.0f)
	)
{ }


FORCEINLINE FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOVX, float HalfFOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(MultFOVX / YMath::Tan(HalfFOVX),	0.0f,								0.0f,													0.0f),
		FPlane(0.0f,							MultFOVY / YMath::Tan(HalfFOVY),	0.0f,													0.0f),
		FPlane(0.0f,							0.0f,								((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)),			1.0f),
		FPlane(0.0f,							0.0f,								((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)),	0.0f)
	)
{ }


FORCEINLINE FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ, float MaxZ)
	: FMatrix(
		FPlane(1.0f / YMath::Tan(HalfFOV),	0.0f,									0.0f,													0.0f),
		FPlane(0.0f,						Width / YMath::Tan(HalfFOV) / Height,	0.0f,													0.0f),
		FPlane(0.0f,						0.0f,									((MinZ == MaxZ) ? 0.0f : MinZ / (MinZ - MaxZ)),			1.0f),
		FPlane(0.0f,						0.0f,									((MinZ == MaxZ) ? MinZ : -MaxZ * MinZ / (MinZ - MaxZ)),	0.0f)
	)
{ }


FORCEINLINE FReversedZPerspectiveMatrix::FReversedZPerspectiveMatrix(float HalfFOV, float Width, float Height, float MinZ)
	: FMatrix(
		FPlane(1.0f / YMath::Tan(HalfFOV),	0.0f,									0.0f, 0.0f),
		FPlane(0.0f,						Width / YMath::Tan(HalfFOV) / Height,	0.0f, 0.0f),
		FPlane(0.0f,						0.0f,									0.0f, 1.0f),
		FPlane(0.0f,						0.0f,									MinZ, 0.0f)
	)
{ }


#ifdef _MSC_VER
#pragma warning (pop)
#endif
