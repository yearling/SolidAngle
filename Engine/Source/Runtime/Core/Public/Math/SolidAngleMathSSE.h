#pragma once

#include <emmintrin.h>

// float4 vector register type, where the first float(X) is stored in the lowest 32 bits, and so on

typedef __m128					VectorRegister;
typedef __m128i					VectorRegisterInt;
typedef __m128d					VectorRegisterDouble;

// for an __m128, we need a single set of braces (for clang)
#define DECLARE_VECTOR_REGISTER(X, Y, Z, W) { X, Y, Z, W }

// A0	Selects which element(0 - 3) from 'A' into 1st slot in the result
// A1	Selects which element(0 - 3) from 'A' into 2nd slot in the result
// B2	Selects which element(0 - 3) from 'B' into 3rd slot in the result
// B3	Selects which element(0 - 3) from 'B' into 4th slot in the result
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )

// Returns a bitwise equivalent vector based on 4 DWORDs.
// X		1st uint32 component
// Y		2nd uint32 component
// Z		3rd uint32 component
// W		4th uint32 component
// return	Bitwise equivalent vector with 4 floats
FORCEINLINE VectorRegister		MakeVectorRegister(uint32 X, uint32 Y, uint32 Z, uint32 W)
{
	union { VectorRegister v; VectorRegisterInt i; } Tmp;
	Tmp.i = _mm_setr_epi32(X, Y, Z, W);
	return Tmp.v;
}

// Returns a vector based on 4 FLOATs.
// X		1st float component
// Y		2nd float component
// Z		3rd float component
// W		4th float component
// return	Vector of the 4 FLOATs
FORCEINLINE VectorRegister		MakeVectorRegister(float X, float Y, float Z, float W)
{
	// !!Note by zyx, 这里是带r的版本，反着从低位向高位填。不带r的__mm_set_ps是从高位向低位填
	return _mm_setr_ps(X, Y, Z, W);
}

// Returns a vector based on 4 int32.
// X		1st int32 component
// Y		2nd int32 component
// Z		3rd int32 component
// W		4th int32 component
// return	Vector of the 4 int32
FORCEINLINE VectorRegisterInt		MakeVectorRegisterInt(int32 X, int32 Y, int32 Z, int32 W)
{
	return _mm_setr_epi32(X, Y, Z, W);
}

// Constants:
#include "SolidAngleVectorConstant.h"

//Instinsics

// Return a vector with all zeros
#define VectorZero()			_mm_setzero_ps()

// Return a vector with all ones
#define VectorOne()				(GlobalVectorConstants::FloatOne)

// Return an component from a vector
// Vec:							VectorRegister
// ComponentIndex:				Which component to gex, X=0, Y=1, Z=2, W=3
// Return:						The component as a float
FORCEINLINE float				VectorGetComponent(VectorRegister Vec, uint32 ComponentIndex)
{
	return ((float*)&(Vec))[ComponentIndex];
}

// Load 4 FLOATs from unaligned memory
// Ptr:							Unaligned memory pointer to the 4 FLOATs
// Return:						VectorRegister(Ptr[0],Ptr[1],Ptr[2],Ptr[3])
#define VectorLoad( Ptr )		_mm_loadu_ps( (float*)(Ptr))

// Load 3 FLOATs from unaligned memory and leaves W undefined.
// Ptr:							Unaligned memory pointer to the 3 FLOATs
// Return:						VectorRegister(Ptr[0],Ptr[1],Ptr[2],undefined)
#define VectorLoadFloat3( Ptr ) MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )

// Load 3 FLOATs from unaligned memory and sets W = 0.0f
// Ptr:							Unaligned memory pointer to the 3 FLOATs
// Return:						VectorRegister(Ptr[0],Ptr[1],Ptr[2], 0.0f)
#define VectorLoadFloat3_W0( Ptr ) MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )

// Load 3 FLOATs from unaligned memory and sets W = 1.0f
// Ptr:							Unaligned memory pointer to the 3 FLOATs
// Return:						VectorRegister(Ptr[0],Ptr[1],Ptr[2], 1.0f)
#define VectorLoadFloat3_W1( Ptr ) MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 1.0f )

// Load 4 FLOATs from aligned memory
// Ptr:							Aligned memory pointer to the 4 FLOATs
// Return:						VectorRegister(Ptr[0],Ptr[1],Ptr[2], Ptr[3])
#define VectorLoadAligned( Ptr )  _mm_load_ps((float*)Ptr)

// Loads 1 float from unaligned memory and replicates it to all 4 elements.
//Ptr:							Unaligned memory pointer to the float
//Return:						VectorRegister(Ptr[0], Ptr[0], Ptr[0], Ptr[0])
#define VectorLoadFloat1( Ptr ) _mm_load_ps1((float*)(Ptr))

// Creates a vector out of three FLOATs and leaves W undefined.
// X:							1st float component
// Y:							2nd float component
// Z:							3rd float component
// Return:						VectorRegister(X, Y, Z, undefined)
#define VectorSetFloat3( X, Y, Z )		MakeVectorRegister( X, Y, Z, 0.0f)

// Propagates passed in float to all registers
// F:							Float to Set
// Result:						VectorRegister(F,F,F,F)
#define VectorSetFloat1( F )	_mm_set_ps1( F )

// Creates a vector out of four FLOATs.
// X:							1st float component
// Y:							2nd float component
// Z:							3rd float component
// W:							4th float component
// return:						VectorRegister(X, Y, Z, W)
#define VectorSet( X, Y, Z, W )			MakeVectorRegister( X, Y, Z, W )

// Stores a vector to aligned memory.
// Vec:							Vector to store
// Ptr:							Aligned memory pointer
#define VectorStoreAligned( Vec, Ptr )	_mm_store_ps( (float*)(Ptr), Vec) 

//Performs non-temporal store of a vector to aligned memory without polluting the caches
// !!Note by zyx,没看明白Poluuting the caches是什么意思
// Vec:							Vector to store
// Ptr:							Aligned memory pointer
#define VectorStoreAlignedStreamed( Vec, Ptr )	_mm_stream_ps( (float*)(Ptr), Vec )

// Stores a vector to memory(aligned or unaligned).
// Vec:							Vector to store
// Ptr:							Memory pointer
#define VectorStore( Vec, Ptr )			_mm_storeu_ps( (float*)(Ptr), Vec )

// Stores the XYZ components of a vector to unaligned memory.
// Vec:							Vector to store XYZ
// Ptr:							Unaligned memory pointer
FORCEINLINE void				VectorStoreFloat3(const VectorRegister& Vec, void* Ptr)
{
#if 0   // !!Note by zyx, Implement by UE
	union { VectorRegister v; float f[4]; } Tmp;
	Tmp.v = Vec;
	float* FloatPtr = (float*)(Ptr);
	FloatPtr[0] = Tmp.f[0];
	FloatPtr[1] = Tmp.f[1];
	FloatPtr[2] = Tmp.f[2];
#else
	float* FloatPtr = (float*)(Ptr);
	FloatPtr[0] = ((const float*)&(Vec))[0];
	FloatPtr[1] = ((const float*)&(Vec))[1];
	FloatPtr[2] = ((const float*)&(Vec))[2];
#endif
}

// Stores the X component of a vector to unaligned memory.
// Vec:							Vector to store X
// Ptr:							Unaligned memory pointer
#define VectorStoreFloat1( Vec, Ptr )	_mm_store_ss((float*)(Ptr), Vec)

