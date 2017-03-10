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
int main()
{
	static_assert(TAnd<>::Value,"should be true");
	static_assert(TAnd<TAnd<TRUEValue,TRUEValue>,TRUEValue>::Value,"should be true");
	static_assert(YAnd<>::Value,"should be true");
	//static_assert(!YAnd<FALSEValue>::Value,"should be true");
	return 0;
}
