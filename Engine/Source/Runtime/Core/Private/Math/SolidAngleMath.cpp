// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	UnMath.cpp: Unreal math routines
=============================================================================*/

#include "Math/SolidAngleMath.h"
#include "Stats/Stats.h"
#include "Math/RandomStream.h"
#include "SObject/PropertyPortFlags.h"

DEFINE_LOG_CATEGORY(LogUnrealMath);

/**
* Math stats
*/

DECLARE_CYCLE_STAT( TEXT( "Convert Rotator to Quat" ), STAT_MathConvertRotatorToQuat, STATGROUP_MathVerbose );
DECLARE_CYCLE_STAT( TEXT( "Convert Quat to Rotator" ), STAT_MathConvertQuatToRotator, STATGROUP_MathVerbose );

/*-----------------------------------------------------------------------------
	Globals
-----------------------------------------------------------------------------*/

CORE_API const YVector YVector::ZeroVector(0.0f, 0.0f, 0.0f);
CORE_API const YVector YVector::UpVector(0.0f, 0.0f, 1.0f);
CORE_API const YVector YVector::ForwardVector(1.0f, 0.0f, 0.0f);
CORE_API const YVector YVector::RightVector(0.0f, 1.0f, 0.0f);
CORE_API const YVector2D YVector2D::ZeroVector(0.0f, 0.0f);
CORE_API const YVector2D YVector2D::UnitVector(1.0f, 1.0f);
CORE_API const YRotator YRotator::ZeroRotator(0.f,0.f,0.f);

CORE_API const VectorRegister VECTOR_INV_255 = DECLARE_VECTOR_REGISTER(1.f/255.f, 1.f/255.f, 1.f/255.f, 1.f/255.f);

CORE_API const uint32 YMath::BitFlag[32] =
{
	(1U << 0),	(1U << 1),	(1U << 2),	(1U << 3),
	(1U << 4),	(1U << 5),	(1U << 6),	(1U << 7),
	(1U << 8),	(1U << 9),	(1U << 10),	(1U << 11),
	(1U << 12),	(1U << 13),	(1U << 14),	(1U << 15),
	(1U << 16),	(1U << 17),	(1U << 18),	(1U << 19),
	(1U << 20),	(1U << 21),	(1U << 22),	(1U << 23),
	(1U << 24),	(1U << 25),	(1U << 26),	(1U << 27),
	(1U << 28),	(1U << 29),	(1U << 30),	(1U << 31),
};

CORE_API const YIntPoint YIntPoint::ZeroValue(0,0);
CORE_API const YIntPoint YIntPoint::NoneValue(INDEX_NONE,INDEX_NONE);
CORE_API const YIntVector YIntVector::ZeroValue(0,0,0);
CORE_API const YIntVector YIntVector::NoneValue(INDEX_NONE,INDEX_NONE,INDEX_NONE);

/** FVectors NetSerialize without quantization. Use the FVectors_NetQuantize etc (NetSerialization.h) instead. */
bool YVector::NetSerialize(YArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << X;
	Ar << Y;
	Ar << Z;
	return true;
}

bool YVector2D::NetSerialize(YArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << X;
	Ar << Y;
	return true;
}

bool YRotator::NetSerialize(YArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	SerializeCompressedShort( Ar );
	bOutSuccess = true;
	return true;
}

void YRotator::SerializeCompressed( YArchive& Ar )
{
	uint8 BytePitch = YRotator::CompressAxisToByte(Pitch);
	uint8 ByteYaw = YRotator::CompressAxisToByte(Yaw);
	uint8 ByteRoll = YRotator::CompressAxisToByte(Roll);

	uint8 B = (BytePitch!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << BytePitch;
	}
	else
	{
		BytePitch = 0;
	}
	
	B = (ByteYaw!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << ByteYaw;
	}
	else
	{
		ByteYaw = 0;
	}
	
	B = (ByteRoll!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << ByteRoll;
	}
	else
	{
		ByteRoll = 0;
	}
	
	if( Ar.IsLoading() )
	{
		Pitch = YRotator::DecompressAxisFromByte(BytePitch);
		Yaw	= YRotator::DecompressAxisFromByte(ByteYaw);
		Roll = YRotator::DecompressAxisFromByte(ByteRoll);
	}
}

void YRotator::SerializeCompressedShort( YArchive& Ar )
{
	uint16 ShortPitch = YRotator::CompressAxisToShort(Pitch);
	uint16 ShortYaw = YRotator::CompressAxisToShort(Yaw);
	uint16 ShortRoll = YRotator::CompressAxisToShort(Roll);

	uint8 B = (ShortPitch!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << ShortPitch;
	}
	else
	{
		ShortPitch = 0;
	}

	B = (ShortYaw!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << ShortYaw;
	}
	else
	{
		ShortYaw = 0;
	}

	B = (ShortRoll!=0);
	Ar.SerializeBits( &B, 1 );
	if( B )
	{
		Ar << ShortRoll;
	}
	else
	{
		ShortRoll = 0;
	}

	if( Ar.IsLoading() )
	{
		Pitch = YRotator::DecompressAxisFromShort(ShortPitch);
		Yaw	= YRotator::DecompressAxisFromShort(ShortYaw);
		Roll = YRotator::DecompressAxisFromShort(ShortRoll);
	}
}

YRotator YVector::ToOrientationRotator() const
{
	YRotator R;

	// Find yaw.
	R.Yaw = YMath::Atan2(Y,X) * (180.f / PI);

	// Find pitch.
	R.Pitch = YMath::Atan2(Z,YMath::Sqrt(X*X+Y*Y)) * (180.f / PI);

	// Find roll.
	R.Roll = 0;

#if ENABLE_NAN_DIAGNOSTIC
	if (R.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("YVector::Rotation(): Rotator result %s contains NaN! Input YVector = %s"), *R.ToString(), *this->ToString());
		R = YRotator::ZeroRotator;
	}
#endif

	return R;
}

YRotator YVector::Rotation() const
{
	return ToOrientationRotator();
}

YRotator YVector4::ToOrientationRotator() const
{
	YRotator R;

	// Find yaw.
	R.Yaw = YMath::Atan2(Y,X) * (180.f / PI);

	// Find pitch.
	R.Pitch = YMath::Atan2(Z,YMath::Sqrt(X*X+Y*Y)) * (180.f / PI);

	// Find roll.
	R.Roll = 0;

#if ENABLE_NAN_DIAGNOSTIC
	if (R.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("YVector4::Rotation(): Rotator result %s contains NaN! Input YVector4 = %s"), *R.ToString(), *this->ToString());
		R = YRotator::ZeroRotator;
	}
#endif

	return R;
}

YRotator YVector4::Rotation() const
{
	return ToOrientationRotator();
}

YQuat YVector::ToOrientationQuat() const
{
	// Essentially an optimized Vector->Rotator->Quat made possible by knowing Roll == 0, and avoiding radians->degrees->radians.
	// This is done to avoid adding any roll (which our API states as a constraint).
	const float YawRad = YMath::Atan2(Y, X);
	const float PitchRad = YMath::Atan2(Z, YMath::Sqrt(X*X + Y*Y));

	const float DIVIDE_BY_2 = 0.5f;
	float SP, SY;
	float CP, CY;

	YMath::SinCos(&SP, &CP, PitchRad * DIVIDE_BY_2);
	YMath::SinCos(&SY, &CY, YawRad * DIVIDE_BY_2);

	YQuat RotationQuat;
	RotationQuat.X =  SP*SY;
	RotationQuat.Y = -SP*CY;
	RotationQuat.Z =  CP*SY;
	RotationQuat.W =  CP*CY;
	return RotationQuat;
}


YQuat YVector4::ToOrientationQuat() const
{
	// Essentially an optimized Vector->Rotator->Quat made possible by knowing Roll == 0, and avoiding radians->degrees->radians.
	// This is done to avoid adding any roll (which our API states as a constraint).
	const float YawRad = YMath::Atan2(Y, X);
	const float PitchRad = YMath::Atan2(Z, YMath::Sqrt(X*X + Y*Y));

	const float DIVIDE_BY_2 = 0.5f;
	float SP, SY;
	float CP, CY;

	YMath::SinCos(&SP, &CP, PitchRad * DIVIDE_BY_2);
	YMath::SinCos(&SY, &CY, YawRad * DIVIDE_BY_2);

	YQuat RotationQuat;
	RotationQuat.X =  SP*SY;
	RotationQuat.Y = -SP*CY;
	RotationQuat.Z =  CP*SY;
	RotationQuat.W =  CP*CY;
	return RotationQuat;
}


void YVector::FindBestAxisVectors( YVector& Axis1, YVector& Axis2 ) const
{
	const float NX = YMath::Abs(X);
	const float NY = YMath::Abs(Y);
	const float NZ = YMath::Abs(Z);

	// Find best basis vectors.
	if( NZ>NX && NZ>NY )	Axis1 = YVector(1,0,0);
	else					Axis1 = YVector(0,0,1);

	Axis1 = (Axis1 - *this * (Axis1 | *this)).GetSafeNormal();
	Axis2 = Axis1 ^ *this;
}

YVector YMath::ClosestPointOnLine(const YVector& LineStart, const YVector& LineEnd, const YVector& Point)
{
	// Solve to find alpha along line that is closest point
	// Weisstein, Eric W. "Point-Line Distance--3-Dimensional." From MathWorld--A Switchram Web Resource. http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html 
	const float A = (LineStart - Point) | (LineEnd - LineStart);
	const float B = (LineEnd - LineStart).SizeSquared();
	// This should be robust to B == 0 (resulting in NaN) because clamp should return 1.
	const float T = YMath::Clamp(-A/B, 0.f, 1.f);

	// Generate closest point
	YVector ClosestPoint = LineStart + (T * (LineEnd - LineStart));

	return ClosestPoint;
}

YVector YMath::ClosestPointOnInfiniteLine(const YVector& LineStart, const YVector& LineEnd, const YVector& Point)
{
	const float A = (LineStart - Point) | (LineEnd - LineStart);
	const float B = (LineEnd - LineStart).SizeSquared();
	if (B < SMALL_NUMBER)
	{
		return LineStart;
	}
	const float T = -A/B;

	// Generate closest point
	const YVector ClosestPoint = LineStart + (T * (LineEnd - LineStart));
	return ClosestPoint;
}

