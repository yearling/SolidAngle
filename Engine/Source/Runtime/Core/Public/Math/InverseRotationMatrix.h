// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/SolidAngleMathUtility.h"
#include "Math/Plane.h"
#include "Math/Matrix.h"

/** Inverse Rotation matrix */
class FInverseRotationMatrix
	: public FMatrix
{
public:
	/**
	 * Constructor.
	 *
	 * @param Rot rotation
	 */
	FInverseRotationMatrix(const FRotator& Rot);
};


FORCEINLINE FInverseRotationMatrix::FInverseRotationMatrix(const FRotator& Rot)
	: FMatrix(
		FMatrix( // Yaw
		FPlane(+YMath::Cos(Rot.Yaw * PI / 180.f), -YMath::Sin(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
		FPlane(+YMath::Sin(Rot.Yaw * PI / 180.f), +YMath::Cos(Rot.Yaw * PI / 180.f), 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, 1.0f, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Pitch
		FPlane(+YMath::Cos(Rot.Pitch * PI / 180.f), 0.0f, -YMath::Sin(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, 1.0f, 0.0f, 0.0f),
		FPlane(+YMath::Sin(Rot.Pitch * PI / 180.f), 0.0f, +YMath::Cos(Rot.Pitch * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f)) *
		FMatrix( // Roll
		FPlane(1.0f, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, +YMath::Cos(Rot.Roll * PI / 180.f), +YMath::Sin(Rot.Roll * PI / 180.f), 0.0f),
		FPlane(0.0f, -YMath::Sin(Rot.Roll * PI / 180.f), +YMath::Cos(Rot.Roll * PI / 180.f), 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f))
	)
{ }
