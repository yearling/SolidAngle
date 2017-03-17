// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/Matrix.h"
#include "Math/RotationMatrix.h"
#include "Math/TranslationMatrix.h"
#include "Math/Quat.h"
#include "Math/ScaleMatrix.h"
#include "Math/TransformCalculus.h"


//////////////////////////////////////////////////////////////////////////
// Transform calculus for 3D types. Since UE4 already has existing 3D transform
// types, this is mostly a set of adapter overloads for the primitive operations
// requires by the transform calculus framework.
//
// The following types are adapted.
// * float           -> represents a uniform scale.
// * FScale          -> represents a 3D non-uniform scale.
// * YVector         -> represents a 3D translation.
// * FRotator        -> represents a pure rotation.
// * FQuat           -> represents a pure rotation.
// * FMatrix         -> represents a general 3D homogeneous transform.
//
//////////////////////////////////////////////////////////////////////////

/**
 * Represents a 3D non-uniform scale (to disambiguate from an YVector, which is used for translation).
 * 
 * Serves as a good base example of how to write a class that supports the basic transform calculus
 * operations.
 */
class FScale
{
public:
	/** Ctor. initialize to an identity scale, 1.0. */
	FScale() :Scale(1.0f) {}
	/** Ctor. initialize from a uniform scale. */
	explicit FScale(float InScale) :Scale(InScale) {}
	/** Ctor. initialize from an YVector defining the 3D scale. */
	explicit FScale(const YVector& InScale) :Scale(InScale) {}
	/** Access to the underlying YVector that stores the scale. */
	const YVector& GetVector() const { return Scale; }
	/** Concatenate two scales. */
	const FScale Concatenate(const FScale& RHS) const
	{
		return FScale(Scale * RHS.GetVector());
	}
	/** Invert the scale. */
	const FScale Inverse() const
	{
		return FScale(YVector(1.0f / Scale.X, 1.0f / Scale.Y, 1.0f / Scale.Z));
	}
private:
	/** Underlying storage of the 3D scale. */
	YVector Scale;
};

/** Specialization for converting a FMatrix to an FRotator. It uses a non-standard explicit conversion function. */
template<> template<> inline YRotator TransformConverter<YRotator>::Convert<YMatrix>(const YMatrix& Transform)
{
	return Transform.Rotator();
}

//////////////////////////////////////////////////////////////////////////
// FMatrix Support
//////////////////////////////////////////////////////////////////////////

/**
 * Converts a generic transform to a matrix using a ToMatrix() member function.
 * Uses decltype to allow some classes to return const-ref types for efficiency.
 * 
 * @param Transform
 * @return the FMatrix stored by the Transform.
 */
template<typename TransformType>
inline auto ToMatrix(const TransformType& Transform) -> decltype(Transform.ToMatrix())
{
	return Transform.ToMatrix();
}

/**
 * Specialization for the NULL Matrix conversion.
 * 
 * @param Scale Uniform Scale
 * @return Matrix that represents the uniform Scale space.
 */
inline const YMatrix& ToMatrix(const YMatrix& Transform)
{
	return Transform;
}

/**
 * Specialization for floats as a uniform scale.
 * 
 * @param Scale Uniform Scale
 * @return Matrix that represents the uniform Scale space.
 */
inline YMatrix ToMatrix(float Scale)
{
	return YScaleMatrix(Scale);
}

/**
 * Specialization for non-uniform Scale.
 * 
 * @param Scale Non-uniform Scale
 * @return Matrix that represents the non-uniform Scale space.
 */
inline YMatrix ToMatrix(const FScale& Scale)
{
	return YScaleMatrix(Scale.GetVector());
}

/**
 * Specialization for translation.
 * 
 * @param Translation Translation
 * @return Matrix that represents the translated space.
 */
inline YMatrix ToMatrix(const YVector& Translation)
{
	return YTranslationMatrix(Translation);
}

/**
 * Specialization for rotation.
 * 
 * @param Rotation Rotation
 * @return Matrix that represents the rotated space.
 */
