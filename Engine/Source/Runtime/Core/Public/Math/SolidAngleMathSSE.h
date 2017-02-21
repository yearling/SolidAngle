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


