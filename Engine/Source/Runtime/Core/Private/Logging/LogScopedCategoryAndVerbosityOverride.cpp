// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Logging/LogScopedCategoryAndVerbosityOverride.h"
#include "HAL/PlatformTLS.h"

FLogScopedCategoryAndVerbosityOverride::FLogScopedCategoryAndVerbosityOverride(YName Category, ELogVerbosity::Type Verbosity)
{
	FOverride* TLS = GetTLSCurrent();
	Backup = *TLS;
	*TLS = FOverride(Category, Verbosity);
}

FLogScopedCategoryAndVerbosityOverride::~FLogScopedCategoryAndVerbosityOverride()
{
	FOverride* TLS = GetTLSCurrent();
	*TLS = Backup;
}

static uint32 OverrrideTLSID = YPlatformTLS::AllocTlsSlot();
FLogScopedCategoryAndVerbosityOverride::FOverride* FLogScopedCategoryAndVerbosityOverride::GetTLSCurrent()
{
	FOverride* TLS = (FOverride*)YPlatformTLS::GetTlsValue(OverrrideTLSID);
	if (!TLS)
	{
		TLS = new FOverride;
		YPlatformTLS::SetTlsValue(OverrrideTLSID, TLS);
	}	
	return TLS;
}

