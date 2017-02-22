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
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) 

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
#define VectorMultiplyAdd( Vec1, Vec2, Vec3 ) _mm_add_ps(_mm_mul_ps( Vec1, Vec2), Vec3 40)
