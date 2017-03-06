// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SolidAngleMathUtility.h"

/**
* Implements a container for rotation information.
*
* All rotation values are stored in degrees.
*/
struct YRotator
{
public:
	/** Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down) */
	float Pitch;

	/** Rotation around the up axis (around Z axis), Running in circles 0=East, +North, -South. */
	float Yaw;

	/** Rotation around the forward axis (around X axis), Tilting your head, 0=Straight, +Clockwise, -CCW. */
	float Roll;

public:

	/** A rotator of zero degrees on each axis. */
	static CORE_API const YRotator ZeroRotator;

public:

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("FRotator contains NaN: %s"), *ToString());
			*const_cast<YRotator*>(this) = ZeroRotator;
		}
	}

	FORCEINLINE void DiagnosticCheckNaN(const TCHAR* Message) const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("%s: FRotator contains NaN: %s"), Message, *ToString());
			*const_cast<YRotator*>(this) = ZeroRotator;
		}
	}
#else
	FORCEINLINE void			DiagnosticCheckNaN() const {}
	FORCEINLINE void			DiagnosticCheckNaN(const TCHAR* Message) const {}
#endif

	/**
	* Default constructor (no initialization).
	*/
	FORCEINLINE					YRotator() { }

	/**
	* Constructor
	*
	* @param InF Value to set all components to.
	*/
	explicit FORCEINLINE		YRotator(float InF);

	/**
	* Constructor.
	*
	* @param InPitch Pitch in degrees.
	* @param InYaw Yaw in degrees.
	* @param InRoll Roll in degrees.
	*/
	FORCEINLINE					YRotator(float InPitch, float InYaw, float InRoll);

	/**
	* Constructor.
	*
	* @param EForceInit Force Init Enum.
	*/
	explicit FORCEINLINE		YRotator(EForceInit);

	/**
	* Constructor.
	*
	* @param Quat Quaternion used to specify rotation.
	*/
	explicit CORE_API			YRotator(const YQuat& Quat);

public:

	// Binary arithmetic operators.

	/**
	* Get the result of adding a rotator to this.
	*
	* @param R The other rotator.
	* @return The result of adding a rotator to this.
	*/
	YRotator operator+(const YRotator& R) const;

	/**
	* Get the result of subtracting a rotator from this.
	*
	* @param R The other rotator.
	* @return The result of subtracting a rotator from this.
	*/
	YRotator operator-(const YRotator& R) const;

	/**
	* Get the result of scaling this rotator.
	*
	* @param Scale The scaling factor.
	* @return The result of scaling.
	*/
	YRotator operator*(float Scale) const;

	/**
	* Multiply this rotator by a scaling factor.
	*
	* @param Scale The scaling factor.
	* @return Copy of the rotator after scaling.
	*/
	YRotator operator*=(float Scale);

	// Unary operators.

	// Binary comparison operators.

	/**
	* Checks whether two rotators are identical. This checks each component for exact equality.
	*
	* @param R The other rotator.
	* @return true if two rotators are identical, otherwise false.
	* @see Equals()
	*/
	bool operator==(const YRotator& R) const;

	/**
	* Checks whether two rotators are different.
	*
	* @param V The other rotator.
	* @return true if two rotators are different, otherwise false.
	*/
	bool operator!=(const YRotator& V) const;

	// Assignment operators.

	/**
	* Adds another rotator to this.
	*
	* @param R The other rotator.
	* @return Copy of rotator after addition.
	*/
	YRotator operator+=(const YRotator& R);

	/**
	* Subtracts another rotator from this.
	*
	* @param R The other rotator.
	* @return Copy of rotator after subtraction.
	*/
	YRotator operator-=(const YRotator& R);