void YVector::CreateOrthonormalBasis(YVector& XAxis,YVector& YAxis,YVector& ZAxis)
{
	// Project the X and Y axes onto the plane perpendicular to the Z axis.
	XAxis -= (XAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;
	YAxis -= (YAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;

	// If the X axis was parallel to the Z axis, choose a vector which is orthogonal to the Y and Z axes.
	if(XAxis.SizeSquared() < DELTA*DELTA)
	{
		XAxis = YAxis ^ ZAxis;
	}

	// If the Y axis was parallel to the Z axis, choose a vector which is orthogonal to the X and Z axes.
	if(YAxis.SizeSquared() < DELTA*DELTA)
	{
		YAxis = XAxis ^ ZAxis;
	}

	// Normalize the basis vectors.
	XAxis.Normalize();
	YAxis.Normalize();
	ZAxis.Normalize();
}

void YVector::UnwindEuler()
{
	X = YMath::UnwindDegrees(X);
	Y = YMath::UnwindDegrees(Y);
	Z = YMath::UnwindDegrees(Z);
}


YRotator::YRotator(const YQuat& Quat)
{
	*this = Quat.Rotator();
	DiagnosticCheckNaN();
}


CORE_API YVector YRotator::Vector() const
{
	float CP, SP, CY, SY;
	YMath::SinCos( &SP, &CP, YMath::DegreesToRadians(Pitch) );
	YMath::SinCos( &SY, &CY, YMath::DegreesToRadians(Yaw) );
	YVector V = YVector( CP*CY, CP*SY, SP );

	return V;
}


YRotator YRotator::GetInverse() const
{
	return Quaternion().Inverse().Rotator();
}


YQuat YRotator::Quaternion() const
{
	SCOPE_CYCLE_COUNTER(STAT_MathConvertRotatorToQuat);

	DiagnosticCheckNaN();

#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister Angles = MakeVectorRegister(Pitch, Yaw, Roll, 0.0f);
	const VectorRegister HalfAngles = VectorMultiply(Angles, GlobalVectorConstants::DEG_TO_RAD_HALF);

	VectorRegister SinAngles, CosAngles;
	VectorSinCos(&SinAngles, &CosAngles, &HalfAngles);

	// Vectorized conversion, measured 20% faster than using scalar version after VectorSinCos.
	// Indices within VectorRegister (for shuffles): P=0, Y=1, R=2
	const VectorRegister SR = VectorReplicate(SinAngles, 2);
	const VectorRegister CR = VectorReplicate(CosAngles, 2);

	const VectorRegister SY_SY_CY_CY_Temp = VectorShuffle(SinAngles, CosAngles, 1, 1, 1, 1);

	const VectorRegister SP_SP_CP_CP = VectorShuffle(SinAngles, CosAngles, 0, 0, 0, 0);
	const VectorRegister SY_CY_SY_CY = VectorShuffle(SY_SY_CY_CY_Temp, SY_SY_CY_CY_Temp, 0, 2, 0, 2);

	const VectorRegister CP_CP_SP_SP = VectorShuffle(CosAngles, SinAngles, 0, 0, 0, 0);
	const VectorRegister CY_SY_CY_SY = VectorShuffle(SY_SY_CY_CY_Temp, SY_SY_CY_CY_Temp, 2, 0, 2, 0);

	const uint32 Neg = uint32(1 << 31);
	const uint32 Pos = uint32(0);
	const VectorRegister SignBitsLeft  = MakeVectorRegister(Pos, Neg, Pos, Pos);
	const VectorRegister SignBitsRight = MakeVectorRegister(Neg, Neg, Neg, Pos);
	const VectorRegister LeftTerm  = VectorBitwiseXor(SignBitsLeft , VectorMultiply(CR, VectorMultiply(SP_SP_CP_CP, SY_CY_SY_CY)));
	const VectorRegister RightTerm = VectorBitwiseXor(SignBitsRight, VectorMultiply(SR, VectorMultiply(CP_CP_SP_SP, CY_SY_CY_SY)));

	YQuat RotationQuat;
	const VectorRegister Result = VectorAdd(LeftTerm, RightTerm);	
	VectorStoreAligned(Result, &RotationQuat);
#else
	const float DEG_TO_RAD = PI/(180.f);
	const float DIVIDE_BY_2 = DEG_TO_RAD/2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	YMath::SinCos(&SP, &CP, Pitch*DIVIDE_BY_2);
	YMath::SinCos(&SY, &CY, Yaw*DIVIDE_BY_2);
	YMath::SinCos(&SR, &CR, Roll*DIVIDE_BY_2);

	YQuat RotationQuat;
	RotationQuat.X =  CR*SP*SY - SR*CP*CY;
	RotationQuat.Y = -CR*SP*CY - SR*CP*SY;
	RotationQuat.Z =  CR*CP*SY - SR*SP*CY;
	RotationQuat.W =  CR*CP*CY + SR*SP*SY;
#endif // PLATFORM_ENABLE_VECTORINTRINSICS

	RotationQuat.DiagnosticCheckNaN();

	return RotationQuat;
}

YVector YRotator::Euler() const
{
	return YVector( Roll, Pitch, Yaw );
}

YRotator YRotator::MakeFromEuler(const YVector& Euler)
{
	return YRotator(Euler.Y, Euler.Z, Euler.X);
}

YVector YRotator::UnrotateVector(const YVector& V) const
{
	return YRotationMatrix(*this).GetTransposed().TransformVector( V );
}	

YVector YRotator::RotateVector(const YVector& V) const
{
	return YRotationMatrix(*this).TransformVector( V );
}	


void YRotator::GetWindingAndRemainder(YRotator& Winding, YRotator& Remainder) const
{
	//// YAW
	Remainder.Yaw = NormalizeAxis(Yaw);

	Winding.Yaw = Yaw - Remainder.Yaw;

	//// PITCH
	Remainder.Pitch = NormalizeAxis(Pitch);

	Winding.Pitch = Pitch - Remainder.Pitch;

	//// ROLL
	Remainder.Roll = NormalizeAxis(Roll);

	Winding.Roll = Roll - Remainder.Roll;
}



YRotator YMatrix::Rotator() const
{
	const YVector		XAxis	= GetScaledAxis( EAxis::X );
	const YVector		YAxis	= GetScaledAxis( EAxis::Y );
	const YVector		ZAxis	= GetScaledAxis( EAxis::Z );

	YRotator	Rotator	= YRotator( 
									YMath::Atan2( XAxis.Z, YMath::Sqrt(YMath::Square(XAxis.X)+YMath::Square(XAxis.Y)) ) * 180.f / PI, 
									YMath::Atan2( XAxis.Y, XAxis.X ) * 180.f / PI, 
									0 
								);
	
	const YVector		SYAxis	= YRotationMatrix( Rotator ).GetScaledAxis( EAxis::Y );
	Rotator.Roll		= YMath::Atan2( ZAxis | SYAxis, YAxis | SYAxis ) * 180.f / PI;

	Rotator.DiagnosticCheckNaN();
	return Rotator;
}


YQuat YMatrix::ToQuat() const
{
	YQuat Result(*this);
	return Result;
}

const YMatrix YMatrix::Identity(YPlane(1,0,0,0),YPlane(0,1,0,0),YPlane(0,0,1,0),YPlane(0,0,0,1));

CORE_API const YQuat YQuat::Identity(0,0,0,1);

YString YMatrix::ToString() const
{
	YString Output;

	Output += YString::Printf(TEXT("[%g %g %g %g] "), M[0][0], M[0][1], M[0][2], M[0][3]);
	Output += YString::Printf(TEXT("[%g %g %g %g] "), M[1][0], M[1][1], M[1][2], M[1][3]);
	Output += YString::Printf(TEXT("[%g %g %g %g] "), M[2][0], M[2][1], M[2][2], M[2][3]);
	Output += YString::Printf(TEXT("[%g %g %g %g] "), M[3][0], M[3][1], M[3][2], M[3][3]);

	return Output;
}

void YMatrix::DebugPrint() const
{
	UE_LOG(LogUnrealMath, Log, TEXT("%s"), *ToString());
}

uint32 YMatrix::ComputeHash() const
{
	uint32 Ret = 0;

	const uint32* Data = (uint32*)this;

	for(uint32 i = 0; i < 16; ++i)
	{
		Ret ^= Data[i] + i;
	}

	return Ret;
}

//////////////////////////////////////////////////////////////////////////
// YQuat

YRotator YQuat::Rotator() const
{
	SCOPE_CYCLE_COUNTER(STAT_MathConvertQuatToRotator);

	DiagnosticCheckNaN();
	const float SingularityTest = Z*X-W*Y;
	const float YawY = 2.f*(W*Z+X*Y);
	const float YawX = (1.f-2.f*(YMath::Square(Y) + YMath::Square(Z)));

	// reference 
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

	// this value was found from experience, the above websites recommend different values
	// but that isn't the case for us, so I went through different testing, and finally found the case 
	// where both of world lives happily. 
	const float SINGULARITY_THRESHOLD = 0.4999995f;
	const float RAD_TO_DEG = (180.f)/PI;
	YRotator RotatorFromQuat;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = -90.f;
		RotatorFromQuat.Yaw = YMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = YRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * YMath::Atan2(X, W) * RAD_TO_DEG));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = 90.f;
		RotatorFromQuat.Yaw = YMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = YRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * YMath::Atan2(X, W) * RAD_TO_DEG));
	}
	else
	{
		RotatorFromQuat.Pitch = YMath::FastAsin(2.f*(SingularityTest)) * RAD_TO_DEG;
		RotatorFromQuat.Yaw = YMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = YMath::Atan2(-2.f*(W*X+Y*Z), (1.f-2.f*(YMath::Square(X) + YMath::Square(Y)))) * RAD_TO_DEG;
	}

#if ENABLE_NAN_DIAGNOSTIC
	if (RotatorFromQuat.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("YQuat::Rotator(): Rotator result %s contains NaN! Quat = %s, YawY = %.9f, YawX = %.9f"), *RotatorFromQuat.ToString(), *this->ToString(), YawY, YawX);
		RotatorFromQuat = YRotator::ZeroRotator;
	}
#endif

	return RotatorFromQuat;
}

YQuat YQuat::MakeFromEuler(const YVector& Euler)
{
	return YRotator::MakeFromEuler(Euler).Quaternion();
}

void YQuat::ToSwingTwist(const YVector& InTwistAxis, YQuat& OutSwing, YQuat& OutTwist) const
{
	// Vector part projected onto twist axis
	YVector Projection = YVector::DotProduct(InTwistAxis, YVector(X, Y, Z)) * InTwistAxis;

	// Twist quaternion
	OutTwist = YQuat(Projection.X, Projection.Y, Projection.Z, W);

	// Singularity close to 180deg
	if(OutTwist.SizeSquared() == 0.0f)
	{
		OutTwist = YQuat::Identity;
	}
	else
	{
		OutTwist.Normalize();
	}

	// Set swing
	OutSwing = *this * OutTwist.Inverse();
}

YMatrix YRotationAboutPointMatrix::Make(const YQuat& Rot, const YVector& Origin)
{
	return YRotationAboutPointMatrix(Rot.Rotator(), Origin);
}

