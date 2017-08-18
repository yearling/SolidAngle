// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Templates/SharedPointer.h"
#include "Internationalization/CulturePointer.h"

struct FDecimalNumberFormattingRules;
enum class ETextPluralForm : uint8;
enum class ETextPluralType : uint8;

class CORE_API FCulture
{
public:
#if UE_ENABLE_ICU
	static FCulturePtr Create(const YString& LocaleName);
#else
	static FCulturePtr Create(
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
#endif

	const YString& GetDisplaFName() const;

	const YString& GetEnglishName() const;

	int GetKeyboardLayoutId() const;

	int GetLCID() const;

	TArray<YString> GetPrioritizedParentCultureNames() const;

	static TArray<YString> GetPrioritizedParentCultureNames(const YString& LanguageCode, const YString& ScriptCode, const YString& RegionCode);

	static YString GetCanonicalName(const YString& Name);

	const YString& GetName() const;

	const YString& GetNativeName() const;

	const YString& GetUnrealLegacyThreeLetterISOLanguageName() const;

	const YString& GetThreeLetterISOLanguageName() const;

	const YString& GetTwoLetterISOLanguageName() const;

	const YString& GetNativeLanguage() const;

	const YString& GetRegion() const;

	const YString& GetNativeRegion() const;

	const YString& GetScript() const;

	const YString& GetVariant() const;

	const FDecimalNumberFormattingRules& GetDecimalNumberFormattingRules() const;

	const FDecimalNumberFormattingRules& GetPercentFormattingRules() const;

	const FDecimalNumberFormattingRules& GetCurrencyFormattingRules(const YString& InCurrencyCode) const;

	/**
	* Get the correct plural form to use for the given number
	* @param PluralType The type of plural form to get (cardinal or ordinal)
	*/
	ETextPluralForm GetPluralForm(float Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(double Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(int8 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(int16 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(int32 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(int64 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(uint8 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(uint16 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(uint32 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(uint64 Val, const ETextPluralType PluralType) const;
	ETextPluralForm GetPluralForm(long Val, const ETextPluralType PluralType) const;

	void HandleCultureChanged();

public:
#if UE_ENABLE_ICU
	class FICUCultureImplementation;
	typedef FICUCultureImplementation FImplementation;
	TSharedRef<FICUCultureImplementation> Implementation;
#else
	class FLegacyCultureImplementation;
	typedef FLegacyCultureImplementation FImplementation;
	TSharedRef<FLegacyCultureImplementation> Implementation;
#endif

protected:
#if UE_ENABLE_ICU
	FCulture(const YString& LocaleName);
#else
	FCulture(
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
#endif

private:
#if !UE_ENABLE_ICU
	friend class FText;
#endif

	YString CachedDisplaFName;
	YString CachedEnglishName;
	YString CachedName;
	YString CachedNativeName;
	YString CachedUnrealLegacyThreeLetterISOLanguageName;
	YString CachedThreeLetterISOLanguageName;
	YString CachedTwoLetterISOLanguageName;
	YString CachedNativeLanguage;
	YString CachedRegion;
	YString CachedNativeRegion;
	YString CachedScript;
	YString CachedVariant;
};
