// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Internationalization/Culture.h"

#if UE_ENABLE_ICU
#include "Internationalization/ICUCulture.h"
#else
#include "Internationalization/LegacyCulture.h"
#endif

#if UE_ENABLE_ICU
FCulturePtr FCulture::Create(const YString& LocaleName)
{
	return MakeShareable(new FCulture(LocaleName));
}
#else
FCulturePtr FCulture::Create(
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
	)
{
	return MakeShareable(new FCulture(InDisplaFName, InEnglishName, InKeyboardLayoutId, InLCID, InName, InNativeName, InUnrealLegacyThreeLetterISOLanguageName, InThreeLetterISOLanguageName, InTwoLetterISOLanguageName, InDecimalNumberFormattingRules, InPercentFormattingRules, InBaseCurrencyFormattingRules));
}
#endif

#if UE_ENABLE_ICU
FCulture::FCulture(const YString& LocaleName)
	: Implementation( new FICUCultureImplementation( LocaleName ) )
#else
FCulture::FCulture(
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
	) 
	: Implementation( new FLegacyCultureImplementation(InDisplaFName, InEnglishName, InKeyboardLayoutId, InLCID, InName, InNativeName, InUnrealLegacyThreeLetterISOLanguageName, InThreeLetterISOLanguageName, InTwoLetterISOLanguageName, InDecimalNumberFormattingRules, InPercentFormattingRules, InBaseCurrencyFormattingRules) )
#endif
	, CachedDisplaFName(Implementation->GetDisplaFName())
	, CachedEnglishName(Implementation->GetEnglishName())
	, CachedName(Implementation->GetName())
	, CachedNativeName(Implementation->GetNativeName())
	, CachedUnrealLegacyThreeLetterISOLanguageName(Implementation->GetUnrealLegacyThreeLetterISOLanguageName())
	, CachedThreeLetterISOLanguageName(Implementation->GetThreeLetterISOLanguageName())
	, CachedTwoLetterISOLanguageName(Implementation->GetTwoLetterISOLanguageName())
	, CachedNativeLanguage(Implementation->GetNativeLanguage())
#if UE_ENABLE_ICU
	, CachedRegion(Implementation->GetRegion())
#endif
	, CachedNativeRegion(Implementation->GetNativeRegion())
#if UE_ENABLE_ICU
	, CachedScript(Implementation->GetScript())
	, CachedVariant(Implementation->GetVariant())
#endif
{ 
}

const YString& FCulture::GetDisplaFName() const
{
	return CachedDisplaFName;
}

const YString& FCulture::GetEnglishName() const
{
	return CachedEnglishName;
}

int FCulture::GetKeyboardLayoutId() const
{
	return Implementation->GetKeyboardLayoutId();
}

int FCulture::GetLCID() const
{
	return Implementation->GetLCID();
}

TArray<YString> FCulture::GetPrioritizedParentCultureNames() const
{
	return GetPrioritizedParentCultureNames(GetTwoLetterISOLanguageName(), GetScript(), GetRegion());
}

TArray<YString> FCulture::GetPrioritizedParentCultureNames(const YString& LanguageCode, const YString& ScriptCode, const YString& RegionCode)
{
	TArray<YString> LocaleTagCombinations;
	if (!ScriptCode.IsEmpty() && !RegionCode.IsEmpty())
	{
		LocaleTagCombinations.Add(LanguageCode + TEXT("-") + ScriptCode + TEXT("-") + RegionCode);
	}
	if (!RegionCode.IsEmpty())
	{
		LocaleTagCombinations.Add(LanguageCode + TEXT("-") + RegionCode);
	}
	if (!ScriptCode.IsEmpty())
	{
		LocaleTagCombinations.Add(LanguageCode + TEXT("-") + ScriptCode);
	}
	LocaleTagCombinations.Add(LanguageCode);

	return LocaleTagCombinations;
}

YString FCulture::GetCanonicalName(const YString& Name)
{
	return FImplementation::GetCanonicalName(Name);
}

const YString& FCulture::GetName() const
{
	return CachedName;
}

const YString& FCulture::GetNativeName() const
{
	return CachedNativeName;
}

const YString& FCulture::GetUnrealLegacyThreeLetterISOLanguageName() const
{
	return CachedUnrealLegacyThreeLetterISOLanguageName;
}

const YString& FCulture::GetThreeLetterISOLanguageName() const
{
	return CachedThreeLetterISOLanguageName;
}

const YString& FCulture::GetTwoLetterISOLanguageName() const
{
	return CachedTwoLetterISOLanguageName;
}

const YString& FCulture::GetNativeLanguage() const
{
	return CachedNativeLanguage;
}

const YString& FCulture::GetRegion() const
{
	return CachedRegion;
}

const YString& FCulture::GetNativeRegion() const
{
	return CachedNativeRegion;
}

const YString& FCulture::GetScript() const
{
	return CachedScript;
}

const YString& FCulture::GetVariant() const
{
	return CachedVariant;
}

const FDecimalNumberFormattingRules& FCulture::GetDecimalNumberFormattingRules() const
{
	return Implementation->GetDecimalNumberFormattingRules();
}

const FDecimalNumberFormattingRules& FCulture::GetPercentFormattingRules() const
{
	return Implementation->GetPercentFormattingRules();
}

const FDecimalNumberFormattingRules& FCulture::GetCurrencyFormattingRules(const YString& InCurrencyCode) const
{
	return Implementation->GetCurrencyFormattingRules(InCurrencyCode);
}

/**
 * Get the correct plural form to use for the given number
 * ICU only supports int32 and double, so we cast larger int values to double to try and keep as much precision as possible
 */

#define DEF_GETPLURALFORM_CAST(T1, T2) ETextPluralForm FCulture::GetPluralForm(T1 Val, const ETextPluralType PluralType) const { return GetPluralForm((T2)Val, PluralType); }
DEF_GETPLURALFORM_CAST(float, double)
DEF_GETPLURALFORM_CAST(int8, int32)
DEF_GETPLURALFORM_CAST(int16, int32)
DEF_GETPLURALFORM_CAST(int64, double)
DEF_GETPLURALFORM_CAST(uint8, int32)
DEF_GETPLURALFORM_CAST(uint16, int32)
DEF_GETPLURALFORM_CAST(uint32, double)
DEF_GETPLURALFORM_CAST(uint64, double)
#undef DEF_GETPLURALFORM_CAST

ETextPluralForm FCulture::GetPluralForm(int32 Val, const ETextPluralType PluralType) const
{
	if (Val < 0)
	{
		// Must be positive
		Val *= -1;
	}
	return Implementation->GetPluralForm(Val, PluralType);
}

ETextPluralForm FCulture::GetPluralForm(double Val, const ETextPluralType PluralType) const
{
	if (YMath::IsNegativeDouble(Val))
	{
		// Must be positive
		Val *= -1.0;
	}
	return Implementation->GetPluralForm(Val, PluralType);
}

void FCulture::HandleCultureChanged()
{
	// Re-cache the DisplaFName, as this may change when the active culture is changed
	CachedDisplaFName = Implementation->GetDisplaFName();
}