YMatrix YRotationMatrix::Make(YQuat const& Rot)
{
	return YQuatRotationTranslationMatrix(Rot, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromX(YVector const& XAxis)
{
	YVector const NewX = XAxis.GetSafeNormal();

	// try to use up if possible
	YVector const UpVector = ( YMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);

	const YVector NewY = (UpVector ^ NewX).GetSafeNormal();
	const YVector NewZ = NewX ^ NewY;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromY(YVector const& YAxis)
{
	YVector const NewY = YAxis.GetSafeNormal();

	// try to use up if possible
	YVector const UpVector = ( YMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);

	const YVector NewZ = (UpVector ^ NewY).GetSafeNormal();
	const YVector NewX = NewY ^ NewZ;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromZ(YVector const& ZAxis)
{
	YVector const NewZ = ZAxis.GetSafeNormal();

	// try to use up if possible
	YVector const UpVector = ( YMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);

	const YVector NewX = (UpVector ^ NewZ).GetSafeNormal();
	const YVector NewY = NewZ ^ NewX;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromXY(YVector const& XAxis, YVector const& YAxis)
{
	YVector NewX = XAxis.GetSafeNormal();
	YVector Norm = YAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewX | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}

	const YVector NewZ = (NewX ^ Norm).GetSafeNormal();
	const YVector NewY = NewZ ^ NewX;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromXZ(YVector const& XAxis, YVector const& ZAxis)
{
	YVector const NewX = XAxis.GetSafeNormal();
	YVector Norm = ZAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewX | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}

	const YVector NewY = (Norm ^ NewX).GetSafeNormal();
	const YVector NewZ = NewX ^ NewY;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromYX(YVector const& YAxis, YVector const& XAxis)
{
	YVector const NewY = YAxis.GetSafeNormal();
	YVector Norm = XAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewY | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}
	
	const YVector NewZ = (Norm ^ NewY).GetSafeNormal();
	const YVector NewX = NewY ^ NewZ;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromYZ(YVector const& YAxis, YVector const& ZAxis)
{
	YVector const NewY = YAxis.GetSafeNormal();
	YVector Norm = ZAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewY | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}

	const YVector NewX = (NewY ^ Norm).GetSafeNormal();
	const YVector NewZ = NewX ^ NewY;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromZX(YVector const& ZAxis, YVector const& XAxis)
{
	YVector const NewZ = ZAxis.GetSafeNormal();
	YVector Norm = XAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewZ | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}

	const YVector NewY = (NewZ ^ Norm).GetSafeNormal();
	const YVector NewX = NewY ^ NewZ;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YMatrix YRotationMatrix::MakeFromZY(YVector const& ZAxis, YVector const& YAxis)
{
	YVector const NewZ = ZAxis.GetSafeNormal();
	YVector Norm = YAxis.GetSafeNormal();

	// if they're almost same, we need to find arbitrary vector
	if ( YMath::IsNearlyEqual(YMath::Abs(NewZ | Norm), 1.f) )
	{
		// make sure we don't ever pick the same as NewX
		Norm = ( YMath::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER) ) ? YVector(0,0,1.f) : YVector(1.f,0,0);
	}

	const YVector NewX = (Norm ^ NewZ).GetSafeNormal();
	const YVector NewY = NewZ ^ NewX;

	return YMatrix(NewX, NewY, NewZ, YVector::ZeroVector);
}

YVector YQuat::Euler() const
{
	return Rotator().Euler();
}

bool YQuat::NetSerialize(YArchive& Ar, class UPackageMap*, bool& bOutSuccess)
{
	YQuat &Q = *this;

	if (Ar.IsSaving())
	{
		// Make sure we have a non null SquareSum. It shouldn't happen with a quaternion, but better be safe.
		if(Q.SizeSquared() <= SMALL_NUMBER)
		{
			Q = YQuat::Identity;
		}
		else
		{
			// All transmitted quaternions *MUST BE* unit quaternions, in which case we can deduce the value of W.
			Q.Normalize();
			// force W component to be non-negative
			if (Q.W < 0.f)
			{
				Q.X *= -1.f;
				Q.Y *= -1.f;
				Q.Z *= -1.f;
				Q.W *= -1.f;
			}
		}
	}

	Ar << Q.X << Q.Y << Q.Z;
	if ( Ar.IsLoading() )
	{
		const float XYZMagSquared = (Q.X*Q.X + Q.Y*Q.Y + Q.Z*Q.Z);
		const float WSquared = 1.0f - XYZMagSquared;
		// If mag of (X,Y,Z) <= 1.0, then we calculate W to make magnitude of Q 1.0
		if (WSquared >= 0.f)
		{
			Q.W = YMath::Sqrt(WSquared);
		}
		// If mag of (X,Y,Z) > 1.0, we set W to zero, and then renormalize 
		else
		{
			Q.W = 0.f;

			const float XYZInvMag = YMath::InvSqrt(XYZMagSquared);
			Q.X *= XYZInvMag;
			Q.Y *= XYZInvMag;
			Q.Z *= XYZInvMag;
		}
	}

	bOutSuccess = true;
	return true;
}


//
// Based on:
// http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final
// http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
//
FORCEINLINE_DEBUGGABLE YQuat FindBetween_Helper(const YVector& A, const YVector& B, float NormAB)
{
	float W = NormAB + YVector::DotProduct(A, B);
	YQuat Result;

	if (W >= 1e-6f * NormAB)
	{
		//Axis = YVector::CrossProduct(A, B);
		Result = YQuat(A.Y * B.Z - A.Z * B.Y,
					   A.Z * B.X - A.X * B.Z,
					   A.X * B.Y - A.Y * B.X,
					   W);
	}
	else
	{
		// A and B point in opposite directions
		W = 0.f;
		Result = YMath::Abs(A.X) > YMath::Abs(A.Y)
				? YQuat(-A.Z, 0.f, A.X, W)
				: YQuat(0.f, -A.Z, A.Y, W);
	}

	Result.Normalize();
	return Result;
}

YQuat YQuat::FindBetweenNormals(const YVector& A, const YVector& B)
{
	const float NormAB = 1.f;
	return FindBetween_Helper(A, B, NormAB);
}

YQuat YQuat::FindBetweenVectors(const YVector& A, const YVector& B)
{
	const float NormAB = YMath::Sqrt(A.SizeSquared() * B.SizeSquared());
	return FindBetween_Helper(A, B, NormAB);
}

YQuat YQuat::Log() const
{
	YQuat Result;
	Result.W = 0.f;

	if ( YMath::Abs(W) < 1.f )
	{
		const float Angle = YMath::Acos(W);
		const float SinAngle = YMath::Sin(Angle);

		if ( YMath::Abs(SinAngle) >= SMALL_NUMBER )
		{
			const float Scale = Angle/SinAngle;
			Result.X = Scale*X;
			Result.Y = Scale*Y;
			Result.Z = Scale*Z;

			return Result;
		}
	}

	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;

	return Result;
}

YQuat YQuat::Exp() const
{
	const float Angle = YMath::Sqrt(X*X + Y*Y + Z*Z);
	const float SinAngle = YMath::Sin(Angle);

	YQuat Result;
	Result.W = YMath::Cos(Angle);

	if ( YMath::Abs(SinAngle) >= SMALL_NUMBER )
	{
		const float Scale = SinAngle/Angle;
		Result.X = Scale*X;
		Result.Y = Scale*Y;
		Result.Z = Scale*Z;
	}
	else
	{
		Result.X = X;
		Result.Y = Y;
		Result.Z = Z;
	}

	return Result;
}

/*-----------------------------------------------------------------------------
	Swept-Box vs Box test.
-----------------------------------------------------------------------------*/

/* Line-extent/Box Test Util */
bool YMath::LineExtentBoxIntersection(const YBox& inBox, 
								 const YVector& Start, 
								 const YVector& End,
								 const YVector& Extent,
								 YVector& HitLocation,
								 YVector& HitNormal,
								 float& HitTime)
{
	YBox box = inBox;
	box.Max.X += Extent.X;
	box.Max.Y += Extent.Y;
	box.Max.Z += Extent.Z;
	
	box.Min.X -= Extent.X;
	box.Min.Y -= Extent.Y;
	box.Min.Z -= Extent.Z;

	const YVector Dir = (End - Start);
	
	YVector	Time;
	bool	Inside = 1;
	float   faceDir[3] = {1, 1, 1};
	
	/////////////// X
	if(Start.X < box.Min.X)
	{
		if(Dir.X <= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			faceDir[0] = -1;
			Time.X = (box.Min.X - Start.X) / Dir.X;
		}
	}
	else if(Start.X > box.Max.X)
	{
		if(Dir.X >= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			Time.X = (box.Max.X - Start.X) / Dir.X;
		}
	}
	else
		Time.X = 0.0f;
	
	/////////////// Y
	if(Start.Y < box.Min.Y)
	{
		if(Dir.Y <= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			faceDir[1] = -1;
			Time.Y = (box.Min.Y - Start.Y) / Dir.Y;
		}
	}
	else if(Start.Y > box.Max.Y)
	{
		if(Dir.Y >= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			Time.Y = (box.Max.Y - Start.Y) / Dir.Y;
		}
	}
	else
		Time.Y = 0.0f;
	
	/////////////// Z
	if(Start.Z < box.Min.Z)
	{
		if(Dir.Z <= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			faceDir[2] = -1;
			Time.Z = (box.Min.Z - Start.Z) / Dir.Z;
		}
	}
	else if(Start.Z > box.Max.Z)
	{
		if(Dir.Z >= 0.0f)
			return 0;
		else
		{
			Inside = 0;
			Time.Z = (box.Max.Z - Start.Z) / Dir.Z;
		}
	}
	else
		Time.Z = 0.0f;
	
	// If the line started inside the box (ie. player started in contact with the fluid)
	if(Inside)
	{
		HitLocation = Start;
		HitNormal = YVector(0, 0, 1);
		HitTime = 0;
		return 1;
	}
	// Otherwise, calculate when hit occured
	else
	{	
		if(Time.Y > Time.Z)
		{
			HitTime = Time.Y;
			HitNormal = YVector(0, faceDir[1], 0);
		}
		else
		{
			HitTime = Time.Z;
			HitNormal = YVector(0, 0, faceDir[2]);
		}
		
		if(Time.X > HitTime)
		{
			HitTime = Time.X;
			HitNormal = YVector(faceDir[0], 0, 0);
		}
		
		if(HitTime >= 0.0f && HitTime <= 1.0f)
		{
			HitLocation = Start + Dir * HitTime;
			const float BOX_SIDE_THRESHOLD = 0.1f;
			if(	HitLocation.X > box.Min.X - BOX_SIDE_THRESHOLD && HitLocation.X < box.Max.X + BOX_SIDE_THRESHOLD &&
				HitLocation.Y > box.Min.Y - BOX_SIDE_THRESHOLD && HitLocation.Y < box.Max.Y + BOX_SIDE_THRESHOLD &&
				HitLocation.Z > box.Min.Z - BOX_SIDE_THRESHOLD && HitLocation.Z < box.Max.Z + BOX_SIDE_THRESHOLD)
			{				
				return 1;
			}
		}
		
		return 0;
	}
}

float YVector::EvaluateBezier(const YVector* ControlPoints, int32 NumPoints, TArray<YVector>& OutPoints)
{
	check( ControlPoints );
	check( NumPoints >= 2 );

	// var q is the change in t between successive evaluations.
	const float q = 1.f/(NumPoints-1); // q is dependent on the number of GAPS = POINTS-1

	// recreate the names used in the derivation
	const YVector& P0 = ControlPoints[0];
	const YVector& P1 = ControlPoints[1];
	const YVector& P2 = ControlPoints[2];
	const YVector& P3 = ControlPoints[3];

	// coefficients of the cubic polynomial that we're FDing -
	const YVector a = P0;
	const YVector b = 3*(P1-P0);
	const YVector c = 3*(P2-2*P1+P0);
	const YVector d = P3-3*P2+3*P1-P0;

	// initial values of the poly and the 3 diffs -
	YVector S  = a;						// the poly value
	YVector U  = b*q + c*q*q + d*q*q*q;	// 1st order diff (quadratic)
	YVector V  = 2*c*q*q + 6*d*q*q*q;	// 2nd order diff (linear)
	YVector W  = 6*d*q*q*q;				// 3rd order diff (constant)

	// Path length.
	float Length = 0.f;

	YVector OldPos = P0;
	OutPoints.Add( P0 );	// first point on the curve is always P0.

	for( int32 i = 1 ; i < NumPoints ; ++i )
	{
		// calculate the next value and update the deltas
		S += U;			// update poly value
		U += V;			// update 1st order diff value
		V += W;			// update 2st order diff value
		// 3rd order diff is constant => no update needed.

		// Update Length.
		Length += YVector::Dist( S, OldPos );
		OldPos  = S;

		OutPoints.Add( S );
	}

	// Return path length as experienced in sequence (linear interpolation between points).
	return Length;
}

float YLinearColor::EvaluateBezier(const YLinearColor* ControlPoints, int32 NumPoints, TArray<YLinearColor>& OutPoints)
{
	check( ControlPoints );
	check( NumPoints >= 2 );

	// var q is the change in t between successive evaluations.
	const float q = 1.f/(NumPoints-1); // q is dependent on the number of GAPS = POINTS-1

	// recreate the names used in the derivation
	const YLinearColor& P0 = ControlPoints[0];
	const YLinearColor& P1 = ControlPoints[1];
	const YLinearColor& P2 = ControlPoints[2];
	const YLinearColor& P3 = ControlPoints[3];

	// coefficients of the cubic polynomial that we're FDing -
	const YLinearColor a = P0;
	const YLinearColor b = 3*(P1-P0);
	const YLinearColor c = 3*(P2-2*P1+P0);
	const YLinearColor d = P3-3*P2+3*P1-P0;

	// initial values of the poly and the 3 diffs -
	YLinearColor S  = a;						// the poly value
	YLinearColor U  = b*q + c*q*q + d*q*q*q;	// 1st order diff (quadratic)
	YLinearColor V  = 2*c*q*q + 6*d*q*q*q;	// 2nd order diff (linear)
	YLinearColor W  = 6*d*q*q*q;				// 3rd order diff (constant)

	// Path length.
	float Length = 0.f;

	YLinearColor OldPos = P0;
	OutPoints.Add( P0 );	// first point on the curve is always P0.

	for( int32 i = 1 ; i < NumPoints ; ++i )
	{
		// calculate the next value and update the deltas
		S += U;			// update poly value
		U += V;			// update 1st order diff value
		V += W;			// update 2st order diff value
		// 3rd order diff is constant => no update needed.

		// Update Length.
		Length += YLinearColor::Dist( S, OldPos );
		OldPos  = S;

		OutPoints.Add( S );
	}

	// Return path length as experienced in sequence (linear interpolation between points).
	return Length;
}



YQuat YQuat::Slerp_NotNormalized(const YQuat& Quat1,const YQuat& Quat2, float Slerp)
{
	// Get cosine of angle between quats.
	const float RawCosom = 
		    Quat1.X * Quat2.X +
			Quat1.Y * Quat2.Y +
			Quat1.Z * Quat2.Z +
			Quat1.W * Quat2.W;
	// Unaligned quats - compensate, results in taking shorter route.
	const float Cosom = YMath::FloatSelect( RawCosom, RawCosom, -RawCosom );
	
	float Scale0, Scale1;

	if( Cosom < 0.9999f )
	{	
		const float Omega = YMath::Acos(Cosom);
		const float InvSin = 1.f/YMath::Sin(Omega);
		Scale0 = YMath::Sin( (1.f - Slerp) * Omega ) * InvSin;
		Scale1 = YMath::Sin( Slerp * Omega ) * InvSin;
	}
	else
	{
		// Use linear interpolation.
		Scale0 = 1.0f - Slerp;
		Scale1 = Slerp;	
	}

	// In keeping with our flipped Cosom:
	Scale1 = YMath::FloatSelect( RawCosom, Scale1, -Scale1 );

	YQuat Result;
		
	Result.X = Scale0 * Quat1.X + Scale1 * Quat2.X;
	Result.Y = Scale0 * Quat1.Y + Scale1 * Quat2.Y;
	Result.Z = Scale0 * Quat1.Z + Scale1 * Quat2.Z;
	Result.W = Scale0 * Quat1.W + Scale1 * Quat2.W;

	return Result;
}

YQuat YQuat::SlerpFullPath_NotNormalized(const YQuat &quat1, const YQuat &quat2, float Alpha )
{
	const float CosAngle = YMath::Clamp(quat1 | quat2, -1.f, 1.f);
	const float Angle = YMath::Acos(CosAngle);

	//UE_LOG(LogUnrealMath, Log,  TEXT("CosAngle: %f Angle: %f"), CosAngle, Angle );

	if ( YMath::Abs(Angle) < KINDA_SMALL_NUMBER )
	{
		return quat1;
	}

	const float SinAngle = YMath::Sin(Angle);
	const float InvSinAngle = 1.f/SinAngle;

	const float Scale0 = YMath::Sin((1.0f-Alpha)*Angle)*InvSinAngle;
	const float Scale1 = YMath::Sin(Alpha*Angle)*InvSinAngle;

	return quat1*Scale0 + quat2*Scale1;
}

YQuat YQuat::Squad(const YQuat& quat1, const YQuat& tang1, const YQuat& quat2, const YQuat& tang2, float Alpha)
{
	// Always slerp along the short path from quat1 to quat2 to prevent axis flipping.
	// This approach is taken by OGRE engine, amongst others.
	const YQuat Q1 = YQuat::Slerp_NotNormalized(quat1, quat2, Alpha);
	const YQuat Q2 = YQuat::SlerpFullPath_NotNormalized(tang1, tang2, Alpha);
	const YQuat Result = YQuat::SlerpFullPath(Q1, Q2, 2.f * Alpha * (1.f - Alpha));

	return Result;
}

YQuat YQuat::SquadFullPath(const YQuat& quat1, const YQuat& tang1, const YQuat& quat2, const YQuat& tang2, float Alpha)
{
	const YQuat Q1 = YQuat::SlerpFullPath_NotNormalized(quat1, quat2, Alpha);
	const YQuat Q2 = YQuat::SlerpFullPath_NotNormalized(tang1, tang2, Alpha);
	const YQuat Result = YQuat::SlerpFullPath(Q1, Q2, 2.f * Alpha * (1.f - Alpha));

	return Result;
}

void YQuat::CalcTangents(const YQuat& PrevP, const YQuat& P, const YQuat& NextP, float Tension, YQuat& OutTan)
{
	const YQuat InvP = P.Inverse();
	const YQuat Part1 = (InvP * PrevP).Log();
	const YQuat Part2 = (InvP * NextP).Log();

	const YQuat PreExp = (Part1 + Part2) * -0.5f;

	OutTan = P * PreExp.Exp();
}

static void FindBounds( float& OutMin, float& OutMax,  float Start, float StartLeaveTan, float StartT, float End, float EndArriveTan, float EndT, bool bCurve )
{
	OutMin = YMath::Min( Start, End );
	OutMax = YMath::Max( Start, End );

	// Do we need to consider extermeties of a curve?
	if(bCurve)
	{
		// Scale tangents based on time interval, so this code matches the behaviour in FInterpCurve::Eval
		float Diff = EndT - StartT;
		StartLeaveTan *= Diff;
		EndArriveTan *= Diff;

		const float a = 6.f*Start + 3.f*StartLeaveTan + 3.f*EndArriveTan - 6.f*End;
		const float b = -6.f*Start - 4.f*StartLeaveTan - 2.f*EndArriveTan + 6.f*End;
		const float c = StartLeaveTan;

		const float Discriminant = (b*b) - (4.f*a*c);
		if(Discriminant > 0.f && !YMath::IsNearlyZero(a)) // Solving doesn't work if a is zero, which usually indicates co-incident start and end, and zero tangents anyway
		{
			const float SqrtDisc = YMath::Sqrt( Discriminant );

			const float x0 = (-b + SqrtDisc)/(2.f*a); // x0 is the 'Alpha' ie between 0 and 1
			const float t0 = StartT + x0*(EndT - StartT); // Then t0 is the actual 'time' on the curve
			if(t0 > StartT && t0 < EndT)
			{
				const float Val = YMath::CubicInterp( Start, StartLeaveTan, End, EndArriveTan, x0 );

				OutMin = YMath::Min( OutMin, Val );
				OutMax = YMath::Max( OutMax, Val );
			}

			const float x1 = (-b - SqrtDisc)/(2.f*a);
			const float t1 = StartT + x1*(EndT - StartT);
			if(t1 > StartT && t1 < EndT)
			{
				const float Val = YMath::CubicInterp( Start, StartLeaveTan, End, EndArriveTan, x1 );

				OutMin = YMath::Min( OutMin, Val );
				OutMax = YMath::Max( OutMax, Val );
			}
		}
	}
}

void CORE_API CurveFloatFindIntervalBounds( const FInterpCurvePoint<float>& Start, const FInterpCurvePoint<float>& End, float& CurrentMin, float& CurrentMax )
{
	const bool bIsCurve = Start.IsCurveKey();

	float OutMin, OutMax;

	FindBounds(OutMin, OutMax, Start.OutVal, Start.LeaveTangent, Start.InVal, End.OutVal, End.ArriveTangent, End.InVal, bIsCurve);

	CurrentMin = YMath::Min( CurrentMin, OutMin );
	CurrentMax = YMath::Max( CurrentMax, OutMax );
}

void CORE_API CurveVector2DFindIntervalBounds( const FInterpCurvePoint<YVector2D>& Start, const FInterpCurvePoint<YVector2D>& End, YVector2D& CurrentMin, YVector2D& CurrentMax )
{
	const bool bIsCurve = Start.IsCurveKey();

	float OutMin, OutMax;

	FindBounds(OutMin, OutMax, Start.OutVal.X, Start.LeaveTangent.X, Start.InVal, End.OutVal.X, End.ArriveTangent.X, End.InVal, bIsCurve);
	CurrentMin.X = YMath::Min( CurrentMin.X, OutMin );
	CurrentMax.X = YMath::Max( CurrentMax.X, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.Y, Start.LeaveTangent.Y, Start.InVal, End.OutVal.Y, End.ArriveTangent.Y, End.InVal, bIsCurve);
	CurrentMin.Y = YMath::Min( CurrentMin.Y, OutMin );
	CurrentMax.Y = YMath::Max( CurrentMax.Y, OutMax );
}

void CORE_API CurveVectorFindIntervalBounds( const FInterpCurvePoint<YVector>& Start, const FInterpCurvePoint<YVector>& End, YVector& CurrentMin, YVector& CurrentMax )
{
	const bool bIsCurve = Start.IsCurveKey();

	float OutMin, OutMax;

	FindBounds(OutMin, OutMax, Start.OutVal.X, Start.LeaveTangent.X, Start.InVal, End.OutVal.X, End.ArriveTangent.X, End.InVal, bIsCurve);
	CurrentMin.X = YMath::Min( CurrentMin.X, OutMin );
	CurrentMax.X = YMath::Max( CurrentMax.X, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.Y, Start.LeaveTangent.Y, Start.InVal, End.OutVal.Y, End.ArriveTangent.Y, End.InVal, bIsCurve);
	CurrentMin.Y = YMath::Min( CurrentMin.Y, OutMin );
	CurrentMax.Y = YMath::Max( CurrentMax.Y, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.Z, Start.LeaveTangent.Z, Start.InVal, End.OutVal.Z, End.ArriveTangent.Z, End.InVal, bIsCurve);
	CurrentMin.Z = YMath::Min( CurrentMin.Z, OutMin );
	CurrentMax.Z = YMath::Max( CurrentMax.Z, OutMax );
}

void CORE_API CurveTwoVectorsFindIntervalBounds(const FInterpCurvePoint<YTwoVectors>& Start, const FInterpCurvePoint<YTwoVectors>& End, YTwoVectors& CurrentMin, YTwoVectors& CurrentMax)
{
	const bool bIsCurve = Start.IsCurveKey();

	float OutMin;
	float OutMax;

	// Do the first curve
	FindBounds(OutMin, OutMax, Start.OutVal.v1.X, Start.LeaveTangent.v1.X, Start.InVal, End.OutVal.v1.X, End.ArriveTangent.v1.X, End.InVal, bIsCurve);
	CurrentMin.v1.X = YMath::Min( CurrentMin.v1.X, OutMin );
	CurrentMax.v1.X = YMath::Max( CurrentMax.v1.X, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.v1.Y, Start.LeaveTangent.v1.Y, Start.InVal, End.OutVal.v1.Y, End.ArriveTangent.v1.Y, End.InVal, bIsCurve);
	CurrentMin.v1.Y = YMath::Min( CurrentMin.v1.Y, OutMin );
	CurrentMax.v1.Y = YMath::Max( CurrentMax.v1.Y, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.v1.Z, Start.LeaveTangent.v1.Z, Start.InVal, End.OutVal.v1.Z, End.ArriveTangent.v1.Z, End.InVal, bIsCurve);
	CurrentMin.v1.Z = YMath::Min( CurrentMin.v1.Z, OutMin );
	CurrentMax.v1.Z = YMath::Max( CurrentMax.v1.Z, OutMax );

	// Do the second curve
	FindBounds(OutMin, OutMax, Start.OutVal.v2.X, Start.LeaveTangent.v2.X, Start.InVal, End.OutVal.v2.X, End.ArriveTangent.v2.X, End.InVal, bIsCurve);
	CurrentMin.v2.X = YMath::Min( CurrentMin.v2.X, OutMin );
	CurrentMax.v2.X = YMath::Max( CurrentMax.v2.X, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.v2.Y, Start.LeaveTangent.v2.Y, Start.InVal, End.OutVal.v2.Y, End.ArriveTangent.v2.Y, End.InVal, bIsCurve);
	CurrentMin.v2.Y = YMath::Min( CurrentMin.v2.Y, OutMin );
	CurrentMax.v2.Y = YMath::Max( CurrentMax.v2.Y, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.v2.Z, Start.LeaveTangent.v2.Z, Start.InVal, End.OutVal.v2.Z, End.ArriveTangent.v2.Z, End.InVal, bIsCurve);
	CurrentMin.v2.Z = YMath::Min( CurrentMin.v2.Z, OutMin );
	CurrentMax.v2.Z = YMath::Max( CurrentMax.v2.Z, OutMax );
}

void CORE_API CurveLinearColorFindIntervalBounds( const FInterpCurvePoint<YLinearColor>& Start, const FInterpCurvePoint<YLinearColor>& End, YLinearColor& CurrentMin, YLinearColor& CurrentMax )
{
	const bool bIsCurve = Start.IsCurveKey();

	float OutMin, OutMax;

	FindBounds(OutMin, OutMax, Start.OutVal.R, Start.LeaveTangent.R, Start.InVal, End.OutVal.R, End.ArriveTangent.R, End.InVal, bIsCurve);
	CurrentMin.R = YMath::Min( CurrentMin.R, OutMin );
	CurrentMax.R = YMath::Max( CurrentMax.R, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.G, Start.LeaveTangent.G, Start.InVal, End.OutVal.G, End.ArriveTangent.G, End.InVal, bIsCurve);
	CurrentMin.G = YMath::Min( CurrentMin.G, OutMin );
	CurrentMax.G = YMath::Max( CurrentMax.G, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.B, Start.LeaveTangent.B, Start.InVal, End.OutVal.B, End.ArriveTangent.B, End.InVal, bIsCurve);
	CurrentMin.B = YMath::Min( CurrentMin.B, OutMin );
	CurrentMax.B = YMath::Max( CurrentMax.B, OutMax );

	FindBounds(OutMin, OutMax, Start.OutVal.A, Start.LeaveTangent.A, Start.InVal, End.OutVal.A, End.ArriveTangent.A, End.InVal, bIsCurve);
	CurrentMin.A = YMath::Min( CurrentMin.A, OutMin );
	CurrentMax.A = YMath::Max( CurrentMax.A, OutMax );
}

CORE_API float YMath::PointDistToLine(const YVector &Point, const YVector &Direction, const YVector &Origin, YVector &OutClosestPoint)
{
	const YVector SafeDir = Direction.GetSafeNormal();
	OutClosestPoint = Origin + (SafeDir * ((Point-Origin) | SafeDir));
	return (OutClosestPoint-Point).Size();
}

CORE_API float YMath::PointDistToLine(const YVector &Point, const YVector &Direction, const YVector &Origin)
{
	const YVector SafeDir = Direction.GetSafeNormal();
	const YVector OutClosestPoint = Origin + (SafeDir * ((Point-Origin) | SafeDir));
	return (OutClosestPoint-Point).Size();
}

YVector YMath::ClosestPointOnSegment(const YVector &Point, const YVector &StartPoint, const YVector &EndPoint)
{
	const YVector Segment = EndPoint - StartPoint;
	const YVector VectToPoint = Point - StartPoint;

	// See if closest point is before StartPoint
	const float Dot1 = VectToPoint | Segment;
	if( Dot1 <= 0 )
	{
		return StartPoint;
	}

	// See if closest point is beyond EndPoint
	const float Dot2 = Segment | Segment;
	if( Dot2 <= Dot1 )
	{
		return EndPoint;
	}

	// Closest Point is within segment
	return StartPoint + Segment * (Dot1 / Dot2);
}

YVector2D YMath::ClosestPointOnSegment2D(const YVector2D &Point, const YVector2D &StartPoint, const YVector2D &EndPoint)
{
	const YVector2D Segment = EndPoint - StartPoint;
	const YVector2D VectToPoint = Point - StartPoint;

	// See if closest point is before StartPoint
	const float Dot1 = VectToPoint | Segment;
	if (Dot1 <= 0)
	{
		return StartPoint;
	}

	// See if closest point is beyond EndPoint
	const float Dot2 = Segment | Segment;
	if (Dot2 <= Dot1)
	{
		return EndPoint;
	}

	// Closest Point is within segment
	return StartPoint + Segment * (Dot1 / Dot2);
}

float YMath::PointDistToSegment(const YVector &Point, const YVector &StartPoint, const YVector &EndPoint) 
{
	const YVector ClosestPoint = ClosestPointOnSegment(Point, StartPoint, EndPoint);
	return (Point - ClosestPoint).Size();
}

float YMath::PointDistToSegmentSquared(const YVector &Point, const YVector &StartPoint, const YVector &EndPoint) 
{
	const YVector ClosestPoint = ClosestPointOnSegment(Point, StartPoint, EndPoint);
	return (Point - ClosestPoint).SizeSquared();
}

struct SegmentDistToSegment_Solver
{
	SegmentDistToSegment_Solver(const YVector& InA1, const YVector& InB1, const YVector& InA2, const YVector& InB2):
		bLinesAreNearlyParallel(false),
		A1(InA1),
		A2(InA2),
		S1(InB1 - InA1),
		S2(InB2 - InA2),
		S3(InA1 - InA2)
	{
	}

	bool bLinesAreNearlyParallel;

	const YVector& A1;
	const YVector& A2;

	const YVector S1;
	const YVector S2;
	const YVector S3;

	void Solve(YVector& OutP1, YVector& OutP2)
	{
		const float Dot11 = S1 | S1;
		const float Dot12 = S1 | S2;
		const float Dot13 = S1 | S3;
		const float Dot22 = S2 | S2;
		const float Dot23 = S2 | S3;

		const float D = Dot11*Dot22 - Dot12*Dot12;

		float D1 = D;
		float D2 = D;

		float N1;
		float N2;

		if (bLinesAreNearlyParallel || D < KINDA_SMALL_NUMBER)
		{
			// the lines are almost parallel
			N1 = 0.f;	// force using point A on segment S1
			D1 = 1.f;	// to prevent possible division by 0 later
			N2 = Dot23;
			D2 = Dot22;
		}
		else
		{
			// get the closest points on the infinite lines
			N1 = (Dot12*Dot23 - Dot22*Dot13);
			N2 = (Dot11*Dot23 - Dot12*Dot13);

			if (N1 < 0.f)
			{
				// t1 < 0.f => the s==0 edge is visible
				N1 = 0.f;
				N2 = Dot23;
				D2 = Dot22;
			}
			else if (N1 > D1)
			{
				// t1 > 1 => the t1==1 edge is visible
				N1 = D1;
				N2 = Dot23 + Dot12;
				D2 = Dot22;
			}
		}

		if (N2 < 0.f)
		{
			// t2 < 0 => the t2==0 edge is visible
			N2 = 0.f;

			// recompute t1 for this edge
			if (-Dot13 < 0.f)
			{
				N1 = 0.f;
			}
			else if (-Dot13 > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = -Dot13;
				D1 = Dot11;
			}
		}
		else if (N2 > D2)
		{
			// t2 > 1 => the t2=1 edge is visible
			N2 = D2;

			// recompute t1 for this edge
			if ((-Dot13 + Dot12) < 0.f)
			{
				N1 = 0.f;
			}
			else if ((-Dot13 + Dot12) > Dot11)
			{
				N1 = D1;
			}
			else
			{
				N1 = (-Dot13 + Dot12);
				D1 = Dot11;
			}
		}

		// finally do the division to get the points' location
		const float T1 = (YMath::Abs(N1) < KINDA_SMALL_NUMBER ? 0.f : N1 / D1);
		const float T2 = (YMath::Abs(N2) < KINDA_SMALL_NUMBER ? 0.f : N2 / D2);

		// return the closest points
		OutP1 = A1 + T1 * S1;
		OutP2 = A2 + T2 * S2;
	}
};

void YMath::SegmentDistToSegmentSafe(YVector A1, YVector B1, YVector A2, YVector B2, YVector& OutP1, YVector& OutP2)
{
	SegmentDistToSegment_Solver Solver(A1, B1, A2, B2);

	const YVector S1_norm = Solver.S1.GetSafeNormal();
	const YVector S2_norm = Solver.S2.GetSafeNormal();

	const bool bS1IsPoint = S1_norm.IsZero();
	const bool bS2IsPoint = S2_norm.IsZero();

	if (bS1IsPoint && bS2IsPoint)
	{
		OutP1 = A1;
		OutP2 = A2;
	}
	else if (bS2IsPoint)
	{
		OutP1 = ClosestPointOnSegment(A2, A1, B1);
		OutP2 = A2;
	}
	else if (bS1IsPoint)
	{
		OutP1 = A1;
		OutP2 = ClosestPointOnSegment(A1, A2, B2);
	}
	else
	{
		const	float	Dot11_norm = S1_norm | S1_norm;	// always >= 0
		const	float	Dot22_norm = S2_norm | S2_norm;	// always >= 0
		const	float	Dot12_norm = S1_norm | S2_norm;
		const	float	D_norm	= Dot11_norm*Dot22_norm - Dot12_norm*Dot12_norm;	// always >= 0

		Solver.bLinesAreNearlyParallel = D_norm < KINDA_SMALL_NUMBER;
		Solver.Solve(OutP1, OutP2);
	}
}

void YMath::SegmentDistToSegment(YVector A1, YVector B1, YVector A2, YVector B2, YVector& OutP1, YVector& OutP2)
{
	SegmentDistToSegment_Solver(A1, B1, A2, B2).Solve(OutP1, OutP2);
}

float YMath::GetTForSegmentPlaneIntersect(const YVector& StartPoint, const YVector& EndPoint, const YPlane& Plane)
{
	return ( Plane.W - (StartPoint|Plane) ) / ( (EndPoint - StartPoint)|Plane);	
}

bool YMath::SegmentPlaneIntersection(const YVector& StartPoint, const YVector& EndPoint, const YPlane& Plane, YVector& out_IntersectionPoint)
{
	float T = YMath::GetTForSegmentPlaneIntersect(StartPoint, EndPoint, Plane);
	// If the parameter value is not between 0 and 1, there is no intersection
	if (T > -KINDA_SMALL_NUMBER && T < 1.f + KINDA_SMALL_NUMBER)
	{
		out_IntersectionPoint = StartPoint + T * (EndPoint - StartPoint);
		return true;
	}
	return false;
}

bool YMath::SegmentIntersection2D(const YVector& SegmentStartA, const YVector& SegmentEndA, const YVector& SegmentStartB, const YVector& SegmentEndB, YVector& out_IntersectionPoint)
{
	const YVector VectorA = SegmentEndA - SegmentStartA;
	const YVector VectorB = SegmentEndB - SegmentStartB;

	const float S = (-VectorA.Y * (SegmentStartA.X - SegmentStartB.X) + VectorA.X * (SegmentStartA.Y - SegmentStartB.Y)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);
	const float T = (VectorB.X * (SegmentStartA.Y - SegmentStartB.Y) - VectorB.Y * (SegmentStartA.X - SegmentStartB.X)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);

	const bool bIntersects = (S >= 0 && S <= 1 && T >= 0 && T <= 1);

	if (bIntersects)
	{
		out_IntersectionPoint.X = SegmentStartA.X + (T * VectorA.X);
		out_IntersectionPoint.Y = SegmentStartA.Y + (T * VectorA.Y);
		out_IntersectionPoint.Z = SegmentStartA.Z + (T * VectorA.Z);
	}

	return bIntersects;
}

/**
 * Compute the screen bounds of a point light along one axis.
 * Based on http://www.gamasutra.com/features/20021011/lengyel_06.htm
 * and http://sourceforge.net/mailarchive/message.php?msg_id=10501105
 */
static bool ComputeProjectedSphereShaft(
	float LightX,
	float LightZ,
	float Radius,
	const YMatrix& ProjMatrix,
	const YVector& Axis,
	float AxisSign,
	int32& InOutMinX,
	int32& InOutMaxX
	)
{
	float ViewX = InOutMinX;
	float ViewSizeX = InOutMaxX - InOutMinX;

	// Vertical planes: T = <Nx, 0, Nz, 0>
	float Discriminant = (YMath::Square(LightX) - YMath::Square(Radius) + YMath::Square(LightZ)) * YMath::Square(LightZ);
	if(Discriminant >= 0)
	{
		float SqrtDiscriminant = YMath::Sqrt(Discriminant);
		float InvLightSquare = 1.0f / (YMath::Square(LightX) + YMath::Square(LightZ));

		float Nxa = (Radius * LightX - SqrtDiscriminant) * InvLightSquare;
		float Nxb = (Radius * LightX + SqrtDiscriminant) * InvLightSquare;
		float Nza = (Radius - Nxa * LightX) / LightZ;
		float Nzb = (Radius - Nxb * LightX) / LightZ;
		float Pza = LightZ - Radius * Nza;
		float Pzb = LightZ - Radius * Nzb;

		// Tangent a
		if(Pza > 0)
		{
			float Pxa = -Pza * Nza / Nxa;
			YVector4 P = ProjMatrix.TransformFVector4(YVector4(Axis.X * Pxa,Axis.Y * Pxa,Pza,1));
			float X = (Dot3(P,Axis) / P.W + 1.0f * AxisSign) / 2.0f * AxisSign;
			if(YMath::IsNegativeFloat(Nxa) ^ YMath::IsNegativeFloat(AxisSign))
			{
				InOutMaxX = YMath::Min<int64>(YMath::CeilToInt(ViewSizeX * X + ViewX),InOutMaxX);
			}
			else
			{
				InOutMinX = YMath::Max<int64>(YMath::FloorToInt(ViewSizeX * X + ViewX),InOutMinX);
			}
		}

		// Tangent b
		if(Pzb > 0)
		{
			float Pxb = -Pzb * Nzb / Nxb;
			YVector4 P = ProjMatrix.TransformFVector4(YVector4(Axis.X * Pxb,Axis.Y * Pxb,Pzb,1));
			float X = (Dot3(P,Axis) / P.W + 1.0f * AxisSign) / 2.0f * AxisSign;
			if(YMath::IsNegativeFloat(Nxb) ^ YMath::IsNegativeFloat(AxisSign))
			{
				InOutMaxX = YMath::Min<int64>(YMath::CeilToInt(ViewSizeX * X + ViewX),InOutMaxX);
			}
			else
			{
				InOutMinX = YMath::Max<int64>(YMath::FloorToInt(ViewSizeX * X + ViewX),InOutMinX);
			}
		}
	}

	return InOutMinX <= InOutMaxX;
}

uint32 YMath::ComputeProjectedSphereScissorRect(YIntRect& InOutScissorRect, YVector SphereOrigin, float Radius, YVector ViewOrigin, const YMatrix& ViewMatrix, const YMatrix& ProjMatrix)
{
	// Calculate a scissor rectangle for the light's radius.
	if((SphereOrigin - ViewOrigin).SizeSquared() > YMath::Square(Radius))
	{
		YVector LightVector = ViewMatrix.TransformPosition(SphereOrigin);

		if(!ComputeProjectedSphereShaft(
			LightVector.X,
			LightVector.Z,
			Radius,
			ProjMatrix,
			YVector(+1,0,0),
			+1,
			InOutScissorRect.Min.X,
			InOutScissorRect.Max.X))
		{
			return 0;
		}

		if(!ComputeProjectedSphereShaft(
			LightVector.Y,
			LightVector.Z,
			Radius,
			ProjMatrix,
			YVector(0,+1,0),
			-1,
			InOutScissorRect.Min.Y,
			InOutScissorRect.Max.Y))
		{
			return 0;
		}

		return 1;
	}
	else
	{
		return 2;
	}
}

bool YMath::PlaneAABBIntersection(const YPlane& P, const YBox& AABB)
{
	// find diagonal most closely aligned with normal of plane
	YVector Vmin, Vmax;

	// Bypass the slow YVector[] operator. Not RESTRICT because it won't update Vmin, Vmax
	float* VminPtr = (float*)&Vmin;
	float* VmaxPtr = (float*)&Vmax;

	// Use restrict to get better instruction scheduling and to bypass the slow YVector[] operator
	const float* RESTRICT AABBMinPtr = (const float*)&AABB.Min;
	const float* RESTRICT AABBMaxPtr = (const float*)&AABB.Max;
	const float* RESTRICT PlanePtr = (const float*)&P;

	for(int32 Idx=0;Idx<3;++Idx)
	{
		if(PlanePtr[Idx] >= 0.f)
		{
			VminPtr[Idx] = AABBMinPtr[Idx];
			VmaxPtr[Idx] = AABBMaxPtr[Idx];
		}
		else
		{
			VminPtr[Idx] = AABBMaxPtr[Idx];
			VmaxPtr[Idx] = AABBMinPtr[Idx]; 
		}
	}

	// if either diagonal is right on the plane, or one is on either side we have an interesection
	float dMax = P.PlaneDot(Vmax);
	float dMin = P.PlaneDot(Vmin);

	// if Max is below plane, or Min is above we know there is no intersection.. otherwise there must be one
	return (dMax >= 0.f && dMin <= 0.f);
}

bool YMath::SphereConeIntersection(const YVector& SphereCenter, float SphereRadius, const YVector& ConeAxis, float ConeAngleSin, float ConeAngleCos)
{
	/**
	 * from http://www.geometrictools.com/Documentation/IntersectionSphereCone.pdf
	 * (Copyright c 1998-2008. All Rights Reserved.) http://www.geometrictools.com (boost license)
	 */

	// the following code assumes the cone tip is at 0,0,0 (means the SphereCenter is relative to the cone tip)

	YVector U = ConeAxis * (-SphereRadius / ConeAngleSin);
	YVector D = SphereCenter - U;
	float dsqr = D | D;
	float e = ConeAxis | D;

	if(e > 0 && e * e >= dsqr * YMath::Square(ConeAngleCos))
	{
		dsqr = SphereCenter |SphereCenter;
		e = -ConeAxis | SphereCenter;
		if(e > 0 && e*e >= dsqr * YMath::Square(ConeAngleSin))
		{
			return dsqr <= YMath::Square(SphereRadius);
		}
		else
		{
			return true;
		}
	}
	return false;
}

YVector YMath::ClosestPointOnTriangleToPoint(const YVector& Point, const YVector& A, const YVector& B, const YVector& C)
{
	//Figure out what region the point is in and compare against that "point" or "edge"
	const YVector BA = A - B;
	const YVector AC = C - A;
	const YVector CB = B - C;
	const YVector TriNormal = BA ^ CB;

	// Get the planes that define this triangle
	// edges BA, AC, BC with normals perpendicular to the edges facing outward
	const YPlane Planes[3] = { YPlane(B, TriNormal ^ BA), YPlane(A, TriNormal ^ AC), YPlane(C, TriNormal ^ CB) };
	int32 PlaneHalfspaceBitmask = 0;

	//Determine which side of each plane the test point exists
	for (int32 i=0; i<3; i++)
	{
		if (Planes[i].PlaneDot(Point) > 0.0f)
		{
			PlaneHalfspaceBitmask |= (1 << i);
		}
	}

	YVector Result(Point.X, Point.Y, Point.Z);
	switch (PlaneHalfspaceBitmask)
	{
	case 0: //000 Inside
		return YVector::PointPlaneProject(Point, A, B, C);
	case 1:	//001 Segment BA
		Result = YMath::ClosestPointOnSegment(Point, B, A);
		break;
	case 2:	//010 Segment AC
		Result = YMath::ClosestPointOnSegment(Point, A, C);
		break;
	case 3:	//011 point A
		return A;
	case 4: //100 Segment BC
		Result = YMath::ClosestPointOnSegment(Point, B, C);
		break;
	case 5: //101 point B
		return B;
	case 6: //110 point C
		return C;
	default:
		UE_LOG(LogUnrealMath, Log, TEXT("Impossible result in YMath::ClosestPointOnTriangleToPoint"));
		break;
	}

	return Result;
}

YVector YMath::GetBaryCentric2D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C)
{
	float a = ((B.Y-C.Y)*(Point.X-C.X) + (C.X-B.X)*(Point.Y-C.Y)) / ((B.Y-C.Y)*(A.X-C.X) + (C.X-B.X)*(A.Y-C.Y));
	float b = ((C.Y-A.Y)*(Point.X-C.X) + (A.X-C.X)*(Point.Y-C.Y)) / ((B.Y-C.Y)*(A.X-C.X) + (C.X-B.X)*(A.Y-C.Y));

	return YVector(a, b, 1.0f - a - b);	
}

YVector YMath::ComputeBaryCentric2D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C)
{
	// Compute the normal of the triangle
	const YVector TriNorm = (B-A) ^ (C-A);

	//check collinearity of A,B,C
	check(TriNorm.SizeSquared() > SMALL_NUMBER && "Collinear points in YMath::ComputeBaryCentric2D()");

	const YVector N = TriNorm.GetSafeNormal();

	// Compute twice area of triangle ABC
	const float AreaABCInv = 1.0f / (N | TriNorm);

	// Compute a contribution
	const float AreaPBC = N | ((B-Point) ^ (C-Point));
	const float a = AreaPBC * AreaABCInv;

	// Compute b contribution
	const float AreaPCA = N | ((C-Point) ^ (A-Point));
	const float b = AreaPCA * AreaABCInv;

	// Compute c contribution
	return YVector(a, b, 1.0f - a - b);
}

YVector4 YMath::ComputeBaryCentric3D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C, const YVector& D)
{	
	//http://www.devmaster.net/wiki/Barycentric_coordinates
	//Pick A as our origin and
	//Setup three basis vectors AB, AC, AD
	const YVector B1 = (B-A);
	const YVector B2 = (C-A);
	const YVector B3 = (D-A);

	//check co-planarity of A,B,C,D
	check( fabsf(B1 | (B2 ^ B3)) > SMALL_NUMBER && "Coplanar points in YMath::ComputeBaryCentric3D()");

	//Transform Point into this new space
	const YVector V = (Point - A);

	//Create a matrix of linearly independent vectors
	const YMatrix SolvMat(B1, B2, B3, YVector::ZeroVector);

	//The point V can be expressed as Ax=v where x is the vector containing the weights {w1...wn}
	//Solve for x by multiplying both sides by AInv   (AInv * A)x = AInv * v ==> x = AInv * v
	const YMatrix InvSolvMat = SolvMat.Inverse();
	const YPlane BaryCoords = InvSolvMat.TransformVector(V);	 

	//Reorder the weights to be a, b, c, d
	return YVector4(1.0f - BaryCoords.X - BaryCoords.Y - BaryCoords.Z, BaryCoords.X, BaryCoords.Y, BaryCoords.Z);
}

YVector YMath::ClosestPointOnTetrahedronToPoint(const YVector& Point, const YVector& A, const YVector& B, const YVector& C, const YVector& D)
{
	//Check for coplanarity of all four points
	check( fabsf((C-A) | ((B-A)^(D-C))) > 0.0001f && "Coplanar points in YMath::ComputeBaryCentric3D()");

	//http://osdir.com/ml/games.devel.algorithms/2003-02/msg00394.html
	//     D
	//    /|\		  C-----------B
	//   / | \		   \         /
	//  /  |  \	   or	\  \A/	/
    // C   |   B		 \	|  /
	//  \  |  /			  \	| /
    //   \ | /			   \|/
	//     A				D
	
	// Figure out the ordering (is D in the direction of the CCW triangle ABC)
	YVector Pt1(A),Pt2(B),Pt3(C),Pt4(D);
 	const YPlane ABC(A,B,C);
 	if (ABC.PlaneDot(D) < 0.0f)
 	{
 		//Swap two points to maintain CCW orders
 		Pt3 = D;
 		Pt4 = C;
 	}
		
	//Tetrahedron made up of 4 CCW faces - DCA, DBC, DAB, ACB
	const YPlane Planes[4] = {YPlane(Pt4,Pt3,Pt1), YPlane(Pt4,Pt2,Pt3), YPlane(Pt4,Pt1,Pt2), YPlane(Pt1,Pt3,Pt2)};

	//Determine which side of each plane the test point exists
	int32 PlaneHalfspaceBitmask = 0;
	for (int32 i=0; i<4; i++)
	{
		if (Planes[i].PlaneDot(Point) > 0.0f)
		{
			PlaneHalfspaceBitmask |= (1 << i);
		}
	}

	//Verts + Faces - Edges = 2	(Euler)
	YVector Result(Point.X, Point.Y, Point.Z);
	switch (PlaneHalfspaceBitmask)
	{
	case 0:	 //inside (0000)
		//@TODO - could project point onto any face
		break;
	case 1:	 //0001 Face	DCA
		return YMath::ClosestPointOnTriangleToPoint(Point, Pt4, Pt3, Pt1);
	case 2:	 //0010 Face	DBC
		return YMath::ClosestPointOnTriangleToPoint(Point, Pt4, Pt2, Pt3);
	case 3:  //0011	Edge	DC
		Result = YMath::ClosestPointOnSegment(Point, Pt4, Pt3);
		break;
	case 4:	 //0100 Face	DAB
		return YMath::ClosestPointOnTriangleToPoint(Point, Pt4, Pt1, Pt2);
	case 5:  //0101	Edge	DA
		Result = YMath::ClosestPointOnSegment(Point, Pt4, Pt1);
		break;
	case 6:  //0110	Edge	DB
		Result = YMath::ClosestPointOnSegment(Point, Pt4, Pt2);
		break;
	case 7:	 //0111 Point	D
		return Pt4;
	case 8:	 //1000 Face	ACB
		return YMath::ClosestPointOnTriangleToPoint(Point, Pt1, Pt3, Pt2);
	case 9:  //1001	Edge	AC	
		Result = YMath::ClosestPointOnSegment(Point, Pt1, Pt3);
		break;
	case 10: //1010	Edge	BC
		Result = YMath::ClosestPointOnSegment(Point, Pt2, Pt3);
		break;
	case 11: //1011 Point	C
		return Pt3;
	case 12: //1100	Edge	BA
		Result = YMath::ClosestPointOnSegment(Point, Pt2, Pt1);
		break;
	case 13: //1101 Point	A
		return Pt1;
	case 14: //1110 Point	B
		return Pt2;
	default: //impossible (1111)
		UE_LOG(LogUnrealMath, Log, TEXT("YMath::ClosestPointOnTetrahedronToPoint() : impossible result"));
		break;
	}

	return Result;
}

void YMath::SphereDistToLine(YVector SphereOrigin, float SphereRadius, YVector LineOrigin, YVector NormalizedLineDir, YVector& OutClosestPoint)
{
	//const float A = NormalizedLineDir | NormalizedLineDir  (this is 1 because normalized)
	//solving quadratic formula in terms of t where closest point = LineOrigin + t * NormalizedLineDir
	const YVector LineOriginToSphereOrigin = SphereOrigin - LineOrigin;
	const float B = -2.f * (NormalizedLineDir | LineOriginToSphereOrigin);
	const float C = LineOriginToSphereOrigin.SizeSquared() - YMath::Square(SphereRadius);
	const float D	= YMath::Square(B) - 4.f * C;

	if( D <= KINDA_SMALL_NUMBER )
	{
		// line is not intersecting sphere (or is tangent at one point if D == 0 )
		const YVector PointOnLine = LineOrigin + ( -B * 0.5f ) * NormalizedLineDir;
		OutClosestPoint = SphereOrigin + (PointOnLine - SphereOrigin).GetSafeNormal() * SphereRadius;
	}
	else
	{
		// Line intersecting sphere in 2 points. Pick closest to line origin.
		const float	E	= YMath::Sqrt(D);
		const float T1	= (-B + E) * 0.5f;
		const float T2	= (-B - E) * 0.5f;
		const float T	= YMath::Abs( T1 ) == YMath::Abs( T2 ) ? YMath::Abs( T1 ) : YMath::Abs( T1 ) < YMath::Abs( T2 ) ? T1 : T2;	// In the case where both points are exactly the same distance we take the one in the direction of LineDir

		OutClosestPoint	= LineOrigin + T * NormalizedLineDir;
	}
}

bool YMath::GetDistanceWithinConeSegment(YVector Point, YVector ConeStartPoint, YVector ConeLine, float RadiusAtStart, float RadiusAtEnd, float &PercentageOut)
{
	check(RadiusAtStart >= 0.0f && RadiusAtEnd >= 0.0f && ConeLine.SizeSquared() > 0);
	// -- First we'll draw out a line from the ConeStartPoint down the ConeLine. We'll find the closest point on that line to Point.
	//    If we're outside the max distance, or behind the StartPoint, we bail out as that means we've no chance to be in the cone.

	YVector PointOnCone; // Stores the point on the cone's center line closest to our target point.

	const float Distance = YMath::PointDistToLine(Point, ConeLine, ConeStartPoint, PointOnCone); // distance is how far from the viewline we are

	PercentageOut = 0.0; // start assuming we're outside cone until proven otherwise.

	const YVector VectToStart = ConeStartPoint - PointOnCone;
	const YVector VectToEnd = (ConeStartPoint + ConeLine) - PointOnCone;
	
	const float ConeLengthSqr = ConeLine.SizeSquared();
	const float DistToStartSqr = VectToStart.SizeSquared();
	const float DistToEndSqr = VectToEnd.SizeSquared();

	if (DistToStartSqr > ConeLengthSqr || DistToEndSqr > ConeLengthSqr)
	{
		//Outside cone
		return false;
	}

	const float PercentAlongCone = YMath::Sqrt(DistToStartSqr) / YMath::Sqrt(ConeLengthSqr); // don't have to catch outside 0->1 due to above code (saves 2 sqrts if outside)
	const float RadiusAtPoint = RadiusAtStart + ((RadiusAtEnd - RadiusAtStart) * PercentAlongCone);

	if(Distance > RadiusAtPoint) // target is farther from the line than the radius at that distance)
		return false;

	PercentageOut = RadiusAtPoint > 0.0f ? (RadiusAtPoint - Distance) / RadiusAtPoint : 1.0f;

	return true;
}

bool YMath::PointsAreCoplanar(const TArray<YVector>& Points, const float Tolerance)
{
	//less than 4 points = coplanar
	if (Points.Num() < 4)
	{
		return true;
	}

	//Get the Normal for plane determined by first 3 points
	const YVector Normal = YVector::CrossProduct(Points[2] - Points[0], Points[1] - Points[0]).GetSafeNormal();

	const int32 Total = Points.Num();
	for (int32 v = 3; v < Total; v++)
	{
		//Abs of PointPlaneDist, dist should be 0
		if (YMath::Abs(YVector::PointPlaneDist(Points[v], Points[0], Normal)) > Tolerance)
		{
			return false;
		}
	}

	return true;
}

bool YMath::GetDotDistance
( 
			YVector2D	&OutDotDist, 
	const	YVector		&Direction, 
	const	YVector		&AxisX, 
	const	YVector		&AxisY, 
	const	YVector		&AxisZ 	
)
{
	const YVector NormalDir = Direction.GetSafeNormal();

	// Find projected point (on AxisX and AxisY, remove AxisZ component)
	const YVector NoZProjDir = (NormalDir - (NormalDir | AxisZ) * AxisZ).GetSafeNormal();
	
	// Figure out if projection is on right or left.
	const float AzimuthSign = ( (NoZProjDir | AxisY) < 0.f ) ? -1.f : 1.f;

	OutDotDist.Y = NormalDir | AxisZ;
	const float DirDotX	= NoZProjDir | AxisX;
	OutDotDist.X = AzimuthSign * YMath::Abs(DirDotX);

	return (DirDotX >= 0.f );
}

YVector2D YMath::GetAzimuthAndElevation
(
	const YVector &Direction, 
	const YVector &AxisX, 
	const YVector &AxisY, 
	const YVector &AxisZ 	
)
{
	const YVector NormalDir = Direction.GetSafeNormal();
	// Find projected point (on AxisX and AxisY, remove AxisZ component)
	const YVector NoZProjDir = (NormalDir - (NormalDir | AxisZ) * AxisZ).GetSafeNormal();
	// Figure out if projection is on right or left.
	const float AzimuthSign = ((NoZProjDir | AxisY) < 0.f) ? -1.f : 1.f;
	const float ElevationSin = NormalDir | AxisZ;
	const float AzimuthCos = NoZProjDir | AxisX;

	// Convert to Angles in Radian.
	return YVector2D(YMath::Acos(AzimuthCos) * AzimuthSign, YMath::Asin(ElevationSin));
}

CORE_API YVector YMath::VInterpNormalRotationTo(const YVector& Current, const YVector& Target, float DeltaTime, float RotationSpeedDegrees)
{
	// Find delta rotation between both normals.
	YQuat DeltaQuat = YQuat::FindBetween(Current, Target);

	// Decompose into an axis and angle for rotation
	YVector DeltaAxis(0.f);
	float DeltaAngle = 0.f;
	DeltaQuat.ToAxisAndAngle(DeltaAxis, DeltaAngle);

	// Find rotation step for this frame
	const float RotationStepRadians = RotationSpeedDegrees * (PI / 180) * DeltaTime;

	if( YMath::Abs(DeltaAngle) > RotationStepRadians )
	{
		DeltaAngle = YMath::Clamp(DeltaAngle, -RotationStepRadians, RotationStepRadians);
		DeltaQuat = YQuat(DeltaAxis, DeltaAngle);
		return DeltaQuat.RotateVector(Current);
	}
	return Target;
}

CORE_API YVector YMath::VInterpConstantTo(const YVector& Current, const YVector& Target, float DeltaTime, float InterpSpeed)
{
	const YVector Delta = Target - Current;
	const float DeltaM = Delta.Size();
	const float MaxStep = InterpSpeed * DeltaTime;

	if( DeltaM > MaxStep )
	{
		if( MaxStep > 0.f )
		{
			const YVector DeltaN = Delta / DeltaM;
			return Current + DeltaN * MaxStep;
		}
		else
		{
			return Current;
		}
	}

	return Target;
}

CORE_API YVector YMath::VInterpTo( const YVector& Current, const YVector& Target, float DeltaTime, float InterpSpeed )
{
	// If no interp speed, jump to target value
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	// Distance to reach
	const YVector Dist = Target - Current;

	// If distance is too small, just set the desired location
	if( Dist.SizeSquared() < KINDA_SMALL_NUMBER )
	{
		return Target;
	}

	// Delta Move, Clamp so we do not over shoot.
	const YVector	DeltaMove = Dist * YMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return Current + DeltaMove;
}

CORE_API YVector2D YMath::Vector2DInterpConstantTo( const YVector2D& Current, const YVector2D& Target, float DeltaTime, float InterpSpeed )
{
	const YVector2D Delta = Target - Current;
	const float DeltaM = Delta.Size();
	const float MaxStep = InterpSpeed * DeltaTime;

	if( DeltaM > MaxStep )
	{
		if( MaxStep > 0.f )
		{
			const YVector2D DeltaN = Delta / DeltaM;
			return Current + DeltaN * MaxStep;
		}
		else
		{
			return Current;
		}
	}

	return Target;
}

CORE_API YVector2D YMath::Vector2DInterpTo( const YVector2D& Current, const YVector2D& Target, float DeltaTime, float InterpSpeed )
{
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	const YVector2D Dist = Target - Current;
	if( Dist.SizeSquared() < KINDA_SMALL_NUMBER )
	{
		return Target;
	}

	const YVector2D DeltaMove = Dist * YMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);
	return Current + DeltaMove;
}

