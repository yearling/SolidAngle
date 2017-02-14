#pragma once
// 以后使用这个包裹后的Windows头文件来替代<Windows.h>
#include "Windows/PreWindowsApi.h"
#ifndef STRICT
#define STRICT
#endif
#include "Windows/MinWindows.h"
#include "Windows/PostWindowsApi.h"