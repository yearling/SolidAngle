// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Math/SolidAngleMathUtility.h"
#include "Containers/SolidAngleString.h"
#include "Logging/LogMacros.h"
#include "Math/Vector.h"
#include "Math/VectorRegister.h"
#include "Math/Rotator.h"
#include "Math/Matrix.h"

class Error;

/**
* Floating point quaternion that can represent a rotation about an axis in 3-D space.
* The X, Y, Z, W components also double as the Axis/Angle format.
*
* Order matters when composing quaternions: C = A * B will yield a quaternion C that logically
* first applies B then A to any subsequent transformation (right first, then left).
* Note that this is the opposite order of FTransform multiplication.
*
* Example: LocalToWorld = (LocalToWorld * DeltaRotation) will change rotation in local space by DeltaRotation.
* Example: LocalToWorld = (DeltaRotation * LocalToWorld) will change rotation in world space by DeltaRotation.
*/
MS_ALIGN(16) struct YQuat
{
public:

	/** The quaternion's X-component. */
	float X;

	/** The quaternion's Y-component. */
	float Y;

	/** The quaternion's Z-component. */
	float Z;

	/** The quaternion's W-component. */
	float W;

public:

	/** Identity quaternion. */
	static CORE_API const YQuat Identity;

public:

	/** Default constructor (no initialization). */
	FORCEINLINE YQuat() { }

	/**
	* Creates and initializes a new quaternion, with the W component either 0 or 1.
	*
	* @param EForceInit Force init enum: if equal to ForceInitToZero then W is 0, otherwise W = 1 (creating an identity transform)
	*/
	explicit FORCEINLINE YQuat(EForceInit);

	/**
	* Constructor.
	*
	* @param InX X component of the quaternion
	* @param InY Y component of the quaternion
	* @param InZ Z component of the quaternion
	* @param InW W component of the quaternion
	*/
	FORCEINLINE YQuat(float InX, float InY, float InZ, float InW);

	/**
	* Copy constructor.
	*
	* @param Q A YQuat object to use to create new quaternion from.
	*/
	FORCEINLINE YQuat(const YQuat& Q);

	/**
	* Creates and initializes a new quaternion from the given matrix.
	*
	* @param M The rotation matrix to initialize from.
	*/
	explicit YQuat(const YMatrix& M);

	/**
	* Creates and initializes a new quaternion from the given rotator.
	*
	* @param R The rotator to initialize from.
	*/
	explicit YQuat(const YRotator& R);

	/**
	* Creates and initializes a new quaternion from the a rotation around the given axis.
	*
	* @param Axis assumed to be a normalized vector
	* @param Angle angle to rotate above the given axis (in radians)
	*/
	YQuat(YVector Axis, float AngleRad);

public:

#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
	/**
	* Copy another YQuat into this one
	*
	* @return reference to this YQuat
	*/
	FORCEINLINE YQuat& operator=(const YQuat& Other);
#endif

	/**
	* Gets the result of adding a Quaternion to this.
	* This is a component-wise addition; composing quaternions should be done via multiplication.
	*
	* @param Q The Quaternion to add.
	* @return The result of addition.
	*/
	FORCEINLINE YQuat operator+(const YQuat& Q) const;

	/**
	* Adds to this quaternion.
	* This is a component-wise addition; composing quaternions should be done via multiplication.
	*
	* @param Other The quaternion to add to this.
	* @return Result after addition.
	*/
	FORCEINLINE YQuat operator+=(const YQuat& Q);

	/**
	* Gets the result of subtracting a Quaternion to this.
	* This is a component-wise subtraction; composing quaternions should be done via multiplication.
	*
	* @param Q The Quaternion to subtract.
	* @return The result of subtraction.
	*/
	FORCEINLINE YQuat operator-(const YQuat& Q) const;

	/**
	* Checks whether another Quaternion is equal to this, within specified tolerance.
	*
	* @param Q The other Quaternion.
	* @param Tolerance Error tolerance for comparison with other Quaternion.
	* @return true if two Quaternions are equal, within specified tolerance, otherwise false.
	*/
	FORCEINLINE bool Equals(const YQuat& Q, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether this Quaternion is an Identity Quaternion.
	* Assumes Quaternion tested is normalized.
	*
	* @param Tolerance Error tolerance for comparison with Identity Quaternion.
	* @return true if Quaternion is a normalized Identity Quaternion.
	*/
	FORCEINLINE bool IsIdentity(float Tolerance = SMALL_NUMBER) const;

	/**
	* Subtracts another quaternion from this.
	* This is a component-wise subtraction; composing quaternions should be done via multiplication.
	*
	* @param Q The other quaternion.
	* @return reference to this after subtraction.
	*/
	FORCEINLINE YQuat operator-=(const YQuat& Q);

	/**
	* Gets the result of multiplying this by another quaternion (this * Q).
	*
	* Order matters when composing quaternions: C = A * B will yield a quaternion C that logically
	* first applies B then A to any subsequent transformation (right first, then left).
	*
	* @param Q The Quaternion to multiply this by.
	* @return The result of multiplication (this * Q).
	*/
	FORCEINLINE YQuat operator*(const YQuat& Q) const;

	/**
	* Multiply this by a quaternion (this = this * Q).
	*
	* Order matters when composing quaternions: C = A * B will yield a quaternion C that logically
	* first applies B then A to any subsequent transformation (right first, then left).
	*
	* @param Q the quaternion to multiply this by.
	* @return The result of multiplication (this * Q).
	*/
	FORCEINLINE YQuat operator*=(const YQuat& Q);

	/**
	* Rotate a vector by this quaternion.
	*
	* @param V the vector to be rotated
	* @return vector after rotation
	* @see RotateVector
	*/
	YVector operator*(const YVector& V) const;

	/**
	* Multiply this by a matrix.
	* This matrix conversion came from
	* http://www.m-hikari.com/ija/ija-password-2008/ija-password17-20-2008/aristidouIJA17-20-2008.pdf
	* used for non-uniform scaling transform.
	*
	* @param M Matrix to multiply by.
	* @return Matrix result after multiplication.
	*/
	YMatrix operator*(const YMatrix& M) const;

	/**
	* Multiply this quaternion by a scaling factor.
	*
	* @param Scale The scaling factor.
	* @return a reference to this after scaling.
	*/
	FORCEINLINE YQuat operator*=(const float Scale);

	/**
	* Get the result of scaling this quaternion.
	*
	* @param Scale The scaling factor.
	* @return The result of scaling.
	*/
	FORCEINLINE YQuat operator*(const float Scale) const;

	/**
	* Divide this quaternion by scale.
	*
	* @param Scale What to divide by.
	* @return a reference to this after scaling.
	*/
	FORCEINLINE YQuat operator/=(const float Scale);

	/**
	* Divide this quaternion by scale.
	*
	* @param Scale What to divide by.
	* @return new Quaternion of this after division by scale.
	*/
	FORCEINLINE YQuat operator/(const float Scale) const;

	/**
	* Checks whether two quaternions are identical.
	* This is an exact comparison per-component;see Equals() for a comparison
	* that allows for a small error tolerance and flipped axes of rotation.
	*
	* @param Q The other quaternion.
	* @return true if two quaternion are identical, otherwise false.
	* @see Equals
	*/
	bool operator==(const YQuat& Q) const;

	/**
	* Checks whether two quaternions are not identical.
	*
	* @param Q The other quaternion.
	* @return true if two quaternion are not identical, otherwise false.
	*/
	bool operator!=(const YQuat& Q) const;

	/**
	* Calculates dot product of two quaternions.
	*
	* @param Q The other quaternions.
	* @return The dot product.
	*/
	float operator|(const YQuat& Q) const;

public:

	/**
	* Convert a vector of floating-point Euler angles (in degrees) into a Quaternion.
	*
	* @param Euler the Euler angles
	* @return constructed YQuat
	*/
	static CORE_API YQuat MakeFromEuler(const YVector& Euler);

	/** Convert a Quaternion into floating-point Euler angles (in degrees). */
	CORE_API YVector Euler() const;

	/**
	* Normalize this quaternion if it is large enough.
	* If it is too small, returns an identity quaternion.
	*
	* @param Tolerance Minimum squared length of quaternion for normalization.
	*/
	FORCEINLINE void Normalize(float Tolerance = SMALL_NUMBER);

	/**
	* Get a normalized copy of this quaternion.
	* If it is too small, returns an identity quaternion.
	*
	* @param Tolerance Minimum squared length of quaternion for normalization.
	*/
	FORCEINLINE YQuat GetNormalized(float Tolerance = SMALL_NUMBER) const;

	// Return true if this quaternion is normalized
	bool IsNormalized() const;

	/**
	* Get the length of this quaternion.
	*
	* @return The length of this quaternion.
	*/
	FORCEINLINE float Size() const;

	/**
	* Get the length squared of this quaternion.
	*
	* @return The length of this quaternion.
	*/
	FORCEINLINE float SizeSquared() const;

	/**
	* get the axis and angle of rotation of this quaternion
	*
	* @param Axis{out] vector of axis of the quaternion
	* @param Angle{out] angle of the quaternion
	* @warning : assumes normalized quaternions.
	*/
	void ToAxisAndAngle(YVector& Axis, float& Angle) const;

	/**
	* Get the swing and twist decomposition for a specified axis
	*
	* @param InTwistAxis Axis to use for decomposition
	* @param OutSwing swing component quaternion
	* @param OutTwist Twist component quaternion
	* @warning assumes normalised quaternion and twist axis
	*/
	CORE_API void ToSwingTwist(const YVector& InTwistAxis, YQuat& OutSwing, YQuat& OutTwist) const;

	/**
	* Rotate a vector by this quaternion.
	*
	* @param V the vector to be rotated
	* @return vector after rotation
	*/
	YVector RotateVector(YVector V) const;

	/**
	* Rotate a vector by the inverse of this quaternion.
	*
	* @param V the vector to be rotated
	* @return vector after rotation by the inverse of this quaternion.
	*/
	YVector UnrotateVector(YVector V) const;

	/**
	* @return quaternion with W=0 and V=theta*v.
	*/
	CORE_API YQuat Log() const;

	/**
	* @note Exp should really only be used after Log.
	* Assumes a quaternion with W=0 and V=theta*v (where |v| = 1).
	* Exp(q) = (sin(theta)*v, cos(theta))
	*/
	CORE_API YQuat Exp() const;

	/**
	* @return inverse of this quaternion
	*/
	FORCEINLINE YQuat Inverse() const;

	/**
	* Enforce that the delta between this Quaternion and another one represents
	* the shortest possible rotation angle
	*/
	void EnforceShortestArcWith(const YQuat& OtherQuat);

	/** Get the forward direction (X axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetAxisX() const;

	/** Get the right direction (Y axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetAxisY() const;

	/** Get the up direction (Z axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetAxisZ() const;

	/** Get the forward direction (X axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetForwardVector() const;

	/** Get the right direction (Y axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetRightVector() const;

	/** Get the up direction (Z axis) after it has been rotated by this Quaternion. */
	FORCEINLINE YVector GetUpVector() const;

	/** Convert a rotation into a unit vector facing in its direction. Equivalent to GetForwardVector(). */
	FORCEINLINE YVector Vector() const;

	/** Get the YRotator representation of this Quaternion. */
	CORE_API YRotator Rotator() const;

	/**
	* Get the axis of rotation of the Quaternion.
	* This is the axis around which rotation occurs to transform the canonical coordinate system to the target orientation.
	* For the identity Quaternion which has no such rotation, YVector(1,0,0) is returned.
	*/
	FORCEINLINE YVector GetRotationAxis() const;

	/** Find the angular distance between two rotation quaternions (in radians) */
	FORCEINLINE float AngularDistance(const YQuat& Q) const;

	/**
	* Serializes the vector compressed for e.g. network transmission.
	* @param Ar Archive to serialize to/ from.
	* @return false to allow the ordinary struct code to run (this never happens).
	*/
	CORE_API bool NetSerialize(YArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	/**
	* Utility to check if there are any non-finite values (NaN or Inf) in this Quat.
	*
	* @return true if there are any non-finite values in this Quaternion, otherwise false.
	*/
	bool ContainsNaN() const;

	/**
	* Get a textual representation of the vector.
	*
	* @return Text describing the vector.
	*/
	YString ToString() const;

public:

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("YQuat contains NaN: %s"), *ToString());
			*const_cast<YQuat*>(this) = YQuat::Identity;
		}
	}

	FORCEINLINE void DiagnosticCheckNaN(const TCHAR* Message) const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("%s: YQuat contains NaN: %s"), Message, *ToString());
			*const_cast<YQuat*>(this) = YQuat::Identity;
		}
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const {}
	FORCEINLINE void DiagnosticCheckNaN(const TCHAR* Message) const {}
