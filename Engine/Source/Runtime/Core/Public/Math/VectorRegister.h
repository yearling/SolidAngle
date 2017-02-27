#pragma once
#if WITH_DIRECTXMATH
#define SIMD_ALIGMENT (16)
#elif PLATFORM_ENABLE_VECTORINTRINSICS
#define SIMD_ALIGMENT (16)
#include "SolidAngleMathSSE.h"
#elif PLATFORM_ENABLE_VECTORINTRINSICS_NEON
#define SIME_ALIGNMENT (16)
#include "SolidAngleMathNeon"
#else
#define SIME_ALIGNMENT (4)
#endif

#include "SolidAngleMathVectorCommon.h"

extern CORE_API  const VectorRegister VECTOR_INV_255;

// Below this weight threshold, animations won't be blended in.
#define ZERO_ANIMWEIGHT_THRESH (0.00001f)

namespace GlobalVectorConstants
{
	static const VectorRegister AnimWeightThreshold = MakeVectorRegister(ZERO_ANIMWEIGHT_THRESH, ZERO_ANIMWEIGHT_THRESH, ZERO_ANIMWEIGHT_THRESH, ZERO_ANIMWEIGHT_THRESH);
	static const VectorRegister RotationSignificantThreshold = MakeVectorRegister(1.0f - DELTA*DELTA, 1.0f - DELTA*DELTA, 1.0f - DELTA*DELTA, 1.0f - DELTA*DELTA);
}
