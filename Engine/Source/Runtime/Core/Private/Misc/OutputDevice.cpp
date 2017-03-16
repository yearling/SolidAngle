// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/OutputDevice.h"
#include "Containers/SolidAngleString.h"
#include "Logging/LogMacros.h"
#include "Internationalization/Text.h"
#include "Logging/LogScopedCategoryAndVerbosityOverride.h"
#include "Misc/OutputDeviceHelper.h"
#include "Misc/VarargsHelper.h"

DEFINE_LOG_CATEGORY(LogOutputDevice);

const TCHAR* YOutputDevice::VerbosityToString(ELogVerbosity::Type Verbosity)
{
	return YOutputDeviceHelper::VerbosityToString(Verbosity);
}

YString YOutputDevice::FormatLogLine( ELogVerbosity::Type Verbosity, const class YName& Category, const TCHAR* Message /*= nullptr*/, ELogTimes::Type LogTime /*= ELogTimes::None*/, const double Time /*= -1.0*/ )
{
	return YOutputDeviceHelper::FormatLogLine(Verbosity, Category, Message, LogTime, Time);
}

void YOutputDevice::Log( ELogVerbosity::Type Verbosity, const TCHAR* Str )
{
	Serialize( Str, Verbosity, NAME_None );
}
void YOutputDevice::Log( ELogVerbosity::Type Verbosity, const YString& S )
{
	Serialize( *S, Verbosity, NAME_None );
}
void YOutputDevice::Log( const class YName& Category, ELogVerbosity::Type Verbosity, const TCHAR* Str )
{
	Serialize( Str, Verbosity, Category );
}
void YOutputDevice::Log( const class YName& Category, ELogVerbosity::Type Verbosity, const YString& S )
{
	Serialize( *S, Verbosity, Category );
}
void YOutputDevice::Log( const TCHAR* Str )
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	Serialize( Str, TLS->Verbosity, TLS->Category );
}
void YOutputDevice::Log( const YString& S )
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	Serialize( *S, TLS->Verbosity, TLS->Category );
}
void YOutputDevice::Log( const FText& T )
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	Serialize( *T.ToString(), TLS->Verbosity, TLS->Category );
}
/*-----------------------------------------------------------------------------
	Formatted printing and messages.
-----------------------------------------------------------------------------*/

VARARG_BODY( void, YOutputDevice::CategorizedLogf, const TCHAR*, VARARG_EXTRA(const class YName& Category) VARARG_EXTRA(ELogVerbosity::Type Verbosity)  )
{
	GROWABLE_LOGF(Serialize(Buffer, Verbosity, Category))
}
VARARG_BODY( void, YOutputDevice::Logf, const TCHAR*, VARARG_EXTRA(ELogVerbosity::Type Verbosity) )
{
	// call serialize with the final buffer
	GROWABLE_LOGF(Serialize(Buffer, Verbosity, NAME_None))
}
VARARG_BODY( void, YOutputDevice::Logf, const TCHAR*, VARARG_NONE )
{
	FScopedCategoryAndVerbosityOverride::FOverride* TLS = FScopedCategoryAndVerbosityOverride::GetTLSCurrent();
	GROWABLE_LOGF(Serialize(Buffer, TLS->Verbosity, TLS->Category))
}



/** Critical errors. */
CORE_API YOutputDeviceError* GError = NULL;