#endif

public:

	/**
	* Generates the 'smallest' (geodesic) rotation between two vectors of arbitrary length.
	*/
	static FORCEINLINE YQuat FindBetween(const YVector& Vector1, const YVector& Vector2)
	{
		return FindBetweenVectors(Vector1, Vector2);
	}

	/**
	* Generates the 'smallest' (geodesic) rotation between two normals (assumed to be unit length).
	*/
	static CORE_API YQuat FindBetweenNormals(const YVector& Normal1, const YVector& Normal2);

	/**
	* Generates the 'smallest' (geodesic) rotation between two vectors of arbitrary length.
	*/
	static CORE_API YQuat FindBetweenVectors(const YVector& Vector1, const YVector& Vector2);

	/**
	* Error measure (angle) between two quaternions, ranged [0..1].
	* Returns the hypersphere-angle between two quaternions; alignment shouldn't matter, though
	* @note normalized input is expected.
	*/
	static FORCEINLINE float Error(const YQuat& Q1, const YQuat& Q2);

	/**
	* YQuat::Error with auto-normalization.
	*/
	static FORCEINLINE float ErrorAutoNormalize(const YQuat& A, const YQuat& B);

	/**
	* Fast Linear Quaternion Interpolation.
	* Result is NOT normalized.
	*/
	static FORCEINLINE YQuat FastLerp(const YQuat& A, const YQuat& B, const float Alpha);

	/**
	* Bi-Linear Quaternion interpolation.
	* Result is NOT normalized.
	*/
	static FORCEINLINE YQuat FastBilerp(const YQuat& P00, const YQuat& P10, const YQuat& P01, const YQuat& P11, float FracX, float FracY);


	/** Spherical interpolation. Will correct alignment. Result is NOT normalized. */
	static CORE_API YQuat Slerp_NotNormalized(const YQuat &Quat1, const YQuat &Quat2, float Slerp);

	/** Spherical interpolation. Will correct alignment. Result is normalized. */
	static FORCEINLINE YQuat Slerp(const YQuat &Quat1, const YQuat &Quat2, float Slerp)
	{
		return Slerp_NotNormalized(Quat1, Quat2, Slerp).GetNormalized();
	}

	/**
	* Simpler Slerp that doesn't do any checks for 'shortest distance' etc.
	* We need this for the cubic interpolation stuff so that the multiple Slerps dont go in different directions.
	* Result is NOT normalized.
	*/
	static CORE_API YQuat SlerpFullPath_NotNormalized(const YQuat &quat1, const YQuat &quat2, float Alpha);

	/**
	* Simpler Slerp that doesn't do any checks for 'shortest distance' etc.
	* We need this for the cubic interpolation stuff so that the multiple Slerps dont go in different directions.
	* Result is normalized.
	*/
	static FORCEINLINE YQuat SlerpFullPath(const YQuat &quat1, const YQuat &quat2, float Alpha)
	{
		return SlerpFullPath_NotNormalized(quat1, quat2, Alpha).GetNormalized();
	}

	/**
	* Given start and end quaternions of quat1 and quat2, and tangents at those points tang1 and tang2, calculate the point at Alpha (between 0 and 1) between them. Result is normalized.
	* This will correct alignment by ensuring that the shortest path is taken.
	*/
	static CORE_API YQuat Squad(const YQuat& quat1, const YQuat& tang1, const YQuat& quat2, const YQuat& tang2, float Alpha);

	/**
	* Simpler Squad that doesn't do any checks for 'shortest distance' etc.
	* Given start and end quaternions of quat1 and quat2, and tangents at those points tang1 and tang2, calculate the point at Alpha (between 0 and 1) between them. Result is normalized.
	*/
	static CORE_API YQuat SquadFullPath(const YQuat& quat1, const YQuat& tang1, const YQuat& quat2, const YQuat& tang2, float Alpha);

	/**
	* Calculate tangents between given points
	*
	* @param PrevP quaternion at P-1
	* @param P quaternion to return the tangent
	* @param NextP quaternion P+1
	* @param Tension @todo document
	* @param OutTan Out control point
	*/
	static CORE_API void CalcTangents(const YQuat& PrevP, const YQuat& P, const YQuat& NextP, float Tension, YQuat& OutTan);