inline YMatrix ToMatrix(const YRotator& Rotation)
{
	return YRotationMatrix(Rotation);
}

/**
 * Specialization for rotation.
 * 
 * @param Rotation Rotation
 * @return Matrix that represents the rotated space.
 */
inline YMatrix ToMatrix(const YQuat& Rotation)
{
	return YRotationMatrix::Make(Rotation);
}

/**
 * Specialization of TransformConverter for FMatrix. Calls ToMatrix() by default.
 * Allows custom types to easily provide support via a ToMatrix() overload or a ToMatrix() member function.
 * Uses decltype to support efficient passthrough of classes that can convert to a FMatrix without creating
 * a new instance.
 */
template<>
struct TransformConverter<YMatrix>
{
	template<typename OtherTransformType>
	static auto Convert(const OtherTransformType& Transform) -> decltype(ToMatrix(Transform))
	{
		return ToMatrix(Transform);
	}
};

/** concatenation rules for basic UE4 types. */
template<> struct ConcatenateRules<float        , FScale       > { typedef FScale ResultType; };
template<> struct ConcatenateRules<FScale       , float        > { typedef FScale ResultType; };
template<> struct ConcatenateRules<float        , YVector      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YVector      , float        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<float        , YRotator     > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YRotator     , float        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<float        , YQuat        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YQuat        , float        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<float        , YMatrix      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YMatrix      , float        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<FScale       , YVector      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YVector      , FScale       > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<FScale       , YRotator     > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YRotator     , FScale       > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<FScale       , YQuat        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YQuat        , FScale       > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<FScale       , YMatrix      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YMatrix      , FScale       > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YVector      , YRotator     > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YRotator     , YVector      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YVector      , YQuat        > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YQuat        , YVector      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YVector      , YMatrix      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YMatrix      , YVector      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YRotator     , YQuat        > { typedef YQuat ResultType; };
template<> struct ConcatenateRules<YQuat        , YRotator     > { typedef YQuat ResultType; };
template<> struct ConcatenateRules<YRotator     , YMatrix      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YMatrix      , YRotator     > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YQuat        , YMatrix      > { typedef YMatrix ResultType; };
template<> struct ConcatenateRules<YMatrix      , YQuat        > { typedef YMatrix ResultType; };

//////////////////////////////////////////////////////////////////////////
// Concatenate overloads. 
// 
// Since these are existing UE4 types, we cannot rely on the default
// template that calls member functions. Instead, we provide direct overloads.
//////////////////////////////////////////////////////////////////////////

/**
 * Specialization for concatenating two Matrices.
 * 
 * @param LHS rotation that goes from space A to space B
 * @param RHS rotation that goes from space B to space C.
 * @return a new rotation representing the transformation from the input space of LHS to the output space of RHS.
 */
inline YMatrix Concatenate(const YMatrix& LHS, const YMatrix& RHS)
{
	return LHS * RHS;
}

/**
 * Specialization for concatenating two rotations.
 *
 * NOTE: FQuat concatenates right to left, opposite of how FMatrix implements it.
 *       Confusing, no? That's why we have these high level functions!
 * 
 * @param LHS rotation that goes from space A to space B
 * @param RHS rotation that goes from space B to space C.
 * @return a new rotation representing the transformation from the input space of LHS to the output space of RHS.
 */
inline YQuat Concatenate(const YQuat& LHS, const YQuat& RHS)
{
	return RHS * LHS;
}

/**
 * Specialization for concatenating two rotations.
 *
 * @param LHS rotation that goes from space A to space B
 * @param RHS rotation that goes from space B to space C.
 * @return a new rotation representing the transformation from the input space of LHS to the output space of RHS.
 */
inline YRotator Concatenate(const YRotator& LHS, const YRotator& RHS)
{
	//@todo implement a more efficient way to do this.
	return TransformCast<YRotator>(Concatenate(TransformCast<YMatrix>(LHS), TransformCast<YMatrix>(RHS)));
}

