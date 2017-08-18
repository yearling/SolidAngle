// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"
#include "Containers/Map.h"
#include "Internationalization/Text.h"
#include "Internationalization/Culture.h"
#include "Internationalization/FastDecimalFormat.h"

#if !UE_ENABLE_ICU
class FCulture::FLegacyCultureImplementation
{
	friend FCulture;

	FLegacyCultureImplementation(
		const FText& InDisplaFName, 
		const YString& InEnglishName, 
		const int InKeyboardLayoutId, 
		const int InLCID, 
		const YString& InName, 
		const YString& InNativeName, 
		const YString& InUnrealLegacyThreeLetterISOLanguageName, 
		const YString& InThreeLetterISOLanguageName, 
		const YString& InTwoLetterISOLanguageName,
		const FDecimalNumberFormattingRules& InDecimalNumberFormattingRules,
		const FDecimalNumberFormattingRules& InPercentFormattingRules,
		const FDecimalNumberFormattingRules& InBaseCurrencyFormattingRules
		);

	YString GetDisplaFName() const;

	YString GetEnglishName() const;

	int GetKeyboardLayoutId() const;

	int GetLCID() const;

	static YString GetCanonicalName(const YString& Name);

	YString GetName() const;

	YString GetNativeName() const;

	YString GetUnrealLegacyThreeLetterISOLanguageName() const;

	YString GetThreeLetterISOLanguageName() const;

	YString GetTwoLetterISOLanguageName() const;

	YString GetNativeLanguage() const;

	YString GetNativeRegion() const;

	const FDecimalNumberFormattingRules& GetDecimalNumberFormattingRules();

	const FDecimalNumberFormattingRules& GetPercentFormattingRules();

	const FDecimalNumberFormattingRules& GetCurrencyFormattingRules(const YString& InCurrencyCode);

	ETextPluralForm GetPluralForm(int32 Val, const ETextPluralType PluralType);

	ETextPluralForm GetPluralForm(double Val, const ETextPluralType PluralType);

	// Full localized culture name
	const FText DisplaFName;

	// The English name of the culture in format languagefull [country/regionfull]
	const YString EnglishName;

	// Keyboard input locale id
	const int KeyboardLayoutId;

	// id for this Culture
	const int LCID;

	// Name of the culture in languagecode2-country/regioncode2 format
	const YString Name;

	// The culture name, consisting of the language, the country/region, and the optional script
	const YString NativeName;

	// ISO 639-2 three letter code of the language - for the purpose of supporting legacy Unreal documentation.
	const YString UnrealLegacyThreeLetterISOLanguageName;

	// ISO 639-2 three letter code of the language
	const YString ThreeLetterISOLanguageName;

	// ISO 639-1 two letter code of the language
	const YString TwoLetterISOLanguageName;

	// Rules for formatting decimal numbers in this culture
	const FDecimalNumberFormattingRules DecimalNumberFormattingRules;

	// Rules for formatting percentile numbers in this culture
	const FDecimalNumberFormattingRules PercentFormattingRules;

	// Rules for formatting currency numbers in this culture
	const FDecimalNumberFormattingRules BaseCurrencyFormattingRules;

	// Rules for formatting alternate currencies in this culture
	TMap<YString, TSharedPtr<const FDecimalNumberFormattingRules>> UEAlternateCurrencyFormattingRules;
	FCriticalSection UEAlternateCurrencyFormattingRulesCS;
};
#endif