public:

	// Functions.

	/**
	* Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation.
	* This means that FRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator.
	*
	* @param Tolerance Error Tolerance.
	* @return true if rotator is nearly zero, within specified tolerance, otherwise false.
	*/
	bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether this has exactly zero rotation, when treated as an orientation.
	* This means that FRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator.
	*
	* @return true if this has exactly zero rotation, otherwise false.
	*/
	bool IsZero() const;

	/**
	* Checks whether two rotators are equal within specified tolerance, when treated as an orientation.
	* This means that FRotator(0, 0, 360).Equals(FRotator(0,0,0)) is true, because they represent the same final orientation.
	*
	* @param R The other rotator.
	* @param Tolerance Error Tolerance.
	* @return true if two rotators are equal, within specified tolerance, otherwise false.
	*/
	bool Equals(const YRotator& R, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Adds to each component of the rotator.
	*
	* @param DeltaPitch Change in pitch. (+/-)
	* @param DeltaYaw Change in yaw. (+/-)
	* @param DeltaRoll Change in roll. (+/-)
	* @return Copy of rotator after addition.
	*/
	YRotator Add(float DeltaPitch, float DeltaYaw, float DeltaRoll);

	/**
	* Returns the inverse of the rotator.
	*/
	CORE_API YRotator GetInverse() const;

	/**
	* Get the rotation, snapped to specified degree segments.
	*
	* @param RotGrid A Rotator specifying how to snap each component.
	* @return Snapped version of rotation.
	*/
	YRotator GridSnap(const YRotator& RotGrid) const;

	/**
	* Convert a rotation into a unit vector facing in its direction.
	*
	* @return Rotation as a unit direction vector.
	*/
	CORE_API YVector Vector() const;

	/**
	* Get Rotation as a quaternion.
	*
	* @return Rotation as a quaternion.
	*/
	CORE_API YQuat Quaternion() const;

	/**
	* Convert a Rotator into floating-point Euler angles (in degrees). Rotator now stored in degrees.
	*
	* @return Rotation as a Euler angle vector.
	*/
	CORE_API YVector Euler() const;

	/**
	* Rotate a vector rotated by this rotator.
	*
	* @param V The vector to rotate.
	* @return The rotated vector.
	*/
	CORE_API YVector RotateVector(const YVector& V) const;

	/**
	* Returns the vector rotated by the inverse of this rotator.
	*
	* @param V The vector to rotate.
	* @return The rotated vector.
	*/
	CORE_API YVector UnrotateVector(const YVector& V) const;

	/**
	* Gets the rotation values so they fall within the range [0,360]
	*
	* @return Clamped version of rotator.
	*/
	YRotator Clamp() const;

	/**
	* Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation.
	*
	* @return Normalized copy of this rotator
	*/
	YRotator GetNormalized() const;

	/**
	* Create a copy of this rotator and denormalize, clamping each axis to 0 - 360.
	*
	* @return Denormalized copy of this rotator
	*/
	YRotator GetDenormalized() const;

	/**
	* In-place normalize, removes all winding and creates the "shortest route" rotation.
	*/
	void Normalize();

	/**
	* Decompose this Rotator into a Winding part (multiples of 360) and a Remainder part.
	* Remainder will always be in [-180, 180] range.
	*
	* @param Winding[Out] the Winding part of this Rotator
	* @param Remainder[Out] the Remainder
	*/
	CORE_API void GetWindingAndRemainder(YRotator& Winding, YRotator& Remainder) const;

	/**
	* Get a textual representation of the vector.
	*
	* @return Text describing the vector.
	*/
	//!!FIXME by zyx
	//FString ToString() const;

	/** Get a short textural representation of this vector, for compact readable logging. */
	//!!FIXME by zyx
	//FString ToCompactString() const;

	/**
	* Initialize this Rotator based on an FString. The String is expected to contain P=, Y=, R=.
	* The FRotator will be bogus when InitFromString returns false.
	*
	* @param InSourceString	FString containing the rotator values.
	* @return true if the P,Y,R values were read successfully; false otherwise.
	*/
	//!!FIXME by zyx
	//bool InitFromString(const FString& InSourceString);

	/**
	* Utility to check if there are any non-finite values (NaN or Inf) in this Rotator.
	*
	* @return true if there are any non-finite values in this Rotator, otherwise false.
	*/
	bool ContainsNaN() const;

	/**
	* Serializes the rotator compressed for e.g. network transmission.
	*
	* @param	Ar	Archive to serialize to/ from
	*/
	//!!FIXME by zyx
	//CORE_API void SerializeCompressed(FArchive& Ar);

	/**
	* Serializes the rotator compressed for e.g. network transmission (use shorts though).
	*
	* @param	Ar	Archive to serialize to/ from
	*/
	//!!FIXME by zyx
	//CORE_API void SerializeCompressedShort(FArchive& Ar);

	/**
	*/
	//!!FIXME by zyx
	//CORE_API bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

public:

	/**
	* Clamps an angle to the range of [0, 360).
	*
	* @param Angle The angle to clamp.
	* @return The clamped angle.
	*/
	static float ClampAxis(float Angle);

	/**
	* Clamps an angle to the range of (-180, 180].
	*
	* @param Angle The Angle to clamp.
	* @return The clamped angle.
	*/
	static float NormalizeAxis(float Angle);

	/**
	* Compresses a floating point angle into a byte.
	*
	* @param Angle The angle to compress.
	* @return The angle as a byte.
	*/
	static uint8 CompressAxisToByte(float Angle);

	/**
	* Decompress a word into a floating point angle.
	*
	* @param Angle The word angle.
	* @return The decompressed angle.
	*/
	static float DecompressAxisFromByte(uint16 Angle);

	/**
	* Compress a floating point angle into a word.
	*
	* @param Angle The angle to compress.
	* @return The decompressed angle.
	*/
	static uint16 CompressAxisToShort(float Angle);

	/**
	* Decompress a short into a floating point angle.
	*
	* @param Angle The word angle.
	* @return The decompressed angle.
	*/
	static float DecompressAxisFromShort(uint16 Angle);

	/**
	* Convert a vector of floating-point Euler angles (in degrees) into a Rotator. Rotator now stored in degrees
	*
	* @param Euler Euler angle vector.
	* @return A rotator from a Euler angle.
	*/
	static CORE_API YRotator MakeFromEuler(const YVector& Euler);

public:

	/**
	* Serializer.
	*
	* @param Ar Serialization Archive.
	* @param R Rotator being serialized.
	* @return Reference to Archive after serialization.
	*/
	//!!FIXME by zyx
	//friend FArchive& operator<<(FArchive& Ar, YRotator& R)
	//{
	//	Ar << R.Pitch << R.Yaw << R.Roll;
	//	return Ar;
	//}

	//!!FIXME by zyx
	//bool Serialize(FArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}
};


