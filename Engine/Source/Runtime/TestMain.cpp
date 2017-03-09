#include "Core.h"
#include <iostream>

TEnableIf<true, int>::Type  foo()
{
	std::cout << "TEnableIf : true" << std::endl;
	return 5;
}

template< int64 N>
struct Fib
{
	enum { Value = Fib<N-2>::Value +Fib<N-1>::Value};
};

template<>
struct Fib<0>
{
	enum { Value = 0 };
};

template<>
struct Fib<1>
{
	enum { Value = 1 };
};

template<int M, int N>
struct Mul
{
	enum { Value = Mul<M,N-1>::Value + M};
};

template<int M>
struct Mul<M, 1>
{
	enum {
		Value = M
	};
};

struct TrueValue
{
	enum { Value = true};
};
struct FalseValue
{
	enum { Value = true };
};
int main()
{
	foo();
	std::cout << Fib<46>::Value << std::endl;
	std::cout << Mul<5,4>::Value << std::endl;
	static_assert(TAnd<TrueValue, TrueValue, TrueValue>::Value, "should be true");
	//static_assert(TAnd<>::Value, "should be true");
	return 0;
}
