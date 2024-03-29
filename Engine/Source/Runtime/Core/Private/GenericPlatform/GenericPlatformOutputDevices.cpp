// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GenericPlatform/GenericPlatformOutputDevices.h"
#include "HAL/PlatformOutputDevices.h"
#include "CoreGlobals.h"
#include "Misc/Parse.h"
#include "Templates/ScopedPointer.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/OutputDeviceMemory.h"
#include "Misc/OutputDeviceFile.h"
#include "Misc/OutputDeviceDebug.h"
#include "Misc/OutputDeviceAnsiError.h"
#include "Misc/App.h"
#include "HAL/FeedbackContextAnsi.h"
#include "Misc/OutputDeviceConsole.h"
#include "Templates/UniquePtr.h"

void YGenericPlatformOutputDevices::SetupOutputDevices()
{
	check(GLog);

	GLog->AddOutputDevice(YPlatformOutputDevices::GetLog());

	bool bHasConsole = !FParse::Param(FCommandLine::Get(), TEXT("NOCONSOLE"));
	if (bHasConsole)
	{
		GLog->AddOutputDevice(GLogConsole);
	}

	// Only create debug output device if a debugger is attached or we're running on a console or build machine
	// A shipping build with logging explicitly enabled will fail the IsDebuggerPresent() check, but we still need to add the debug output device for logging purposes
	if (!FPlatformProperties::SupportsWindowedMode() || YPlatformMisc::IsDebuggerPresent() || (UE_BUILD_SHIPPING && !NO_LOGGING) || GIsBuildMachine)
	{
		// Only need to do this if it's actually going to go to a different place than GLogConsole
		if(!bHasConsole || YPlatformMisc::HasSeparateChannelForDebugOutput())
		{
			GLog->AddOutputDevice(new YOutputDeviceDebug());
		}
	}

	GLog->AddOutputDevice(YPlatformOutputDevices::GetEventLog());
};


YString YGenericPlatformOutputDevices::GetAbsoluteLogFilename()
{
	static TCHAR		Filename[1024] = { 0 };

	if (!Filename[0])
	{
		FCString::Strcpy(Filename, ARRAY_COUNT(Filename), *YPaths::GameLogDir());
		YString LogFilename;
		if (!FParse::Value(FCommandLine::Get(), TEXT("LOG="), LogFilename))
		{
			if (FParse::Value(FCommandLine::Get(), TEXT("ABSLOG="), LogFilename))
			{
				Filename[0] = 0;
			}
		}

		YString Extension(YPaths::GetExtension(LogFilename));
		if (Extension != TEXT("log") && Extension != TEXT("txt"))
		{
			// Ignoring the specified log filename because it doesn't have a .log extension			
			LogFilename.Empty();
		}

		if (LogFilename.Len() == 0)
		{
			if (FCString::Strlen(FApp::GetGameName()) != 0)
			{
				LogFilename = FApp::GetGameName();
			}
			else
			{
				LogFilename = TEXT("UE4");
			}

			LogFilename += TEXT(".log");
		}

		FCString::Strcat(Filename, ARRAY_COUNT(Filename) - FCString::Strlen(Filename), *LogFilename);
	}

	return Filename;
}

#ifndef WITH_LOGGING_TO_MEMORY
	#define WITH_LOGGING_TO_MEMORY 0
#endif

class YOutputDevice* YGenericPlatformOutputDevices::GetLog()
{
	static struct FLogOutputDeviceInitializer
	{
		TUniquePtr<YOutputDevice> LogDevice;
		FLogOutputDeviceInitializer()
		{
#if WITH_LOGGING_TO_MEMORY
#if !IS_PROGRAM && !WITH_EDITORONLY_DATA
			if (!LogDevice
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				 && FParse::Param(FCommandLine::Get(), TEXT("LOGTOMEMORY")) 
#else
				 && !FParse::Param(FCommandLine::Get(), TEXT("NOLOGTOMEMORY")) && !FPlatformProperties::IsServerOnly()
#endif
				 )
			{
				LogDevice = MakeUnique<YOutputDeviceMemory>();
			}
#endif // !IS_PROGRAM && !WITH_EDITORONLY_DATA
#endif // WITH_LOGGING_TO_MEMORY
			if (!LogDevice)
			{
				LogDevice = MakeUnique<YOutputDeviceFile>();
			}
		}

	} Singleton;

	return Singleton.LogDevice.Get();
}


class YOutputDeviceError* YGenericPlatformOutputDevices::GetError()
{
	static YOutputDeviceAnsiError Singleton;
	return &Singleton;
}


class YFeedbackContext* YGenericPlatformOutputDevices::GetWarn()
{
	static FFeedbackContextAnsi Singleton;
	return &Singleton;
}