/* FRotator inline functions
*****************************************************************************/

/**
* Scale a rotator and return.
*
* @param Scale scale to apply to R.
* @param R rotator to be scaled.
* @return Scaled rotator.
*/
FORCEINLINE YRotator operator*(float Scale, const YRotator& R)
{
	return R.operator*(Scale);
}


FORCEINLINE YRotator::YRotator(float InF)
	: Pitch(InF), Yaw(InF), Roll(InF)
{
	DiagnosticCheckNaN();
}


FORCEINLINE YRotator::YRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{
	DiagnosticCheckNaN();
}


FORCEINLINE YRotator::YRotator(EForceInit)
	: Pitch(0), Yaw(0), Roll(0)
{}


FORCEINLINE YRotator YRotator::operator+(const YRotator& R) const
{
	return YRotator(Pitch + R.Pitch, Yaw + R.Yaw, Roll + R.Roll);
}


FORCEINLINE YRotator YRotator::operator-(const YRotator& R) const
{
	return YRotator(Pitch - R.Pitch, Yaw - R.Yaw, Roll - R.Roll);
}


FORCEINLINE YRotator YRotator::operator*(float Scale) const
{
	return YRotator(Pitch*Scale, Yaw*Scale, Roll*Scale);
}


FORCEINLINE YRotator YRotator::operator*= (float Scale)
{
	Pitch = Pitch*Scale; Yaw = Yaw*Scale; Roll = Roll*Scale;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE bool YRotator::operator==(const YRotator& R) const
{
	return Pitch == R.Pitch && Yaw == R.Yaw && Roll == R.Roll;
}


FORCEINLINE bool YRotator::operator!=(const YRotator& V) const
{
	return Pitch != V.Pitch || Yaw != V.Yaw || Roll != V.Roll;
}


FORCEINLINE YRotator YRotator::operator+=(const YRotator& R)
{
	Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE YRotator YRotator::operator-=(const YRotator& R)
{
	Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE bool YRotator::IsNearlyZero(float Tolerance) const
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister RegA = VectorLoadFloat3_W0(this);
	const VectorRegister Norm = VectorNormalizeRotator(RegA);
	const VectorRegister AbsNorm = VectorAbs(Norm);
	return !VectorAnyGreaterThan(AbsNorm, VectorLoadFloat1(&Tolerance));
#else
	return
		YMath::Abs(NormalizeAxis(Pitch)) <= Tolerance
		&&	YMath::Abs(NormalizeAxis(Yaw)) <= Tolerance
		&&	YMath::Abs(NormalizeAxis(Roll)) <= Tolerance;
#endif
}


FORCEINLINE bool YRotator::IsZero() const
{
	return (ClampAxis(Pitch) == 0.f) && (ClampAxis(Yaw) == 0.f) && (ClampAxis(Roll) == 0.f);
}


FORCEINLINE bool YRotator::Equals(const YRotator& R, float Tolerance) const
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister RegA = VectorLoadFloat3_W0(this);
	const VectorRegister RegB = VectorLoadFloat3_W0(&R);
	const VectorRegister NormDelta = VectorNormalizeRotator(VectorSubtract(RegA, RegB));
	const VectorRegister AbsNormDelta = VectorAbs(NormDelta);
	return !VectorAnyGreaterThan(AbsNormDelta, VectorLoadFloat1(&Tolerance));
#else
	return (YMath::Abs(NormalizeAxis(Pitch - R.Pitch)) <= Tolerance)
		&& (YMath::Abs(NormalizeAxis(Yaw - R.Yaw)) <= Tolerance)
		&& (YMath::Abs(NormalizeAxis(Roll - R.Roll)) <= Tolerance);
#endif
}


FORCEINLINE YRotator YRotator::Add(float DeltaPitch, float DeltaYaw, float DeltaRoll)
{
	Yaw += DeltaYaw;
	Pitch += DeltaPitch;
	Roll += DeltaRoll;
	DiagnosticCheckNaN();
	return *this;
}


FORCEINLINE YRotator YRotator::GridSnap(const YRotator& RotGrid) const
{
	return YRotator
	(
		YMath::GridSnap(Pitch, RotGrid.Pitch),
		YMath::GridSnap(Yaw, RotGrid.Yaw),
		YMath::GridSnap(Roll, RotGrid.Roll)
	);
}


FORCEINLINE YRotator YRotator::Clamp() const
{
	return YRotator(ClampAxis(Pitch), ClampAxis(Yaw), ClampAxis(Roll));
}


FORCEINLINE float YRotator::ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = YMath::Fmod(Angle, 360.f);

	if (Angle < 0.f)
	{
		// shift to [0,360) range
		Angle += 360.f;
	}

	return Angle;
}