CORE_API YRotator YMath::RInterpConstantTo( const YRotator& Current, const YRotator& Target, float DeltaTime, float InterpSpeed )
{
	// if DeltaTime is 0, do not perform any interpolation (Location was already calculated for that frame)
	if( DeltaTime == 0.f || Current == Target )
	{
		return Current;
	}

	// If no interp speed, jump to target value
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	const float DeltaInterpSpeed = InterpSpeed * DeltaTime;
	
	const YRotator DeltaMove = (Target - Current).GetNormalized();
	YRotator Result = Current;
	Result.Pitch += YMath::Clamp(DeltaMove.Pitch, -DeltaInterpSpeed, DeltaInterpSpeed);
	Result.Yaw += YMath::Clamp(DeltaMove.Yaw, -DeltaInterpSpeed, DeltaInterpSpeed);
	Result.Roll += YMath::Clamp(DeltaMove.Roll, -DeltaInterpSpeed, DeltaInterpSpeed);
	return Result.GetNormalized();
}

CORE_API YRotator YMath::RInterpTo( const YRotator& Current, const YRotator& Target, float DeltaTime, float InterpSpeed)
{
	// if DeltaTime is 0, do not perform any interpolation (Location was already calculated for that frame)
	if( DeltaTime == 0.f || Current == Target )
	{
		return Current;
	}

	// If no interp speed, jump to target value
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	const float DeltaInterpSpeed = InterpSpeed * DeltaTime;

	const YRotator Delta = (Target - Current).GetNormalized();
	
	// If steps are too small, just return Target and assume we have reached our destination.
	if (Delta.IsNearlyZero())
	{
		return Target;
	}

	// Delta Move, Clamp so we do not over shoot.
	const YRotator DeltaMove = Delta * YMath::Clamp<float>(DeltaInterpSpeed, 0.f, 1.f);
	return (Current + DeltaMove).GetNormalized();
}

