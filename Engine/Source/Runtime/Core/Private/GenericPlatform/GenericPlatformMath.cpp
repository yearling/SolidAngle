#include "CorePrivatePCH.h"

static int32 GSRandSeed;

CORE_API void YGenericPlatformMath::SRandInit(int32 Seed)
{
	GSRandSeed = Seed;
}

CORE_API int32 YGenericPlatformMath::GetRandSeed()
{
	return GSRandSeed;
}

CORE_API float YGenericPlatformMath::SRand()
{
	GSRandSeed = (GSRandSeed * 196314165) + 907633515;
	union { float f; int32 i; } Result;
	union { float f; int32 i; } Temp;
	const float SRandTemp = 1.0f;
	Temp.f = SRandTemp;
	Result.i = (Temp.i & 0xff800000) | (GSRandSeed & 0x007fffff);
	return YPlatformMath::Fractional(Result.f);
}

CORE_API float YGenericPlatformMath::Atan2(float Y, float X)
{
	//return atan2f(Y,X);
	// atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
	// We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
	// On PC this has been measured to be 2x faster than the std C version.
	const float absX = YMath::Abs(X);
	const float absY = YMath::Abs(Y);
	const bool yAbsBigger = (absY > absX);
	float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
	float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

	if (t0 == 0.f)
		return 0.f;

	float t3 = t1 / t0;
	float t4 = t3 * t3;

	static const float c[7] = {
		+7.2128853633444123e-03f,
		-3.5059680836411644e-02f,
		+8.1675882859940430e-02f,
		-1.3374657325451267e-01f,
		+1.9856563505717162e-01f,
		-3.3324998579202170e-01f,
		+1.0f
	};

	t0 = c[0];
	t0 = t0 * t4 + c[1];
	t0 = t0 * t4 + c[2];
	t0 = t0 * t4 + c[3];
	t0 = t0 * t4 + c[4];
	t0 = t0 * t4 + c[5];
	t0 = t0 * t4 + c[6];
	t3 = t0 * t3;

	t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
	t3 = (X < 0.0f) ? PI - t3 : t3;
	t3 = (Y < 0.0f) ? -t3 : t3;

	return t3;
}


CORE_API void YGenericPlatformMath::FmodReportError(float X, float Y)
{
	if (Y == 0)
	{
		//!!FIXME by zyx
	}
}

#if WITH_DEV_AUTOMATION_TESTS
extern float TheCompilerDoesntKnowThisIsAlwaysZero;

void YGenericPlatformMath::AutoTest()
{
	//!!FIXME by zyx,添加大整数测试，添加check的实现
	/*
	check(IsNaN(sqrtf(-1.0f)));
	check(!IsFinite(sqrtf(-1.0f)));
	check(!IsFinite(-1.0f / TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!IsFinite(1.0f / TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!IsNaN(-1.0f / TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!IsNaN(1.0f / TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!IsNaN(MAX_FLT));
	check(IsFinite(MAX_FLT));
	check(!IsNaN(0.0f));
	check(IsFinite(0.0f));
	check(!IsNaN(1.0f));
	check(IsFinite(1.0f));
	check(!IsNaN(-1.e37f));
	check(IsFinite(-1.e37f));
	check(FloorLog2(0) == 0);
	check(FloorLog2(1) == 0);
	check(FloorLog2(2) == 1);
	check(FloorLog2(12) == 3);
	check(FloorLog2(16) == 4);
	*/
}
#endif
