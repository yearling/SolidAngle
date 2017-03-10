#include "Core.h"
#include <iostream>

struct TRUEValue
{
	enum 
	{
		Value = true
	};
};

struct FALSEValue
{
	enum 
	{
		Value = false
	};
};

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
	static_assert(TAnd<>::Value,"should be true");
	static_assert(TAnd<TAnd<TRUEValue,TRUEValue>,TRUEValue>::Value,"should be true");
	static_assert(YAnd<>::Value,"should be true");
	//static_assert(!YAnd<FALSEValue>::Value,"should be true");
	std::cout << Fib<46>::Value << std::endl;
	std::cout << Mul<5,4>::Value << std::endl;
	static_assert(TAnd<TrueValue, TrueValue, TrueValue>::Value, "should be true");
	//static_assert(TAnd<>::Value, "should be true");
	return 0;
}