CORE_API float YMath::FInterpTo( float Current, float Target, float DeltaTime, float InterpSpeed )
{
	// If no interp speed, jump to target value
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	// Distance to reach
	const float Dist = Target - Current;

	// If distance is too small, just set the desired location
	if( YMath::Square(Dist) < SMALL_NUMBER )
	{
		return Target;
	}

	// Delta Move, Clamp so we do not over shoot.
	const float DeltaMove = Dist * YMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return Current + DeltaMove;
}

CORE_API float YMath::FInterpConstantTo( float Current, float Target, float DeltaTime, float InterpSpeed )
{
	const float Dist = Target - Current;

	// If distance is too small, just set the desired location
	if( YMath::Square(Dist) < SMALL_NUMBER )
	{
		return Target;
	}

	const float Step = InterpSpeed * DeltaTime;
	return Current + YMath::Clamp<float>(Dist, -Step, Step);
}

/** Interpolate Linear Color from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out. */
CORE_API YLinearColor YMath::CInterpTo(const YLinearColor& Current, const YLinearColor& Target, float DeltaTime, float InterpSpeed)
{
	// If no interp speed, jump to target value
	if (InterpSpeed <= 0.f)
	{
		return Target;
	}

	// Difference between colors
	const float Dist = YLinearColor::Dist(Target, Current);

	// If distance is too small, just set the desired color
	if (Dist < KINDA_SMALL_NUMBER)
	{
		return Target;
	}

	// Delta change, Clamp so we do not over shoot.
	const YLinearColor DeltaMove = (Target - Current) * YMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return Current + DeltaMove;
}