public:

	/**
	* Serializes the quaternion.
	*
	* @param Ar Reference to the serialization archive.
	* @param F Reference to the quaternion being serialized.
	* @return Reference to the Archive after serialization.
	*/
	friend YArchive& operator<<(YArchive& Ar, YQuat& F)
	{
		return Ar << F.X << F.Y << F.Z << F.W;
	}

	bool Serialize(YArchive& Ar)
	{
		Ar << *this;
		return true;
	}

} GCC_ALIGN(16);


/* YQuat inline functions
*****************************************************************************/

inline YQuat::YQuat(const YMatrix& M)
{
	// If Matrix is NULL, return Identity quaternion. If any of them is 0, you won't be able to construct rotation
	// if you have two plane at least, we can reconstruct the frame using cross product, but that's a bit expensive op to do here
	// for now, if you convert to matrix from 0 scale and convert back, you'll lose rotation. Don't do that. 
	if (M.GetScaledAxis(EAxis::X).IsNearlyZero() || M.GetScaledAxis(EAxis::Y).IsNearlyZero() || M.GetScaledAxis(EAxis::Z).IsNearlyZero())
	{
		*this = YQuat::Identity;
		return;
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// Make sure the Rotation part of the Matrix is unit length.
	// Changed to this (same as RemoveScaling) from RotDeterminant as using two different ways of checking unit length matrix caused inconsistency. 
	if (!ensure((YMath::Abs(1.f - M.GetScaledAxis(EAxis::X).SizeSquared()) <= KINDA_SMALL_NUMBER) && (YMath::Abs(1.f - M.GetScaledAxis(EAxis::Y).SizeSquared()) <= KINDA_SMALL_NUMBER) && (YMath::Abs(1.f - M.GetScaledAxis(EAxis::Z).SizeSquared()) <= KINDA_SMALL_NUMBER)))
	{
		*this = YQuat::Identity;
		return;
	}
#endif

	//const MeReal *const t = (MeReal *) tm;
	float	s;

	// Check diagonal (trace)
	const float tr = M.M[0][0] + M.M[1][1] + M.M[2][2];

	if (tr > 0.0f)
	{
		float InvS = YMath::InvSqrt(tr + 1.f);
		this->W = 0.5f * (1.f / InvS);
		s = 0.5f * InvS;

		this->X = (M.M[1][2] - M.M[2][1]) * s;
		this->Y = (M.M[2][0] - M.M[0][2]) * s;
		this->Z = (M.M[0][1] - M.M[1][0]) * s;
	}
	else
	{
		// diagonal is negative
		int32 i = 0;

		if (M.M[1][1] > M.M[0][0])
			i = 1;

		if (M.M[2][2] > M.M[i][i])
			i = 2;

		static const int32 nxt[3] = { 1, 2, 0 };
		const int32 j = nxt[i];
		const int32 k = nxt[j];

		s = M.M[i][i] - M.M[j][j] - M.M[k][k] + 1.0f;

		float InvS = YMath::InvSqrt(s);

		float qt[4];
		qt[i] = 0.5f * (1.f / InvS);

		s = 0.5f * InvS;

		qt[3] = (M.M[j][k] - M.M[k][j]) * s;
		qt[j] = (M.M[i][j] + M.M[j][i]) * s;
		qt[k] = (M.M[i][k] + M.M[k][i]) * s;

		this->X = qt[0];
		this->Y = qt[1];
		this->Z = qt[2];
		this->W = qt[3];

		DiagnosticCheckNaN();
	}
}


FORCEINLINE YQuat::YQuat(const YRotator& R)
{
	*this = R.Quaternion();
	DiagnosticCheckNaN();
}


FORCEINLINE YVector YQuat::operator*(const YVector& V) const
{
	return RotateVector(V);
}


inline YMatrix YQuat::operator*(const YMatrix& M) const
{
	YMatrix Result;
	YQuat VT, VR;
	YQuat Inv = Inverse();
	for (int32 I = 0; I<4; ++I)
	{
		YQuat VQ(M.M[I][0], M.M[I][1], M.M[I][2], M.M[I][3]);
		VectorQuaternionMultiply(&VT, this, &VQ);
		VectorQuaternionMultiply(&VR, &VT, &Inv);
		Result.M[I][0] = VR.X;
		Result.M[I][1] = VR.Y;
		Result.M[I][2] = VR.Z;
		Result.M[I][3] = VR.W;
	}

	return Result;
}


/* YQuat inline functions
*****************************************************************************/

FORCEINLINE YQuat::YQuat(EForceInit ZeroOrNot)
	: X(0), Y(0), Z(0), W(ZeroOrNot == ForceInitToZero ? 0.0f : 1.0f)
{ }


FORCEINLINE YQuat::YQuat(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{
	DiagnosticCheckNaN();
}


FORCEINLINE YQuat::YQuat(const YQuat& Q)
	: X(Q.X)
	, Y(Q.Y)
	, Z(Q.Z)
	, W(Q.W)
{ }


FORCEINLINE YString YQuat::ToString() const
{
	return YString::Printf(TEXT("X=%.9f Y=%.9f Z=%.9f W=%.9f"), X, Y, Z, W);
}


#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
FORCEINLINE YQuat& YQuat::operator=(const YQuat& Other)
{
	this->X = Other.X;
	this->Y = Other.Y;
	this->Z = Other.Z;
	this->W = Other.W;

	return *this;
}
#endif


FORCEINLINE YQuat::YQuat(YVector Axis, float AngleRad)
{
	const float half_a = 0.5f * AngleRad;
	float s, c;
	YMath::SinCos(&s, &c, half_a);

	X = s * Axis.X;
	Y = s * Axis.Y;
	Z = s * Axis.Z;
	W = c;

	DiagnosticCheckNaN();
}


FORCEINLINE YQuat YQuat::operator+(const YQuat& Q) const
{
	return YQuat(X + Q.X, Y + Q.Y, Z + Q.Z, W + Q.W);
}


FORCEINLINE YQuat YQuat::operator+=(const YQuat& Q)
{
	this->X += Q.X;
	this->Y += Q.Y;
	this->Z += Q.Z;
	this->W += Q.W;

	DiagnosticCheckNaN();

	return *this;
}


FORCEINLINE YQuat YQuat::operator-(const YQuat& Q) const
{
	return YQuat(X - Q.X, Y - Q.Y, Z - Q.Z, W - Q.W);
}


FORCEINLINE bool YQuat::Equals(const YQuat& Q, float Tolerance) const
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister ToleranceV = VectorLoadFloat1(&Tolerance);
	const VectorRegister A = VectorLoadAligned(this);
	const VectorRegister B = VectorLoadAligned(&Q);

	const VectorRegister RotationSub = VectorAbs(VectorSubtract(A, B));
	const VectorRegister RotationAdd = VectorAbs(VectorAdd(A, B));
	return !VectorAnyGreaterThan(RotationSub, ToleranceV) || !VectorAnyGreaterThan(RotationAdd, ToleranceV);
#else
	return (YMath::Abs(X - Q.X) <= Tolerance && YMath::Abs(Y - Q.Y) <= Tolerance && YMath::Abs(Z - Q.Z) <= Tolerance && YMath::Abs(W - Q.W) <= Tolerance)
		|| (YMath::Abs(X + Q.X) <= Tolerance && YMath::Abs(Y + Q.Y) <= Tolerance && YMath::Abs(Z + Q.Z) <= Tolerance && YMath::Abs(W + Q.W) <= Tolerance);
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
}

FORCEINLINE bool YQuat::IsIdentity(float Tolerance) const
{
	return Equals(YQuat::Identity, Tolerance);
}

FORCEINLINE YQuat YQuat::operator-=(const YQuat& Q)
{
	this->X -= Q.X;
	this->Y -= Q.Y;
	this->Z -= Q.Z;
	this->W -= Q.W;

	DiagnosticCheckNaN();

	return *this;
}


FORCEINLINE YQuat YQuat::operator*(const YQuat& Q) const
{
	YQuat Result;
	VectorQuaternionMultiply(&Result, this, &Q);

	Result.DiagnosticCheckNaN();

	return Result;
}


FORCEINLINE YQuat YQuat::operator*=(const YQuat& Q)
{
	/**
	* Now this uses VectorQuaternionMultiply that is optimized per platform.
	*/
	VectorRegister A = VectorLoadAligned(this);
	VectorRegister B = VectorLoadAligned(&Q);
	VectorRegister Result;
	VectorQuaternionMultiply(&Result, &A, &B);
	VectorStoreAligned(Result, this);

	DiagnosticCheckNaN();

	return *this;
}


FORCEINLINE YQuat YQuat::operator*=(const float Scale)
{
	X *= Scale;
	Y *= Scale;
	Z *= Scale;
	W *= Scale;

	DiagnosticCheckNaN();

	return *this;
}


FORCEINLINE YQuat YQuat::operator*(const float Scale) const
{
	return YQuat(Scale * X, Scale * Y, Scale * Z, Scale * W);
}


FORCEINLINE YQuat YQuat::operator/=(const float Scale)
{
	const float Recip = 1.0f / Scale;
	X *= Recip;
	Y *= Recip;
	Z *= Recip;
	W *= Recip;

	DiagnosticCheckNaN();

	return *this;
}


FORCEINLINE YQuat YQuat::operator/(const float Scale) const
{
	const float Recip = 1.0f / Scale;
	return YQuat(X * Recip, Y * Recip, Z * Recip, W * Recip);
}


FORCEINLINE bool YQuat::operator==(const YQuat& Q) const
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister A = VectorLoadAligned(this);
	const VectorRegister B = VectorLoadAligned(&Q);
	return VectorMaskBits(VectorCompareEQ(A, B)) == 0x0F;
#else
	return X == Q.X && Y == Q.Y && Z == Q.Z && W == Q.W;
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
}


