#pragma once

#if !defined(PLATFORM_WINDOWS)
#define PLATFORM_WINDOWS 0
#endif

#include "GenericPlatform/GenericPlatform.h"

#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatform.h"
#else
	#error Unknown Compiler
#endif