CORE_API float ClampFloatTangent( float PrevPointVal, float PrevTime, float CurPointVal, float CurTime, float NextPointVal, float NextTime )
{
	const float PrevToNextTimeDiff = YMath::Max< double >( KINDA_SMALL_NUMBER, NextTime - PrevTime );
	const float PrevToCurTimeDiff = YMath::Max< double >( KINDA_SMALL_NUMBER, CurTime - PrevTime );
	const float CurToNextTimeDiff = YMath::Max< double >( KINDA_SMALL_NUMBER, NextTime - CurTime );

	float OutTangentVal = 0.0f;

	const float PrevToNextHeightDiff = NextPointVal - PrevPointVal;
	const float PrevToCurHeightDiff = CurPointVal - PrevPointVal;
	const float CurToNextHeightDiff = NextPointVal - CurPointVal;

	// Check to see if the current point is crest
	if( ( PrevToCurHeightDiff >= 0.0f && CurToNextHeightDiff <= 0.0f ) ||
		( PrevToCurHeightDiff <= 0.0f && CurToNextHeightDiff >= 0.0f ) )
	{
		// Neighbor points are both both on the same side, so zero out the tangent
		OutTangentVal = 0.0f;
	}
	else
	{
		// The three points form a slope

		// Constants
		const float ClampThreshold = 0.333f;

		// Compute height deltas
		const float CurToNextTangent = CurToNextHeightDiff / CurToNextTimeDiff;
		const float PrevToCurTangent = PrevToCurHeightDiff / PrevToCurTimeDiff;
		const float PrevToNextTangent = PrevToNextHeightDiff / PrevToNextTimeDiff;

		// Default to not clamping
		const float UnclampedTangent = PrevToNextTangent;
		float ClampedTangent = UnclampedTangent;

		const float LowerClampThreshold = ClampThreshold;
		const float UpperClampThreshold = 1.0f - ClampThreshold;

		// @todo: Would we get better results using percentange of TIME instead of HEIGHT?
		const float CurHeightAlpha = PrevToCurHeightDiff / PrevToNextHeightDiff;

		if( PrevToNextHeightDiff > 0.0f )
		{
			if( CurHeightAlpha < LowerClampThreshold )
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
				const float LowerClamp = YMath::Lerp( PrevToNextTangent, PrevToCurTangent, ClampAlpha );
				ClampedTangent = YMath::Min( ClampedTangent, LowerClamp );
			}

			if( CurHeightAlpha > UpperClampThreshold )
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = ( CurHeightAlpha - UpperClampThreshold ) / ClampThreshold;
				const float UpperClamp = YMath::Lerp( PrevToNextTangent, CurToNextTangent, ClampAlpha );
				ClampedTangent = YMath::Min( ClampedTangent, UpperClamp );
			}
		}
		else
		{

			if( CurHeightAlpha < LowerClampThreshold )
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = 1.0f - CurHeightAlpha / ClampThreshold;
				const float LowerClamp = YMath::Lerp( PrevToNextTangent, PrevToCurTangent, ClampAlpha );
				ClampedTangent = YMath::Max( ClampedTangent, LowerClamp );
			}

			if( CurHeightAlpha > UpperClampThreshold )
			{
				// 1.0 = maximum clamping (flat), 0.0 = minimal clamping (don't touch)
				const float ClampAlpha = ( CurHeightAlpha - UpperClampThreshold ) / ClampThreshold;
				const float UpperClamp = YMath::Lerp( PrevToNextTangent, CurToNextTangent, ClampAlpha );
				ClampedTangent = YMath::Max( ClampedTangent, UpperClamp );
			}
		}

		OutTangentVal = ClampedTangent;
	}

	return OutTangentVal;
}