FORCEINLINE bool YQuat::operator!=(const YQuat& Q) const
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister A = VectorLoadAligned(this);
	const VectorRegister B = VectorLoadAligned(&Q);
	return VectorMaskBits(VectorCompareNE(A, B)) != 0x00;
#else
	return X != Q.X || Y != Q.Y || Z != Q.Z || W != Q.W;
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
}


FORCEINLINE float YQuat::operator|(const YQuat& Q) const
{
	return X * Q.X + Y * Q.Y + Z * Q.Z + W * Q.W;
}


FORCEINLINE void YQuat::Normalize(float Tolerance)
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const VectorRegister Vector = VectorLoadAligned(this);

	const VectorRegister SquareSum = VectorDot4(Vector, Vector);
	const VectorRegister NonZeroMask = VectorCompareGE(SquareSum, VectorLoadFloat1(&Tolerance));
	const VectorRegister InvLength = VectorReciprocalSqrtAccurate(SquareSum);
	const VectorRegister NormalizedVector = VectorMultiply(InvLength, Vector);
	VectorRegister Result = VectorSelect(NonZeroMask, NormalizedVector, GlobalVectorConstants::Float0001);

	VectorStoreAligned(Result, this);
#else
	const float SquareSum = X * X + Y * Y + Z * Z + W * W;

	if (SquareSum >= Tolerance)
	{
		const float Scale = YMath::InvSqrt(SquareSum);

		X *= Scale;
		Y *= Scale;
		Z *= Scale;
		W *= Scale;
	}
	else
	{
		*this = YQuat::Identity;
	}
