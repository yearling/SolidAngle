
//#include <HAL/PlatformIncludes.h>
#include <iostream>
//#include <intrin.h>
#include <stdio.h>

#pragma intrinsic(_mm_cvtt_ss2si)

//int main()
//{
//	__m128 a;
//	int b;
//
//	float af[4] = { 101.25, 200.75,300.5, 400.5 };
//
//	// Load a with the floating point values.
//	// The values will be copied to the XMM registers.
//	a = _mm_loadu_ps(af);
//
//	// Extract the first element of a
//	b = _mm_cvtt_ss2si(a);
//
//	printf_s("%d\n", b);
//}
//int main()
//{
//	double c = cosf(10);
//}

#define  Trunc(F) _mm_cvtt_ss2si(_mm_set_ss(F))

//FORCEINLINE VectorRegister VectorCeil(const VectorRegister& X)
//{
//	VectorRegister Trunc = VectorTruncate(X);
//	VectorRegister PosMask = VectorCompareGE(X, GlobalVectorConstants::FloatZero);
//	VectorRegister Add = VectorSelect(PosMask, GlobalVectorConstants::FloatOne, (GlobalVectorConstants::FloatZero));
//	return VectorAdd(Trunc, Add);
//}
float FooCeil(float f)
{
	float fTrunc = Trunc(f);
	float add = 0.0f;
	if (f >= 0.0f)
	{
		add = 1.0f;
	}
	else
	{
		add = 0.0f;
	}
	return fTrunc + add;
}
int main()
{
	std::cout << FooCeil(-1.9f) << std::endl;
	std::cout << FooCeil(-0.9f) << std::endl;
	std::cout << FooCeil(0.5f) << std::endl;
	std::cout << FooCeil(1.5f) << std::endl;
	return 0;
}