YVector YMath::VRandCone(YVector const& Dir, float ConeHalfAngleRad)
{
	if (ConeHalfAngleRad > 0.f)
	{
		float const RandU = YMath::FRand();
		float const RandV = YMath::FRand();

		// Get spherical coords that have an even distribution over the unit sphere
		// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
		float Theta = 2.f * PI * RandU;
		float Phi = YMath::Acos((2.f * RandV) - 1.f);

		// restrict phi to [0, ConeHalfAngleRad]
		// this gives an even distribution of points on the surface of the cone
		// centered at the origin, pointing upward (z), with the desired angle
		Phi = YMath::Fmod(Phi, ConeHalfAngleRad);

		// get axes we need to rotate around
		YMatrix const DirMat = YRotationMatrix(Dir.Rotation());
		// note the axis translation, since we want the variation to be around X
		YVector const DirZ = DirMat.GetScaledAxis( EAxis::X );		
		YVector const DirY = DirMat.GetScaledAxis( EAxis::Y );

		YVector Result = Dir.RotateAngleAxis(Phi * 180.f / PI, DirY);
		Result = Result.RotateAngleAxis(Theta * 180.f / PI, DirZ);

		// ensure it's a unit vector (might not have been passed in that way)
		Result = Result.GetSafeNormal();
		
		return Result;
	}
	else
	{
		return Dir.GetSafeNormal();
	}
}

YVector YMath::VRandCone(YVector const& Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfAngleRad)
{
	if ( (VerticalConeHalfAngleRad > 0.f) && (HorizontalConeHalfAngleRad > 0.f) )
	{
		float const RandU = YMath::FRand();
		float const RandV = YMath::FRand();

		// Get spherical coords that have an even distribution over the unit sphere
		// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
		float Theta = 2.f * PI * RandU;
		float Phi = YMath::Acos((2.f * RandV) - 1.f);

		// restrict phi to [0, ConeHalfAngleRad]
		// where ConeHalfAngleRad is now a function of Theta
		// (specifically, radius of an ellipse as a function of angle)
		// function is ellipse function (x/a)^2 + (y/b)^2 = 1, converted to polar coords
		float ConeHalfAngleRad = YMath::Square(YMath::Cos(Theta) / VerticalConeHalfAngleRad) + YMath::Square(YMath::Sin(Theta) / HorizontalConeHalfAngleRad);
		ConeHalfAngleRad = YMath::Sqrt(1.f / ConeHalfAngleRad);

		// clamp to make a cone instead of a sphere
		Phi = YMath::Fmod(Phi, ConeHalfAngleRad);

		// get axes we need to rotate around
		YMatrix const DirMat = YRotationMatrix(Dir.Rotation());
		// note the axis translation, since we want the variation to be around X
		YVector const DirZ = DirMat.GetScaledAxis( EAxis::X );		
		YVector const DirY = DirMat.GetScaledAxis( EAxis::Y );

		YVector Result = Dir.RotateAngleAxis(Phi * 180.f / PI, DirY);
		Result = Result.RotateAngleAxis(Theta * 180.f / PI, DirZ);

		// ensure it's a unit vector (might not have been passed in that way)
		Result = Result.GetSafeNormal();

		return Result;
	}
	else
	{
		return Dir.GetSafeNormal();
	}
}

YVector YMath::RandPointInBox(const YBox& Box)
{
	return YVector(	FRandRange(Box.Min.X, Box.Max.X),
					FRandRange(Box.Min.Y, Box.Max.Y),
					FRandRange(Box.Min.Z, Box.Max.Z) );
}

YVector YMath::GetReflectionVector(const YVector& Direction, const YVector& SurfaceNormal)
{
	return Direction - 2 * (Direction | SurfaceNormal.GetSafeNormal()) * SurfaceNormal.GetSafeNormal();
}

struct FClusterMovedHereToMakeCompile
{
	YVector ClusterPosAccum;
	int32 ClusterSize;
};

void YVector::GenerateClusterCenters(TArray<YVector>& Clusters, const TArray<YVector>& Points, int32 NumIterations, int32 NumConnectionsToBeValid)
{
	// Check we have >0 points and clusters
	if(Points.Num() == 0 || Clusters.Num() == 0)
	{
		return;
	}

	// Temp storage for each cluster that mirrors the order of the passed in Clusters array
	TArray<FClusterMovedHereToMakeCompile> ClusterData;
	ClusterData.AddZeroed( Clusters.Num() );

	// Then iterate
	for(int32 ItCount=0; ItCount<NumIterations; ItCount++)
	{
		// Classify each point - find closest cluster center
		for(int32 i=0; i<Points.Num(); i++)
		{
			const YVector& Pos = Points[i];

			// Iterate over all clusters to find closes one
			int32 NearestClusterIndex = INDEX_NONE;
			float NearestClusterDistSqr = BIG_NUMBER;
			for(int32 j=0; j<Clusters.Num() ; j++)
			{
				const float DistSqr = (Pos - Clusters[j]).SizeSquared();
				if(DistSqr < NearestClusterDistSqr)
				{
					NearestClusterDistSqr = DistSqr;
					NearestClusterIndex = j;
				}
			}
			// Update its info with this point
			if( NearestClusterIndex != INDEX_NONE )
			{
				ClusterData[NearestClusterIndex].ClusterPosAccum += Pos;
				ClusterData[NearestClusterIndex].ClusterSize++;
			}
		}

		// All points classified - update cluster center as average of membership
		for(int32 i=0; i<Clusters.Num(); i++)
		{
			if(ClusterData[i].ClusterSize > 0)
			{
				Clusters[i] = ClusterData[i].ClusterPosAccum / (float)ClusterData[i].ClusterSize;
			}
		}
	}

	// so now after we have possible cluster centers we want to remove the ones that are outliers and not part of the main cluster
	for(int32 i=0; i<ClusterData.Num(); i++)
	{
		if(ClusterData[i].ClusterSize < NumConnectionsToBeValid)
		{
			Clusters.RemoveAt(i);
		}
	}
}

namespace MathRoundingUtil
{

float TruncateToHalfIfClose(float F)
{
	float ValueToFudgeIntegralPart = 0.0f;
	float ValueToFudgeFractionalPart = YMath::Modf(F, &ValueToFudgeIntegralPart);
	if (F < 0.0f)
	{
		return ValueToFudgeIntegralPart + ((YMath::IsNearlyEqual(ValueToFudgeFractionalPart, -0.5f)) ? -0.5f : ValueToFudgeFractionalPart);
	}
	else
	{
		return ValueToFudgeIntegralPart + ((YMath::IsNearlyEqual(ValueToFudgeFractionalPart, 0.5f)) ? 0.5f : ValueToFudgeFractionalPart);
	}
}

double TruncateToHalfIfClose(double F)
{
	double ValueToFudgeIntegralPart = 0.0;
	double ValueToFudgeFractionalPart = YMath::Modf(F, &ValueToFudgeIntegralPart);
	if (F < 0.0)
	{
		return ValueToFudgeIntegralPart + ((YMath::IsNearlyEqual(ValueToFudgeFractionalPart, -0.5)) ? -0.5 : ValueToFudgeFractionalPart);
	}
	else
	{
		return ValueToFudgeIntegralPart + ((YMath::IsNearlyEqual(ValueToFudgeFractionalPart, 0.5)) ? 0.5 : ValueToFudgeFractionalPart);
	}
}

} // namespace GenericPlatformMathInternal

