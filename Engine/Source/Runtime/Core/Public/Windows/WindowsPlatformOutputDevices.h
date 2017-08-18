// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"

class FOutputDevice;
class YOutputDeviceConsole;
class YOutputDeviceError;
class FFeedbackContext;

struct CORE_API YWindowsPlatformOutputDevices
	: public YGenericPlatformOutputDevices
{
	static FOutputDevice*			GetEventLog();
	static YOutputDeviceConsole*	GetLogConsole();
	static YOutputDeviceError*		GetError();
	static FFeedbackContext*		GetWarn();
};


typedef YWindowsPlatformOutputDevices YPlatformOutputDevices;
