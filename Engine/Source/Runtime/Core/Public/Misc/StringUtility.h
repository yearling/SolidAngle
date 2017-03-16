// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"

namespace StringUtility
{
	/**
	 * Unescapes a URI
	 *
	 * @param URLString an escaped string (e.g. File%20Name)
	 *
	 * @return un-escaped string (e.g. "File Name")
	 */
	YString CORE_API UnescapeURI(const YString& URLString);
}

