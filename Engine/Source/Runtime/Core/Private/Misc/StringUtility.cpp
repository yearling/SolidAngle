// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/StringUtility.h"
#include "Logging/LogMacros.h"


DEFINE_LOG_CATEGORY_STATIC(LogStringUtility, Log, All);

YString StringUtility::UnescapeURI(const YString& URLString)
{
	// @todo: support decoding of all escaped symbols in a better way

	YString DecodedString = URLString.Replace(TEXT("%20"), TEXT(" "), ESearchCase::CaseSensitive);
	DecodedString = DecodedString.Replace(TEXT("%21"), TEXT("!"), ESearchCase::CaseSensitive);
	DecodedString = DecodedString.Replace(TEXT("%3F"), TEXT("?"), ESearchCase::CaseSensitive);

	return DecodedString;
}
