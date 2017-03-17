// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"
#include "Misc/Timespan.h"

#if UE_ENABLE_ICU
THIRD_PARTY_INCLUDES_START
	#include <unicode/unistr.h>
THIRD_PARTY_INCLUDES_END

namespace ICUUtilities
{
	/** 
	 * An object that can convert between YString and icu::UnicodeString
	 * Note: This object is not thread-safe.
	 */
	class FStringConverter
	{
	public:
		FStringConverter();
		~FStringConverter();

		/** Convert YString -> icu::UnicodeString */
		void ConvertString(const YString& Source, icu::UnicodeString& Destination, const bool ShouldNullTerminate = true);
		void ConvertString(const TCHAR* Source, const int32 SourceStartIndex, const int32 SourceLen, icu::UnicodeString& Destination, const bool ShouldNullTerminate = true);
		icu::UnicodeString ConvertString(const YString& Source, const bool ShouldNullTerminate = true);
		icu::UnicodeString ConvertString(const TCHAR* Source, const int32 SourceStartIndex, const int32 SourceLen, const bool ShouldNullTerminate = true);

		/** Convert icu::UnicodeString -> YString */
		void ConvertString(const icu::UnicodeString& Source, YString& Destination);
		void ConvertString(const icu::UnicodeString& Source, const int32 SourceStartIndex, const int32 SourceLen, YString& Destination);
		YString ConvertString(const icu::UnicodeString& Source);
		YString ConvertString(const icu::UnicodeString& Source, const int32 SourceStartIndex, const int32 SourceLen);

	private:
		/** Non-copyable */
		FStringConverter(const FStringConverter&);
		FStringConverter& operator=(const FStringConverter&);

		UConverter* ICUConverter;
	};

	/** Convert YString -> icu::UnicodeString */
	void ConvertString(const YString& Source, icu::UnicodeString& Destination, const bool ShouldNullTerminate = true);
	void ConvertString(const TCHAR* Source, const int32 SourceStartIndex, const int32 SourceLen, icu::UnicodeString& Destination, const bool ShouldNullTerminate = true);
	icu::UnicodeString ConvertString(const YString& Source, const bool ShouldNullTerminate = true);
	icu::UnicodeString ConvertString(const TCHAR* Source, const int32 SourceStartIndex, const int32 SourceLen, const bool ShouldNullTerminate = true);

	/** Convert icu::UnicodeString -> YString */
	void ConvertString(const icu::UnicodeString& Source, YString& Destination);
	void ConvertString(const icu::UnicodeString& Source, const int32 SourceStartIndex, const int32 SourceLen, YString& Destination);
	YString ConvertString(const icu::UnicodeString& Source);
	YString ConvertString(const icu::UnicodeString& Source, const int32 SourceStartIndex, const int32 SourceLen);

	/** Given an icu::UnicodeString, count how many characters it would be if converted into an YString (as YString may not always be UTF-16) */
	int32 GetNativeStringLength(const icu::UnicodeString& Source);
	int32 GetNativeStringLength(const icu::UnicodeString& Source, const int32 InSourceStartIndex, const int32 InSourceLength);

	/** Given an YString, count how many characters it would be if converted to an icu::UnicodeString (as YString may not always be UTF-16) */
	int32 GetUnicodeStringLength(const YString& Source);
	int32 GetUnicodeStringLength(const TCHAR* Source, const int32 InSourceStartIndex, const int32 InSourceLength);
}
#endif