FORCEINLINE float YRotator::NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		// shift to (-180,180]
		Angle -= 360.f;
	}

	return Angle;
}


FORCEINLINE uint8 YRotator::CompressAxisToByte(float Angle)
{
	// map [0->360) to [0->256) and mask off any winding
	return YMath::RoundToInt(Angle * 256.f / 360.f) & 0xFF;
}


FORCEINLINE float YRotator::DecompressAxisFromByte(uint16 Angle)
{
	// map [0->256) to [0->360)
	return (Angle * 360.f / 256.f);
}


FORCEINLINE uint16 YRotator::CompressAxisToShort(float Angle)
{
	// map [0->360) to [0->65536) and mask off any winding
	return YMath::RoundToInt(Angle * 65536.f / 360.f) & 0xFFFF;
}


FORCEINLINE float YRotator::DecompressAxisFromShort(uint16 Angle)
{
	// map [0->65536) to [0->360)
	return (Angle * 360.f / 65536.f);
}


FORCEINLINE YRotator YRotator::GetNormalized() const
{
	YRotator Rot = *this;
	Rot.Normalize();
	return Rot;
}


FORCEINLINE YRotator YRotator::GetDenormalized() const
{
	YRotator Rot = *this;
	Rot.Pitch = ClampAxis(Rot.Pitch);
	Rot.Yaw = ClampAxis(Rot.Yaw);
	Rot.Roll = ClampAxis(Rot.Roll);
	return Rot;
}