#endif // PLATFORM_ENABLE_VECTORINTRINSICS
}


FORCEINLINE YQuat YQuat::GetNormalized(float Tolerance) const
{
	YQuat Result(*this);
	Result.Normalize(Tolerance);
	return Result;
}



FORCEINLINE bool YQuat::IsNormalized() const
{
	return (YMath::Abs(1.f - SizeSquared()) < THRESH_QUAT_NORMALIZED);
}


FORCEINLINE float YQuat::Size() const
{
	return YMath::Sqrt(X * X + Y * Y + Z * Z + W * W);
}


FORCEINLINE float YQuat::SizeSquared() const
{
	return (X * X + Y * Y + Z * Z + W * W);
}


FORCEINLINE void YQuat::ToAxisAndAngle(YVector& Axis, float& Angle) const
{
	Angle = 2.f * YMath::Acos(W);
	Axis = GetRotationAxis();
}

FORCEINLINE YVector YQuat::GetRotationAxis() const
{
	// Ensure we never try to sqrt a neg number
	const float S = YMath::Sqrt(YMath::Max(1.f - (W * W), 0.f));

	if (S >= 0.0001f)
	{
		return YVector(X / S, Y / S, Z / S);
	}

	return YVector(1.f, 0.f, 0.f);
}

float YQuat::AngularDistance(const YQuat& Q) const
{
	float InnerProd = X*Q.X + Y*Q.Y + Z*Q.Z + W*Q.W;
	return YMath::Acos((2 * InnerProd * InnerProd) - 1.f);
}


