// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"

class YOutputDevice;
class FOutputDeviceConsole;
class FOutputDeviceError;
class YFeedbackContext;

struct CORE_API FWindowsPlatformOutputDevices
	: public FGenericPlatformOutputDevices
{
	static YOutputDevice*			GetEventLog();
	static FOutputDeviceConsole*	GetLogConsole();
	static FOutputDeviceError*		GetError();
	static YFeedbackContext*		GetWarn();
};


typedef FWindowsPlatformOutputDevices FPlatformOutputDevices;
