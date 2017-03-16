// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"

class YOutputDevice;
class YOutputDeviceConsole;
class YOutputDeviceError;
class YFeedbackContext;

struct CORE_API FWindowsPlatformOutputDevices
	: public FGenericPlatformOutputDevices
{
	static YOutputDevice*			GetEventLog();
	static YOutputDeviceConsole*	GetLogConsole();
	static YOutputDeviceError*		GetError();
	static YFeedbackContext*		GetWarn();
};


typedef FWindowsPlatformOutputDevices FPlatformOutputDevices;