FORCEINLINE YVector YQuat::RotateVector(YVector V) const
{
#if WITH_DIRECTXMATH
	YVector Result;
	VectorQuaternionVector3Rotate(&Result, &V, this);
	return Result;

#else

	// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
	// V' = V + 2w(Q x V) + (2Q x (Q x V))
	// refactor:
	// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
	// T = 2(Q x V);
	// V' = V + w*(T) + (Q x T)

	const YVector Q(X, Y, Z);
	const YVector T = 2.f * YVector::CrossProduct(Q, V);
	const YVector Result = V + (W * T) + YVector::CrossProduct(Q, T);
	return Result;
#endif
}

FORCEINLINE YVector YQuat::UnrotateVector(YVector V) const
{
#if WITH_DIRECTXMATH
	YVector Result;
	VectorQuaternionVector3InverseRotate(&Result, &V, this);
	return Result;
#else
	//return Inverse().RotateVector(V);

	const YVector Q(-X, -Y, -Z); // Inverse
	const YVector T = 2.f * YVector::CrossProduct(Q, V);
	const YVector Result = V + (W * T) + YVector::CrossProduct(Q, T);
	return Result;
#endif
}


FORCEINLINE YQuat YQuat::Inverse() const
{
	checkSlow(IsNormalized());

	return YQuat(-X, -Y, -Z, W);
}


