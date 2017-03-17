// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "Internationalization/LegacyInternationalization.h"

#if !UE_ENABLE_ICU

#include "InvariantCulture.h"

FLegacyInternationalization::FLegacyInternationalization(FInternationalization* const InI18N)
	: I18N(InI18N)
{

}

bool FLegacyInternationalization::Initialize()
{
	I18N->InvariantCulture = FInvariantCulture::Create();
	I18N->DefaultCulture = I18N->InvariantCulture;
	I18N->CurrentCulture = I18N->InvariantCulture;

	return true;
}

void FLegacyInternationalization::Terminate()
{
}

void FLegacyInternationalization::LoadAllCultureData()
{
}

bool FLegacyInternationalization::IsCultureRemapped(const YString& Name, YString* OutMappedCulture)
{
	return false;
}

bool FLegacyInternationalization::IsCultureDisabled(const YString& Name)
{
	return false;
}

bool FLegacyInternationalization::SetCurrentCulture(const YString& Name)
{
	return Name.IsEmpty();
}

void FLegacyInternationalization::GetCultureNames(TArray<YString>& CultureNames) const
{
	CultureNames.Add(TEXT(""));
}

TArray<YString> FLegacyInternationalization::GetPrioritizedCultureNames(const YString& Name)
{
	TArray<YString> PrioritizedCultureNames;
	PrioritizedCultureNames.Add(Name);
	return PrioritizedCultureNames;
}

FCulturePtr FLegacyInternationalization::GetCulture(const YString& Name)
{
	return Name.IsEmpty() ? I18N->InvariantCulture : nullptr;
}

#endif
