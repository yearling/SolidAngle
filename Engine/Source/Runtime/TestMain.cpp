
#include "Math/SolidAngleMath.h"
#include<iostream>
#include <intrin.h>
#include <stdio.h>

//#pragma intrinsic(_mm_cvtt_ss2si)

int main()
{
	__m128 a;
	int b;

	float af[4] = { 101.25, 200.75,300.5, 400.5 };

	// Load a with the floating point values.
	// The values will be copied to the XMM registers.
	a = _mm_loadu_ps(af);

	// Extract the first element of a
	b = _mm_cvtt_ss2si(a);

	printf_s("%d\n", b);
}
//int main()
//{
//	std::cout << sizeof(void*) << std::endl;
//	std::cout << YGenericPlatformMath::FloorLog2(2) << std::endl;
//	std::cout << YGenericPlatformMath::FloorLog2(10) << std::endl;
//	std::cout << YGenericPlatformMath::CeilLogTwo(16) << std::endl;
//}
