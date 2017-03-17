// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Internationalization/CulturePointer.h"

class FInternationalization;

#if !UE_ENABLE_ICU

class FLegacyInternationalization
{
public:
	FLegacyInternationalization(FInternationalization* const I18N);

	bool Initialize();
	void Terminate();

	void LoadAllCultureData();

	bool IsCultureRemapped(const YString& Name, YString* OutMappedCulture);
	bool IsCultureDisabled(const YString& Name);

	bool SetCurrentCulture(const YString& Name);
	void GetCultureNames(TArray<YString>& CultureNames) const;
	TArray<YString> GetPrioritizedCultureNames(const YString& Name);
	FCulturePtr GetCulture(const YString& Name);

private:
	FInternationalization* const I18N;
};

#endif
