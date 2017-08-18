// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#if !UE_ENABLE_ICU

#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/FastDecimalFormat.h"

#define LOCTEXT_NAMESPACE "Internationalization"

class FInvariantCulture
{
public:
	static FCultureRef Create()
	{
		FDecimalNumberFormattingRules DecimalNumberFormattingRules;
		DecimalNumberFormattingRules.NaNString = TEXT("NaN");
		DecimalNumberFormattingRules.NegativePrefixString = TEXT("-");
		DecimalNumberFormattingRules.GroupingSeparatorCharacter = ',';
		DecimalNumberFormattingRules.DecimalSeparatorCharacter = '.';
		DecimalNumberFormattingRules.PrimaryGroupingSize = 3;
		DecimalNumberFormattingRules.SecondaryGroupingSize = 3;

		FDecimalNumberFormattingRules PercentFormattingRules;
		PercentFormattingRules.NaNString = TEXT("NaN");
		PercentFormattingRules.NegativePrefixString = TEXT("-");
		PercentFormattingRules.NegativeSuffixString = TEXT("%");
		PercentFormattingRules.PositiveSuffixString = TEXT("%");
		PercentFormattingRules.GroupingSeparatorCharacter = ',';
		PercentFormattingRules.DecimalSeparatorCharacter = '.';
		PercentFormattingRules.PrimaryGroupingSize = 3;
		PercentFormattingRules.SecondaryGroupingSize = 3;

		FDecimalNumberFormattingRules BaseCurrencyFormattingRules;
		BaseCurrencyFormattingRules.NaNString = TEXT("NaN");
		BaseCurrencyFormattingRules.NegativePrefixString = TEXT("-$");
		BaseCurrencyFormattingRules.PositivePrefixString = TEXT("$");
		BaseCurrencyFormattingRules.GroupingSeparatorCharacter = ',';
		BaseCurrencyFormattingRules.DecimalSeparatorCharacter = '.';
		BaseCurrencyFormattingRules.PrimaryGroupingSize = 3;
		BaseCurrencyFormattingRules.SecondaryGroupingSize = 3;

		FCultureRef Culture = FCulture::Create(
			LOCTEXT("InvariantCultureDisplaFName", "Invariant Language (Invariant Country)"),	//const FText DisplaFName
			FString(TEXT("Invariant Language (Invariant Country)")),							//const YString EnglishName
			1033,																				//const int KeyboardLayoutId
			1033,																				//const int LCID
			FString(TEXT("")),																	//const YString Name
			FString(TEXT("Invariant Language (Invariant Country)")),							//const YString NativeName
			FString(TEXT("INT")),																//const YString UnrealLegacyThreeLetterISOLanguageName
			FString(TEXT("ivl")),																//const YString ThreeLetterISOLanguageName
			FString(TEXT("iv")),																//const YString TwoLetterISOLanguageName
			DecimalNumberFormattingRules,														//const FDecimalNumberFormattingRules InDecimalNumberFormattingRules
			PercentFormattingRules,																//const FDecimalNumberFormattingRules InPercentFormattingRules
			BaseCurrencyFormattingRules															//const FDecimalNumberFormattingRules InBaseCurrencyFormattingRules
			).ToSharedRef();

		return Culture;
	}
};

#undef LOCTEXT_NAMESPACE

#endif