FORCEINLINE void YRotator::Normalize()
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	VectorRegister VRotator = VectorLoadFloat3_W0(this);
	VRotator = VectorNormalizeRotator(VRotator);
	VectorStoreFloat3(VRotator, this);
#else
	Pitch = NormalizeAxis(Pitch);
	Yaw = NormalizeAxis(Yaw);
	Roll = NormalizeAxis(Roll);
#endif
	DiagnosticCheckNaN();
}

//!!FIXME by zyx
//FORCEINLINE FString YRotator::ToString() const
//{
//	return FString::Printf(TEXT("P=%f Y=%f R=%f"), Pitch, Yaw, Roll);
//}


//!!FIXME by zyx
//FORCEINLINE FString YRotator::ToCompactString() const
//{
//	if (IsNearlyZero())
//	{
//		return FString::Printf(TEXT("R(0)"));
//	}
//
//	FString ReturnString(TEXT("R("));
//	bool bIsEmptyString = true;
//	if (!YMath::IsNearlyZero(Pitch))
//	{
//		ReturnString += FString::Printf(TEXT("P=%.2f"), Pitch);
//		bIsEmptyString = false;
//	}
//	if (!YMath::IsNearlyZero(Yaw))
//	{
//		if (!bIsEmptyString)
//		{
//			ReturnString += FString(TEXT(", "));
//		}
//		ReturnString += FString::Printf(TEXT("Y=%.2f"), Yaw);
//		bIsEmptyString = false;
//	}
//	if (!YMath::IsNearlyZero(Roll))
//	{
//		if (!bIsEmptyString)
//		{
//			ReturnString += FString(TEXT(", "));
//		}
//		ReturnString += FString::Printf(TEXT("R=%.2f"), Roll);
//		bIsEmptyString = false;
//	}
//	ReturnString += FString(TEXT(")"));
//	return ReturnString;
//}


//!!FIXME by zyx
//FORCEINLINE bool YRotator::InitFromString(const FString& InSourceString)
//{
//	Pitch = Yaw = Roll = 0;
//
//	// The initialization is only successful if the X, Y, and Z values can all be parsed from the string
//	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("P="), Pitch) && FParse::Value(*InSourceString, TEXT("Y="), Yaw) && FParse::Value(*InSourceString, TEXT("R="), Roll);
//	DiagnosticCheckNaN();
//	return bSuccessful;
//}


FORCEINLINE bool YRotator::ContainsNaN() const
{
	return (!YMath::IsFinite(Pitch) ||
		!YMath::IsFinite(Yaw) ||
		!YMath::IsFinite(Roll));
}


//!!FIXME by zyx
//template<> struct TIsPODType<YRotator> { enum { Value = true }; };


/* YMath inline functions
*****************************************************************************/

template<class U>
FORCEINLINE_DEBUGGABLE YRotator YMath::Lerp(const YRotator& A, const YRotator& B, const U& Alpha)
{
	return A + (B - A).GetNormalized() * Alpha;
}

template<class U>
FORCEINLINE_DEBUGGABLE YRotator YMath::LerpRange(const YRotator& A, const YRotator& B, const U& Alpha)
{
	// Similar to Lerp, but does not take the shortest path. Allows interpolation over more than 180 degrees.
	return (A * (1 - Alpha) + B * Alpha).GetNormalized();
}
