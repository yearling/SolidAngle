#include "Core.h"
#include <iostream>

TEnableIf<true, int>::Type  foo()
{
	std::cout << "TEnableIf : true" << std::endl;
	return 5;
}
int main()
{
	foo();
	return 0;
}