// Replicates one element into all four elements and returns the new vector.
// Vec:							Source Vector
// ElementIndex:				Index(0-3） of the element to replicate
// Return:						VectorRegister( Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex] )
// !!Note by zyx, 实现原理如下
//__m128 _mm_shuffle_ps(__m128 a, __m128 b, unsigned int imm8)
//{
//	SELECT4(src, control) { //下面会调用，就是根据control（2bit,4种值，对应__m128的每个float)
//		CASE(control[1:0])
//			0:	tmp[31:0] : = src[31:0]
//			1 : tmp[31:0] : = src[63:32]
//			2 : tmp[31:0] : = src[95:64]
//			3 : tmp[31:0] : = src[127:96]
//			ESAC
//			RETURN tmp[31:0]
//	}
//
//		dst[31:0] : = SELECT4(a[127:0], imm8[1:0])
//		dst[63:32] : = SELECT4(a[127:0], imm8[3:2])
//		dst[95:64] : = SELECT4(b[127:0], imm8[5:4])
//		dst[127:96] : = SELECT4(b[127:0], imm8[7:6])
//}
#define VectorReplicate( Vec, ElementIndex )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(ElementIndex,ElementIndex,ElementIndex,ElementIndex) )

// Returns the absolute value (component-wise).
// Vec:							Source vector
// return:						VectorRegister( abs(Vec.x), abs(Vec.y), abs(Vec.z), abs(Vec.w) )
// // !!Note by zyx, 实现原理:浮点最高位为符号位，与0x7FFFFFFF作与操作
#define VectorAbs( Vec )		_mm_and_ps(Vec, GlobalVectorConstants::SignMask)

// Returns the negated value (component-wise).
// Vec:							Source Vector
// Return:						VectorRegister( -Vec.x, -Vec.y, -Vec.z, -Vec.w )
#define VectorNegate( Vec )		_mm_sub_ps(_mm_setzero_ps(),Vec)

// Adds two vectors(component - wise) and returns the result.
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(Vec1.x + Vec2.x, Vec1.y + Vec2.y, Vec1.z + Vec2.z, Vec1.w + Vec2.w)
#define VectorAdd( Vec1, Vec2 )			_mm_add_ps( Vec1, Vec2 )

// Subtracts a vector from another (component-wise) and returns the result.
// Vec1:						1st vector
// Vec2:						2nd vector
// Return						VectorRegister( Vec1.x-Vec2.x, Vec1.y-Vec2.y, Vec1.z-Vec2.z, Vec1.w-Vec2.w )
#define VectorSubtract( Vec1, Vec2 )	_mm_sub_ps( Vec1, Vec2 )

// Multiplies two vectors (component-wise) and returns the result.
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister( Vec1.x*Vec2.x, Vec1.y*Vec2.y, Vec1.z*Vec2.z, Vec1.w*Vec2.w )
#define VectorMultiply( Vec1, Vec2 )	_mm_mul_ps( Vec1, Vec2 )

// Multiplies two vectors(component - wise), adds in the third vector and returns the result.
// Vec1:						1st vector
// Vec2:						2nd vector
// Vec3:						3rd vector
// Return:						VectorRegister(Vec1.x*Vec2.x + Vec3.x, Vec1.y*Vec2.y + Vec3.y, Vec1.z*Vec2.z + Vec3.z, Vec1.w*Vec2.w + Vec3.w)
#define VectorMultiplyAdd( Vec1, Vec2, Vec3 )	_mm_add_ps(_mm_mul_ps( Vec1, Vec2), Vec3)

// Calculates the dot3 product of two vectors and returns a vector with the result in all 4 components.
//  Only really efficient on Xbox 360
// Vec1:						1st vector
// Vec2:						2nd vector
// return:						d = dot3(Vec1.xyz, Vec2.xyz), VectorRegister(d, d, d, d)
FORCEINLINE VectorRegister		VectorDot3(const VectorRegister& Vec1, const VectorRegister& Vec2)
{
	VectorRegister Temp = VectorMultiply(Vec1, Vec2);
	return VectorAdd(VectorReplicate(Temp, 0), VectorAdd(VectorReplicate(Temp, 1), VectorReplicate(Temp, 2)));
}