/**
 * Specialization for concatenating two translations.
 * 
 * @param LHS Translation that goes from space A to space B
 * @param RHS Translation that goes from space B to space C.
 * @return a new Translation representing the transformation from the input space of LHS to the output space of RHS.
 */
inline YVector Concatenate(const YVector& LHS, const YVector& RHS)
{
	return LHS + RHS;
}

//////////////////////////////////////////////////////////////////////////
// Inverse overloads. 
// 
// Since these are existing UE4 types, we cannot rely on the default
// template that calls member functions. Instead, we provide direct overloads.
//////////////////////////////////////////////////////////////////////////

/**
 * Inverts a transform from space A to space B so it transforms from space B to space A.
 * Specialization for FMatrix.
 * 
 * @param Transform Input transform from space A to space B.
 * @return Inverted transform from space B to space A.
 */
inline YMatrix Inverse(const YMatrix& Transform)
{
	return Transform.Inverse();
}

/**
 * Inverts a transform from space A to space B so it transforms from space B to space A.
 * Specialization for FRotator.
 * 
 * @param Transform Input transform from space A to space B.
 * @return Inverted transform from space B to space A.
 */
inline YRotator Inverse(const YRotator& Transform)
{
	YVector EulerAngles = Transform.Euler();
	return YRotator::MakeFromEuler(YVector(-EulerAngles.Z, -EulerAngles.Y, -EulerAngles.X));
}

/**
 * Inverts a transform from space A to space B so it transforms from space B to space A.
 * Specialization for FQuat.
 * 
 * @param Transform Input transform from space A to space B.
 * @return Inverted transform from space B to space A.
 */
inline YQuat Inverse(const YQuat& Transform)
{
	return Transform.Inverse();
}

/**
 * Inverts a transform from space A to space B so it transforms from space B to space A.
 * Specialization for translation.
 * 
 * @param Transform Input transform from space A to space B.
 * @return Inverted transform from space B to space A.
 */
inline YVector Inverse(const YVector& Transform)
{
	return -Transform;
}

//////////////////////////////////////////////////////////////////////////
// TransformPoint overloads. 
// 
// Since these are existing UE4 types, we cannot rely on the default
// template that calls member functions. Instead, we provide direct overloads.
//////////////////////////////////////////////////////////////////////////

/**
 * Specialization for FMatrix as it's member function is called something slightly different.
 */
inline YVector TransformPoint(const YMatrix& Transform, const YVector& Point)
{
	return Transform.TransformPosition(Point);
}

/**
 * Specialization for FQuat as it's member function is called something slightly different.
 */
inline YVector TransformPoint(const YQuat& Transform, const YVector& Point)
{
	return Transform.RotateVector(Point);
}

/**
 * Specialization for FQuat as it's member function is called something slightly different.
 */
inline YVector TransformVector(const YQuat& Transform, const YVector& Vector)
{
	return Transform.RotateVector(Vector);
}

/**
 * Specialization for FRotator as it's member function is called something slightly different.
 */
inline YVector TransformPoint(const YRotator& Transform, const YVector& Point)
{
	return Transform.RotateVector(Point);
}

/**
 * Specialization for FRotator as it's member function is called something slightly different.
 */
inline YVector TransformVector(const YRotator& Transform, const YVector& Vector)
{
	return Transform.RotateVector(Vector);
}

/**
 * Specialization for YVector Translation.
 */
inline YVector TransformPoint(const YVector& Transform, const YVector& Point)
{
	return Transform + Point;
}

/**
 * Specialization for YVector Translation (does nothing).
 */
inline const YVector& TransformVector(const YVector& Transform, const YVector& Vector)
{
	return Vector;
}

/**
 * Specialization for Scale.
 */
inline YVector TransformPoint(const FScale& Transform, const YVector& Point)
{
	return Transform.GetVector() * Point;
}

/**
 * Specialization for Scale.
 */
inline YVector TransformVector(const FScale& Transform, const YVector& Vector)
{
	return Transform.GetVector() * Vector;
}