float YMath::RoundHalfToEven(float F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);

	const bool bIsNegative = F < 0.0f;
	const bool bValueIsEven = static_cast<uint32>(FloorToFloat(((bIsNegative) ? -F : F))) % 2 == 0;
	if (bValueIsEven)
	{
		// Round towards value (eg, value is -2.5 or 2.5, and should become -2 or 2)
		return (bIsNegative) ? FloorToFloat(F + 0.5f) : CeilToFloat(F - 0.5f);
	}
	else
	{
		// Round away from value (eg, value is -3.5 or 3.5, and should become -4 or 4)
		return (bIsNegative) ? CeilToFloat(F - 0.5f) : FloorToFloat(F + 0.5f);
	}
}

double YMath::RoundHalfToEven(double F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);

	const bool bIsNegative = F < 0.0;
	const bool bValueIsEven = static_cast<uint64>(YMath::FloorToDouble(((bIsNegative) ? -F : F))) % 2 == 0;
	if (bValueIsEven)
	{
		// Round towards value (eg, value is -2.5 or 2.5, and should become -2 or 2)
		return (bIsNegative) ? FloorToDouble(F + 0.5) : CeilToDouble(F - 0.5);
	}
	else
	{
		// Round away from value (eg, value is -3.5 or 3.5, and should become -4 or 4)
		return (bIsNegative) ? CeilToDouble(F - 0.5) : FloorToDouble(F + 0.5);
	}
}

float YMath::RoundHalfFromZero(float F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);
	return (F < 0.0f) ? CeilToFloat(F - 0.5f) : FloorToFloat(F + 0.5f);
}

double YMath::RoundHalfFromZero(double F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);
	return (F < 0.0) ? CeilToDouble(F - 0.5) : FloorToDouble(F + 0.5);
}

float YMath::RoundHalfToZero(float F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);
	return (F < 0.0f) ? FloorToFloat(F + 0.5f) : CeilToFloat(F - 0.5f);
}

double YMath::RoundHalfToZero(double F)
{
	F = MathRoundingUtil::TruncateToHalfIfClose(F);
	return (F < 0.0) ? FloorToDouble(F + 0.5) : CeilToDouble(F - 0.5);
}

YString YMath::FormatIntToHumanReadable(int32 Val)
{
	YString Src = *YString::Printf(TEXT("%i"), Val);
	YString Dst;

	if (Val > 999)
	{
		Dst = YString::Printf(TEXT(",%s"), *Src.Mid(Src.Len() - 3, 3));
		Src = Src.Left(Src.Len() - 3);

	}

	if (Val > 999999)
	{
		Dst = YString::Printf(TEXT(",%s%s"), *Src.Mid(Src.Len() - 3, 3), *Dst);
		Src = Src.Left(Src.Len() - 3);
	}

	Dst = Src + Dst;

	return Dst;
}

bool YMath::MemoryTest( void* BaseAddress, uint32 NumBytes )
{
	volatile uint32* Ptr;
	uint32 NumDwords = NumBytes / 4;
	uint32 TestWords[2] = { 0xdeadbeef, 0x1337c0de };
	bool bSucceeded = true;

	for ( int32 TestIndex=0; TestIndex < 2; ++TestIndex )
	{
		// Fill the memory with a pattern.
		Ptr = (uint32*) BaseAddress;
		for ( uint32 Index=0; Index < NumDwords; ++Index )
		{
			*Ptr = TestWords[TestIndex];
			Ptr++;
		}

		// Check that each uint32 is still ok and overwrite it with the complement.
		Ptr = (uint32*) BaseAddress;
		for ( uint32 Index=0; Index < NumDwords; ++Index )
		{
			if ( *Ptr != TestWords[TestIndex] )
			{
				YPlatformMisc::LowLevelOutputDebugStringf(TEXT("Failed memory test at 0x%08x, wrote: 0x%08x, read: 0x%08x\n"), Ptr, TestWords[TestIndex], *Ptr );
				bSucceeded = false;
			}
			*Ptr = ~TestWords[TestIndex];
			Ptr++;
		}

		// Check again, now going backwards in memory.
		Ptr = ((uint32*) BaseAddress) + NumDwords;
		for ( uint32 Index=0; Index < NumDwords; ++Index )
		{
			Ptr--;
			if ( *Ptr != ~TestWords[TestIndex] )
			{
				YPlatformMisc::LowLevelOutputDebugStringf(TEXT("Failed memory test at 0x%08x, wrote: 0x%08x, read: 0x%08x\n"), Ptr, ~TestWords[TestIndex], *Ptr );
				bSucceeded = false;
			}
			*Ptr = TestWords[TestIndex];
		}
	}

	return bSucceeded;
}

/**
 * Converts a string to it's numeric equivalent, ignoring whitespace.
 * "123  45" - becomes 12,345
 *
 * @param	Value	The string to convert.
 * @return			The converted value.
 */
float Val(const YString& Value)
{
	float RetValue = 0;

	for( int32 x = 0 ; x < Value.Len() ; x++ )
	{
		YString Char = Value.Mid(x, 1);

		if( Char >= TEXT("0") && Char <= TEXT("9") )
		{
			RetValue *= 10;
			RetValue += FCString::Atoi( *Char );
		}
		else 
		{
			if( Char != TEXT(" ") )
			{
				break;
			}
		}
	}

	return RetValue;
}

YString GrabChar( YString* pStr )
{
	YString GrabChar;
	if( pStr->Len() )
	{
		do
		{		
			GrabChar = pStr->Left(1);
			*pStr = pStr->Mid(1);
		} while( GrabChar == TEXT(" ") );
	}
	else
	{
		GrabChar = TEXT("");
	}

	return GrabChar;
}

bool SubEval( YString* pStr, float* pResult, int32 Prec )
{
	YString c;
	float V, W, N;

	V = W = N = 0.0f;

	c = GrabChar(pStr);

	if( (c >= TEXT("0") && c <= TEXT("9")) || c == TEXT(".") )	// Number
	{
		V = 0;
		while(c >= TEXT("0") && c <= TEXT("9"))
		{
			V = V * 10 + Val(c);
			c = GrabChar(pStr);
		}

		if( c == TEXT(".") )
		{
			N = 0.1f;
			c = GrabChar(pStr);

			while(c >= TEXT("0") && c <= TEXT("9"))
			{
				V = V + N * Val(c);
				N = N / 10.0f;
				c = GrabChar(pStr);
			}
		}
	}
	else if( c == TEXT("("))									// Opening parenthesis
	{
		if( !SubEval(pStr, &V, 0) )
		{
			return 0;
		}
		c = GrabChar(pStr);
	}
	else if( c == TEXT("-") )									// Negation
	{
		if( !SubEval(pStr, &V, 1000) )
		{
			return 0;
		}
		V = -V;
		c = GrabChar(pStr);
	}
	else if( c == TEXT("+"))									// Positive
	{
		if( !SubEval(pStr, &V, 1000) )
		{
			return 0;
		}
		c = GrabChar(pStr);
	}
	else if( c == TEXT("@") )									// Square root
	{
		if( !SubEval(pStr, &V, 1000) )
		{
			return 0;
		}

		if( V < 0 )
		{
			UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Can't take square root of negative number"));
			return 0;
		}
		else
		{
			V = YMath::Sqrt(V);
		}

		c = GrabChar(pStr);
	}
	else														// Error
	{
		UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : No value recognized"));
		return 0;
	}
PrecLoop:
	if( c == TEXT("") )
	{
		*pResult = V;
		return 1;
	}
	else if( c == TEXT(")") )
	{
		*pStr = YString(TEXT(")")) + *pStr;
		*pResult = V;
		return 1;
	}
	else if( c == TEXT("+") )
	{
		if( Prec > 1 )
		{
			*pResult = V;
			*pStr = c + *pStr;
			return 1;
		}
		else
		{
			if( SubEval(pStr, &W, 2) )
			{
				V = V + W;
				c = GrabChar(pStr);
				goto PrecLoop;
			}
			else
			{
				return 0;
			}
		}
	}
	else if( c == TEXT("-") )
	{
		if( Prec > 1 )
		{
			*pResult = V;
			*pStr = c + *pStr;
			return 1;
		}
		else
		{
			if( SubEval(pStr, &W, 2) )
			{
				V = V - W;
				c = GrabChar(pStr);
				goto PrecLoop;
			}
			else
			{
				return 0;
			}
		}
	}
	else if( c == TEXT("/") )
	{
		if( Prec > 2 )
		{
			*pResult = V;
			*pStr = c + *pStr;
			return 1;
		}
		else
		{
			if( SubEval(pStr, &W, 3) )
			{
				if( W == 0 )
				{
					UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Division by zero isn't allowed"));
					return 0;
				}
				else
				{
					V = V / W;
					c = GrabChar(pStr);
					goto PrecLoop;
				}
			}
			else
			{
				return 0;
			}
		}
	}
	else if( c == TEXT("%") )
	{
		if( Prec > 2 )
		{
			*pResult = V;
			*pStr = c + *pStr;
			return 1;
		}
		else
		{
			if( SubEval(pStr, &W, 3) )
			{
				if( W == 0 )
				{
					UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Modulo zero isn't allowed"));
					return 0;
				}
				else
				{
					V = (int32)V % (int32)W;
					c = GrabChar(pStr);
					goto PrecLoop;
				}
			}
			else
			{
				return 0;
			}
		}
	}
	else if( c == TEXT("*") )
	{
		if( Prec > 3 )
		{
			*pResult = V;
			*pStr = c + *pStr;
			return 1;
		}
		else
		{
			if( SubEval(pStr, &W, 4) )
			{
				V = V * W;
				c = GrabChar(pStr);
				goto PrecLoop;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Unrecognized Operator"));
	}

	*pResult = V;
	return 1;
}

bool YMath::Eval( YString Str, float& OutValue )
{
	bool bResult = true;

	// Check for a matching number of brackets right up front.
	int32 Brackets = 0;
	for( int32 x = 0 ; x < Str.Len() ; x++ )
	{
		if( Str.Mid(x,1) == TEXT("(") )
		{
			Brackets++;
		}
		if( Str.Mid(x,1) == TEXT(")") )
		{
			Brackets--;
		}
	}

	if( Brackets != 0 )
	{
		UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Mismatched brackets"));
		bResult = false;
	}

	else
	{
		if( !SubEval( &Str, &OutValue, 0 ) )
		{
			UE_LOG(LogUnrealMath, Log, TEXT("Expression Error : Error in expression"));
			bResult = false;
		}
	}

	return bResult;
}

void YMath::WindRelativeAnglesDegrees(float InAngle0, float& InOutAngle1)
{
	const float Diff = InAngle0 - InOutAngle1;
	const float AbsDiff = Abs(Diff);
	if(AbsDiff > 180.0f)
	{
		InOutAngle1 += 360.0f * Sign(Diff) * FloorToFloat((AbsDiff / 360.0f) + 0.5f);
	}
}

float YMath::FixedTurn(float InCurrent, float InDesired, float InDeltaRate)
{
	if (InDeltaRate == 0.f)
	{
		return YRotator::ClampAxis(InCurrent);
	}

	if (InDeltaRate >= 360.f)
	{
		return YRotator::ClampAxis(InDesired);
	}

	float result = YRotator::ClampAxis(InCurrent);
	InCurrent = result;
	InDesired = YRotator::ClampAxis(InDesired);

	if (InCurrent > InDesired)
	{
		if (InCurrent - InDesired < 180.f)
			result -= YMath::Min((InCurrent - InDesired), YMath::Abs(InDeltaRate));
		else
			result += YMath::Min((InDesired + 360.f - InCurrent), YMath::Abs(InDeltaRate));
	}
	else
	{
		if (InDesired - InCurrent < 180.f)
			result += YMath::Min((InDesired - InCurrent), YMath::Abs(InDeltaRate));
		else
			result -= YMath::Min((InCurrent + 360.f - InDesired), YMath::Abs(InDeltaRate));
	}
	return YRotator::ClampAxis(result);
}

float YMath::ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees)
{
	float const MaxDelta = YRotator::ClampAxis(MaxAngleDegrees - MinAngleDegrees) * 0.5f;			// 0..180
	float const RangeCenter = YRotator::ClampAxis(MinAngleDegrees + MaxDelta);						// 0..360
	float const DeltaFromCenter = YRotator::NormalizeAxis(AngleDegrees - RangeCenter);				// -180..180

	// maybe clamp to nearest edge
	if (DeltaFromCenter > MaxDelta)
	{
		return YRotator::NormalizeAxis(RangeCenter + MaxDelta);
	}
	else if (DeltaFromCenter < -MaxDelta)
	{
		return YRotator::NormalizeAxis(RangeCenter - MaxDelta);
	}

	// already in range, just return it
	return YRotator::NormalizeAxis(AngleDegrees);
}

void YMath::ApplyScaleToFloat(float& Dst, const YVector& DeltaScale, float Magnitude)
{
	const float Multiplier = ( DeltaScale.X > 0.0f || DeltaScale.Y > 0.0f || DeltaScale.Z > 0.0f ) ? Magnitude : -Magnitude;
	Dst += Multiplier * DeltaScale.Size();
	Dst = YMath::Max( 0.0f, Dst );
}

void YMath::CartesianToPolar(const YVector2D InCart, YVector2D& OutPolar)
{
	OutPolar.X = Sqrt(Square(InCart.X) + Square(InCart.Y));
	OutPolar.Y = Atan2(InCart.Y, InCart.X);
}

void YMath::PolarToCartesion(const YVector2D InPolar, YVector2D& OutCart)
{
	OutCart.X = InPolar.X * Cos(InPolar.Y);
	OutCart.Y = InPolar.X * Sin(InPolar.Y);
}

bool YRandomStream::ExportTextItem(YString& ValueStr, YRandomStream const& DefaultValue, class SObject* Parent, int32 PortFlags, class SObject* ExportRootScope) const
{
	if (0 != (PortFlags & EPropertyPortFlags::PPF_ExportCpp))
	{
		ValueStr += YString::Printf(TEXT("FRandomStream(%i)"), DefaultValue.GetInitialSeed());
		return true;
	}
	return false;
}