// Calculates the dot4 product of two vectors and returns a vector with the result in all 4 components.
//  Only really efficient on Xbox 360
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						d = dot4(Vec1.xyzw, Vec2.xyzw), VectorRegister(d, d, d, d)
FORCEINLINE VectorRegister VectorDot4(const VectorRegister& Vec1, const VectorRegister& Vec2)
{
	VectorRegister Temp1, Temp2;
	Temp1 = VectorMultiply(Vec1, Vec2);
	Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(2, 3, 0, 1));	// (Z,W,X,Y).
	Temp1 = VectorAdd(Temp1, Temp2);								// (X*X + Z*Z, Y*Y + W*W, Z*Z + X*X, W*W + Y*Y)
	Temp2 = _mm_shuffle_ps(Temp1, Temp1, SHUFFLEMASK(1, 2, 3, 0));	// Rotate left 4 bytes (Y,Z,W,X).
	return VectorAdd(Temp1, Temp2);								// (X*X + Z*Z + Y*Y + W*W, Y*Y + W*W + Z*Z + X*X, Z*Z + X*X + W*W + Y*Y, W*W + Y*Y + X*X + Z*Z)
}

// Creates a four - part mask based on component - wise == compares of the input vectors
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(Vec1.x == Vec2.x ? 0xFFFFFFFF : 0, same for yzw)
#define VectorCompareEQ( Vec1, Vec2 )			_mm_cmpeq_ps( Vec1, Vec2 )

// Creates a four - part mask based on component - wise != compares of the input vectors
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(Vec1.x != Vec2.x ? 0xFFFFFFFF : 0, same for yzw)
#define VectorCompareNE( Vec1, Vec2 )			_mm_cmpneq_ps( Vec1, Vec2 )

// Creates a four - part mask based on component - wise > compares of the input vectors
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(Vec1.x > Vec2.x ? 0xFFFFFFFF : 0, same for yzw)
#define VectorCompareGT( Vec1, Vec2 )			_mm_cmpgt_ps( Vec1, Vec2 )

// Creates a four - part mask based on component - wise >= compares of the input vectors
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(Vec1.x >= Vec2.x ? 0xFFFFFFFF : 0, same for yzw)
#define VectorCompareGE( Vec1, Vec2 )			_mm_cmpge_ps( Vec1, Vec2 )

// Does a bitwise vector selection based on a mask(e.g., created from VectorCompareXX)
// Mask:						Mask(when 1: use the corresponding bit from Vec1 otherwise from Vec2)
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(for each bit i : Mask[i] ? Vec1[i] : Vec2[i])
FORCEINLINE VectorRegister VectorSelect(const VectorRegister& Mask, const VectorRegister& Vec1, const VectorRegister& Vec2)
{
	return _mm_xor_ps(Vec2, _mm_and_ps(Mask, _mm_xor_ps(Vec1, Vec2)));
}

// Combines two vectors using bitwise OR(treating each vector as a 128 bit field)
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(for each bit i : Vec1[i] | Vec2[i])
#define VectorBitwiseOr(Vec1, Vec2)  _mm_or_ps(Vec1, Vec2)

// Combines two vectors using bitwise AND (treating each vector as a 128 bit field)
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(for each bit i : Vec1[i] & Vec2[i])
#define VectorBitwiseAnd(Vec1, Vec2)  _mm_and_ps(Vec1, Vec2)

// Combines two vectors using bitwise XOR (treating each vector as a 128 bit field)
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						VectorRegister(for each bit i : Vec1[i] ^ Vec2[i])
#define VectorBitwiseXor(Vec1, Vec2)  _mm_xor_ps(Vec1, Vec2)

// Calculates the cross product of two vectors(XYZ components).W is set to 0.
// Vec1:						1st vector
// Vec2:						2nd vector
// Return:						cross(Vec1.xyz, Vec2.xyz).W is set to 0.
FORCEINLINE VectorRegister VectorCross(const VectorRegister& Vec1, const VectorRegister& Vec2)
{
	VectorRegister A_YZXW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(1, 2, 0, 3));
	VectorRegister B_ZXYW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(2, 0, 1, 3));
	VectorRegister A_ZXYW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(2, 0, 1, 3));
	VectorRegister B_YZXW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(1, 2, 0, 3));
	return VectorSubtract(VectorMultiply(A_YZXW, B_ZXYW), VectorMultiply(A_ZXYW, B_YZXW));
}

