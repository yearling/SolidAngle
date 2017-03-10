#pragma once
// 以后使用这个包裹后的Windows头文件来替代<Windows.h>
#include "CoreTypes.h"
#include "HAL/PlatformMemory.h"
#include "PreWindowsApi.h"
#ifndef STRICT
#define STRICT
#endif
#include "MinWindows.h"
#include "PostWindowsApi.h"
