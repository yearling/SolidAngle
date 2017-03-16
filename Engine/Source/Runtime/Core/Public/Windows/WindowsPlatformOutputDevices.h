// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"

class YOutputDevice;
class YOutputDeviceConsole;
class YOutputDeviceError;
class YFeedbackContext;

struct CORE_API YWindowsPlatformOutputDevices
	: public YGenericPlatformOutputDevices
{
	static YOutputDevice*			GetEventLog();
	static YOutputDeviceConsole*	GetLogConsole();
	static YOutputDeviceError*		GetError();
	static YFeedbackContext*		GetWarn();
};


typedef YWindowsPlatformOutputDevices YPlatformOutputDevices;