// Calculates x raised to the power of y(component - wise).
// Base:						Base vector
// Exponent:					Exponent vector
// Return:						VectorRegister(Base.x^Exponent.x, Base.y^Exponent.y, Base.z^Exponent.z, Base.w^Exponent.w)
FORCEINLINE VectorRegister VectorPow(const VectorRegister& Base, const VectorRegister& Exponent)
{
	//@TODO: Optimize
	union { VectorRegister v; float f[4]; } B, E;
	B.v = Base;
	E.v = Exponent;
	return _mm_setr_ps(powf(B.f[0], E.f[0]), powf(B.f[1], E.f[1]), powf(B.f[2], E.f[2]), powf(B.f[3], E.f[3]));
}

// Returns an estimate of 1/sqrt(c) for each component of the vector
// Vector:						Vector
// Return:						VectorRegister(1/sqrt(t), 1/sqrt(t), 1/sqrt(t), 1/sqrt(t))
#define VectorReciprocalSqrt(Vec)		_mm_rsqrt_ps( Vec )

/**
* Computes an estimate of the reciprocal of a vector (component-wise) and returns the result.
*
* @param Vec	1st vector
* @return		VectorRegister( (Estimate) 1.0f / Vec.x, (Estimate) 1.0f / Vec.y, (Estimate) 1.0f / Vec.z, (Estimate) 1.0f / Vec.w )
*/
#define VectorReciprocal(Vec)			_mm_rcp_ps(Vec)

// Return Reciprocal Length of the vector (estimate)
// Vector:						Vector
// Return:						VectorRegister(rlen, rlen, rlen, rlen) when rlen = 1/sqrt(dot4(V))
FORCEINLINE VectorRegister VectorReciprocalLen(const VectorRegister& Vector)
{
	VectorRegister RecipLen = VectorDot4(Vector, Vector);
	return VectorReciprocalSqrt(RecipLen);
}

// Return the reciprocal of the square root of each component
// Vector:						Vector
// Return:						VectorRegister(1 / sqrt(Vec.X), 1 / sqrt(Vec.Y), 1 / sqrt(Vec.Z), 1 / sqrt(Vec.W))
FORCEINLINE VectorRegister VectorReciprocalSqrtAccurate(const VectorRegister& Vec)
{
	// Perform two passes of Newton-Raphson iteration on the hardware estimate
	//    v^-0.5 = x
	// => x^2 = v^-1
	// => 1/(x^2) = v
	// => F(x) = x^-2 - v
	//    F'(x) = -2x^-3

	//    x1 = x0 - F(x0)/F'(x0)
	// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
	// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
	// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)

	const VectorRegister OneHalf = GlobalVectorConstants::FloatOneHalf;
	const VectorRegister VecDivBy2 = VectorMultiply(Vec, OneHalf);

	// Initial estimate
	const VectorRegister x0 = VectorReciprocalSqrt(Vec);

	// First iteration
	VectorRegister x1 = VectorMultiply(x0, x0);
	x1 = VectorSubtract(OneHalf, VectorMultiply(VecDivBy2, x1));
	x1 = VectorMultiplyAdd(x0, x1, x0);

	// Second iteration
	VectorRegister x2 = VectorMultiply(x1, x1);
	x2 = VectorSubtract(OneHalf, VectorMultiply(VecDivBy2, x2));
	x2 = VectorMultiplyAdd(x1, x2, x1);

	return x2;
}

