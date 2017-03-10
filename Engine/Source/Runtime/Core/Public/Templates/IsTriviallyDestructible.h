#pragma once
#include "CoreTypes.h"

/**
* Traits class which tests if a type has a trivial destructor.
*/
template <typename T>
struct TIsTriviallyDestructible
{
	enum { Value = __has_trivial_destructor(T) };
};
