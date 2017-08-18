// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/OutputDeviceArchiveWrapper.h"
#include "Misc/OutputDeviceHelper.h"

void YOutputDeviceArchiveWrapper::Flush()
{
	LogAr->Flush();
}

void YOutputDeviceArchiveWrapper::Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	if (Verbosity != ELogVerbosity::SetColor)
	{
		YOutputDeviceHelper::FormatCastAndSerializeLine(*LogAr, Data, Verbosity, Category, -1.0, true, bAutoEmitLineTerminator);
	}
}

