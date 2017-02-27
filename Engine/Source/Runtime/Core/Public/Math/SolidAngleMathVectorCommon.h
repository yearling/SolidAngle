#pragma once

FORCEINLINE bool VectorIsAligned(const void* Ptr)
{
	return !(PTRINT(Ptr) &(SIMD_ALIGMENT - 1));
}

// Returns a normalized 4 vector = Vector / |Vector|.
// There is no handling of zero length vectors, use VectorNormalizeSafe if this is a possible input.
FORCEINLINE VectorRegister VectorNormalizeAccurate(const VectorRegister& Vector)
{
	const VectorRegister SquareSum = VectorDot4(Vector, Vector);
	const VectorRegister InvLength = VectorReciprocalSqrtAccurate(SquareSum);
	const VectorRegister NormalizedVector = VectorMultiply(InvLength, Vector);
	return NormalizedVector;
}

// Returns ((Vector dot Vector) >= 1e-8) ? (Vector / |Vector|) : DefaultValue
// Uses accurate 1/sqrt, not the estimate
FORCEINLINE VectorRegister VectorNormalizeSafe(const VectorRegister& Vector, const VectorRegister& DefaultValue)
{
	const VectorRegister SquareSum = VectorDot4(Vector, Vector);
	const VectorRegister NonZeroMask = VectorCompareGE(SquareSum, GlobalVectorConstants::SmallLengthThreshold);
	const VectorRegister InvLength = VectorReciprocalSqrtAccurate(SquareSum);
	const VectorRegister NormalizedVector = VectorMultiply(InvLength, Vector);
	return VectorSelect(NonZeroMask, NormalizedVector, DefaultValue);
}