FORCEINLINE void YQuat::EnforceShortestArcWith(const YQuat& OtherQuat)
{
	const float DotResult = (OtherQuat | *this);
	const float Bias = YMath::FloatSelect(DotResult, 1.0f, -1.0f);

	X *= Bias;
	Y *= Bias;
	Z *= Bias;
	W *= Bias;
}


FORCEINLINE YVector YQuat::GetAxisX() const
{
	return RotateVector(YVector(1.f, 0.f, 0.f));
}


FORCEINLINE YVector YQuat::GetAxisY() const
{
	return RotateVector(YVector(0.f, 1.f, 0.f));
}


FORCEINLINE YVector YQuat::GetAxisZ() const
{
	return RotateVector(YVector(0.f, 0.f, 1.f));
}


FORCEINLINE YVector YQuat::GetForwardVector() const
{
	return GetAxisX();
}

FORCEINLINE YVector YQuat::GetRightVector() const
{
	return GetAxisY();
}

FORCEINLINE YVector YQuat::GetUpVector() const
{
	return GetAxisZ();
}

FORCEINLINE YVector YQuat::Vector() const
{
	return GetAxisX();
}


FORCEINLINE float YQuat::Error(const YQuat& Q1, const YQuat& Q2)
{
	const float cosom = YMath::Abs(Q1.X * Q2.X + Q1.Y * Q2.Y + Q1.Z * Q2.Z + Q1.W * Q2.W);
	return (YMath::Abs(cosom) < 0.9999999f) ? YMath::Acos(cosom)*(1.f / PI) : 0.0f;
}


