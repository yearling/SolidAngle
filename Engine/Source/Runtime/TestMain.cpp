
#include "Math/SolidAngleMath.h"
#include<iostream>
int main()
{
	std::cout << sizeof(void*) << std::endl;
	std::cout << YGenericPlatformMath::FloorLog2(2) << std::endl;
	std::cout << YGenericPlatformMath::FloorLog2(10) << std::endl;
	std::cout << YGenericPlatformMath::CeilLogTwo(16) << std::endl;
}