// Computes the reciprocal of a vector(component - wise) and returns the result.
// Vec:							1st vector
// Return:						VectorRegister(1.0f / Vec.x, 1.0f / Vec.y, 1.0f / Vec.z, 1.0f / Vec.w)
FORCEINLINE VectorRegister VectorReciprocalAccurate(const VectorRegister& Vec)
{
	// Perform two passes of Newton-Raphson iteration on the hardware estimate
	//   x1 = x0 - f(x0) / f'(x0)
	//
	//    1 / Vec = x
	// => x * Vec = 1 
	// => F(x) = x * Vec - 1
	//    F'(x) = Vec
	// => x1 = x0 - (x0 * Vec - 1) / Vec
	//
	// Since 1/Vec is what we're trying to solve, use an estimate for it, x0
	// => x1 = x0 - (x0 * Vec - 1) * x0 = 2 * x0 - Vec * x0^2 

	// Initial estimate
	const VectorRegister x0 = VectorReciprocal(Vec);

	// First iteration
	const VectorRegister x0Squared = VectorMultiply(x0, x0);
	const VectorRegister x0Times2 = VectorAdd(x0, x0);
	const VectorRegister x1 = VectorSubtract(x0Times2, VectorMultiply(Vec, x0Squared));

	// Second iteration
	const VectorRegister x1Squared = VectorMultiply(x1, x1);
	const VectorRegister x1Times2 = VectorAdd(x1, x1);
	const VectorRegister x2 = VectorSubtract(x1Times2, VectorMultiply(Vec, x1Squared));

	return x2;
}

// Normalize vector
// Vector:						Vector to normalize
// Return:						Normalized VectorRegister
FORCEINLINE VectorRegister VectorNormalize(const VectorRegister& Vector)
{
	return VectorMultiply(Vector, VectorReciprocalLen(Vector));
}

// Loads XYZ and sets W = 0
// Vector:						VectorRegister
// Return:						VectorRegister(X, Y, Z, 0.0f)
#define VectorSet_W0( Vec )		_mm_and_ps( Vec, GlobalVectorConstants::XYZMask )

// Loads XYZ and sets W = 1
// Vector:						VectorRegister
// Return:						VectorRegister(X, Y, Z, 1.0f)
FORCEINLINE VectorRegister VectorSet_W1(const VectorRegister& Vector)
{
	// Temp = (Vector[2]. Vector[3], 1.0f, 1.0f)
	VectorRegister Temp = _mm_movehl_ps(VectorOne(), Vector);

	// Return (Vector[0], Vector[1], Vector[2], 1.0f)
	return _mm_shuffle_ps(Vector, Temp, SHUFFLEMASK(0, 1, 0, 3));
}

// Multiplies two 4x4 matrices.
// Result:						Pointer to where the result should be stored
// Matrix1:						Pointer to the first matrix
// Matrix2:						Pointer to the second matrix
FORCEINLINE void VectorMatrixMultiply(void *Result, const void* Matrix1, const void* Matrix2)
{
	const VectorRegister *A = (const VectorRegister *)Matrix1;
	const VectorRegister *B = (const VectorRegister *)Matrix2;
	VectorRegister *R = (VectorRegister *)Result;
	VectorRegister Temp, R0, R1, R2, R3;

	// First row of result (Matrix1[0] * Matrix2).
	Temp = VectorMultiply(VectorReplicate(A[0], 0), B[0]);
	Temp = VectorMultiplyAdd(VectorReplicate(A[0], 1), B[1], Temp);
	Temp = VectorMultiplyAdd(VectorReplicate(A[0], 2), B[2], Temp);
	R0 = VectorMultiplyAdd(VectorReplicate(A[0], 3), B[3], Temp);

	// Second row of result (Matrix1[1] * Matrix2).
	Temp = VectorMultiply(VectorReplicate(A[1], 0), B[0]);
	Temp = VectorMultiplyAdd(VectorReplicate(A[1], 1), B[1], Temp);
	Temp = VectorMultiplyAdd(VectorReplicate(A[1], 2), B[2], Temp);
	R1 = VectorMultiplyAdd(VectorReplicate(A[1], 3), B[3], Temp);

	// Third row of result (Matrix1[2] * Matrix2).
	Temp = VectorMultiply(VectorReplicate(A[2], 0), B[0]);
	Temp = VectorMultiplyAdd(VectorReplicate(A[2], 1), B[1], Temp);
	Temp = VectorMultiplyAdd(VectorReplicate(A[2], 2), B[2], Temp);
	R2 = VectorMultiplyAdd(VectorReplicate(A[2], 3), B[3], Temp);

	// Fourth row of result (Matrix1[3] * Matrix2).
	Temp = VectorMultiply(VectorReplicate(A[3], 0), B[0]);
	Temp = VectorMultiplyAdd(VectorReplicate(A[3], 1), B[1], Temp);
	Temp = VectorMultiplyAdd(VectorReplicate(A[3], 2), B[2], Temp);
	R3 = VectorMultiplyAdd(VectorReplicate(A[3], 3), B[3], Temp);

	// Store result
	R[0] = R0;
	R[1] = R1;
	R[2] = R2;
	R[3] = R3;
}