FORCEINLINE float YQuat::ErrorAutoNormalize(const YQuat& A, const YQuat& B)
{
	YQuat Q1 = A;
	Q1.Normalize();

	YQuat Q2 = B;
	Q2.Normalize();

	return YQuat::Error(Q1, Q2);
}


FORCEINLINE YQuat YQuat::FastLerp(const YQuat& A, const YQuat& B, const float Alpha)
{
	// To ensure the 'shortest route', we make sure the dot product between the both rotations is positive.
	const float DotResult = (A | B);
	const float Bias = YMath::FloatSelect(DotResult, 1.0f, -1.0f);
	return (B * Alpha) + (A * (Bias * (1.f - Alpha)));
}


FORCEINLINE YQuat YQuat::FastBilerp(const YQuat& P00, const YQuat& P10, const YQuat& P01, const YQuat& P11, float FracX, float FracY)
{
	return YQuat::FastLerp(
		YQuat::FastLerp(P00, P10, FracX),
		YQuat::FastLerp(P01, P11, FracX),
		FracY
	);
}


FORCEINLINE bool YQuat::ContainsNaN() const
{
	return (!YMath::IsFinite(X) ||
		!YMath::IsFinite(Y) ||
		!YMath::IsFinite(Z) ||
		!YMath::IsFinite(W)
		);
}


template<> struct TIsPODType<YQuat> { enum { Value = true }; };

/* YMath inline functions
*****************************************************************************/

template<class U>
FORCEINLINE_DEBUGGABLE YQuat YMath::Lerp(const YQuat& A, const YQuat& B, const U& Alpha)
{
	return YQuat::Slerp(A, B, Alpha);
}

template<class U>
FORCEINLINE_DEBUGGABLE YQuat YMath::BiLerp(const YQuat& P00, const YQuat& P10, const YQuat& P01, const YQuat& P11, float FracX, float FracY)
{
	YQuat Result;

	Result = Lerp(
		YQuat::Slerp_NotNormalized(P00, P10, FracX),
		YQuat::Slerp_NotNormalized(P01, P11, FracX),
		FracY
	);

	return Result;
}

template<class U>
FORCEINLINE_DEBUGGABLE YQuat YMath::CubicInterp(const YQuat& P0, const YQuat& T0, const YQuat& P1, const YQuat& T1, const U& A)
{
	return YQuat::Squad(P0, T0, P1, T1, A);
}

