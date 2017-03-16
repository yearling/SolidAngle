// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/OutputDeviceDebug.h"
#include "CoreGlobals.h"
#include "Misc/OutputDeviceHelper.h"

/**
 * Serializes the passed in data unless the current event is suppressed.
 *
 * @param	Data	Text to log
 * @param	Event	Event name used for suppression purposes
 */
void YOutputDeviceDebug::Serialize( const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category, const double Time )
{
	static bool Entry=false;
	if( !GIsCriticalError || Entry )
	{
		if (Verbosity != ELogVerbosity::SetColor)
		{
			YPlatformMisc::LowLevelOutputDebugStringf(TEXT("%s%s"),*YOutputDeviceHelper::FormatLogLine(Verbosity, Category, Data, GPrintLogTimes, Time),LINE_TERMINATOR);
		}
	}
	else
	{
		Entry=true;
		Serialize( Data, Verbosity, Category );
		Entry=false;
	}
}

void YOutputDeviceDebug::Serialize( const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category )
{
	Serialize( Data, Verbosity, Category, -1.0 );
}
