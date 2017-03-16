#pragma once

#include "CoreTypes.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/Matrix.h"
#include "Math/RotationTranslationMatrix.h"

/** Rotation matrix no translation */
class FRotationMatrix
	: public FRotationTranslationMatrix
{
public:

	/**
	* Constructor.
	*
	* @param Rot rotation
	*/
	FRotationMatrix(const YRotator& Rot)
		: FRotationTranslationMatrix(Rot, YVector::ZeroVector)
	{ }

	/** Matrix factory. Return an YMatrix so we don't have type conversion issues in expressions. */
	static YMatrix Make(YRotator const& Rot)
	{
		return FRotationMatrix(Rot);
	}

	/** Matrix factory. Return an YMatrix so we don't have type conversion issues in expressions. */
	static CORE_API YMatrix Make(YQuat const& Rot);

	/** Builds a rotation matrix given only a XAxis. Y and Z are unspecified but will be orthonormal. XAxis need not be normalized. */
	static CORE_API YMatrix MakeFromX(YVector const& XAxis);

	/** Builds a rotation matrix given only a YAxis. X and Z are unspecified but will be orthonormal. YAxis need not be normalized. */
	static CORE_API YMatrix MakeFromY(YVector const& YAxis);

	/** Builds a rotation matrix given only a ZAxis. X and Y are unspecified but will be orthonormal. ZAxis need not be normalized. */
	static CORE_API YMatrix MakeFromZ(YVector const& ZAxis);

	/** Builds a matrix with given X and Y axes. X will remain fixed, Y may be changed minimally to enforce orthogonality. Z will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromXY(YVector const& XAxis, YVector const& YAxis);

	/** Builds a matrix with given X and Z axes. X will remain fixed, Z may be changed minimally to enforce orthogonality. Y will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromXZ(YVector const& XAxis, YVector const& ZAxis);

	/** Builds a matrix with given Y and X axes. Y will remain fixed, X may be changed minimally to enforce orthogonality. Z will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromYX(YVector const& YAxis, YVector const& XAxis);

	/** Builds a matrix with given Y and Z axes. Y will remain fixed, Z may be changed minimally to enforce orthogonality. X will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromYZ(YVector const& YAxis, YVector const& ZAxis);

	/** Builds a matrix with given Z and X axes. Z will remain fixed, X may be changed minimally to enforce orthogonality. Y will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromZX(YVector const& ZAxis, YVector const& XAxis);

	/** Builds a matrix with given Z and Y axes. Z will remain fixed, Y may be changed minimally to enforce orthogonality. X will be computed. Inputs need not be normalized. */
	static CORE_API YMatrix MakeFromZY(YVector const& ZAxis, YVector const& YAxis);
};
