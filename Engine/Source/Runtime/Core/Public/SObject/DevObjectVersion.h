// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreTypes.h"
#include "SObject/NameTypes.h"
#include "Misc/Guid.h"
#include "Serialization/CustomVersion.h"
//#include "LoadTimesObjectVersion.h"

class CORE_API FDevVersionRegistration :  public FCustomVersionRegistration
{
public:
	FDevVersionRegistration(YGuid InKey, int32 Version, YName InFriendlyName);

	/** Dumps all registered versions to log */
	static void DumpVersionsToLog();
};