// Calculate the inverse of an YMatrix.
// DstMatrix:					FMatrix pointer to where the result should be stored
// SrcMatrix:					FMatrix pointer to the Matrix to be inversed
// TODO : Vector version of this function that doesn't use D3DX
FORCEINLINE void VectorMatrixInverse(void* DstMatrix, const void* SrcMatrix)
{
	typedef float Float4x4[4][4];
	const Float4x4& M = *((const Float4x4*)SrcMatrix);
	Float4x4 Result;
	float Det[4];
	Float4x4 Tmp;

	Tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
	Tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
	Tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

	Tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
	Tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
	Tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

	Tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
	Tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
	Tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	Tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
	Tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
	Tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	Det[0] = M[1][1] * Tmp[0][0] - M[2][1] * Tmp[0][1] + M[3][1] * Tmp[0][2];
	Det[1] = M[0][1] * Tmp[1][0] - M[2][1] * Tmp[1][1] + M[3][1] * Tmp[1][2];
	Det[2] = M[0][1] * Tmp[2][0] - M[1][1] * Tmp[2][1] + M[3][1] * Tmp[2][2];
	Det[3] = M[0][1] * Tmp[3][0] - M[1][1] * Tmp[3][1] + M[2][1] * Tmp[3][2];

	float Determinant = M[0][0] * Det[0] - M[1][0] * Det[1] + M[2][0] * Det[2] - M[3][0] * Det[3];
	const float	RDet = 1.0f / Determinant;

	Result[0][0] = RDet * Det[0];
	Result[0][1] = -RDet * Det[1];
	Result[0][2] = RDet * Det[2];
	Result[0][3] = -RDet * Det[3];
	Result[1][0] = -RDet * (M[1][0] * Tmp[0][0] - M[2][0] * Tmp[0][1] + M[3][0] * Tmp[0][2]);
	Result[1][1] = RDet * (M[0][0] * Tmp[1][0] - M[2][0] * Tmp[1][1] + M[3][0] * Tmp[1][2]);
	Result[1][2] = -RDet * (M[0][0] * Tmp[2][0] - M[1][0] * Tmp[2][1] + M[3][0] * Tmp[2][2]);
	Result[1][3] = RDet * (M[0][0] * Tmp[3][0] - M[1][0] * Tmp[3][1] + M[2][0] * Tmp[3][2]);
	Result[2][0] = RDet * (
		M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
		M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
		M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
		);
	Result[2][1] = -RDet * (
		M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
		M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
		M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
		);
	Result[2][2] = RDet * (
		M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
		M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
		M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
		);
	Result[2][3] = -RDet * (
		M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
		);
	Result[3][0] = -RDet * (
		M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
		M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
		M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
		);
	Result[3][1] = RDet * (
		M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
		M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
		M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
		);
	Result[3][2] = -RDet * (
		M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
		M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
		M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
		);
	Result[3][3] = RDet * (
		M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
		);

	memcpy(DstMatrix, &Result, 16 * sizeof(float));
}