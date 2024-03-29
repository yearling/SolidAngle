// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformProperties.h"


/**
* Implements Windows platform properties.
*/
template<bool HAS_EDITOR_DATA, bool IS_DEDICATED_SERVER, bool IS_CLIENT_ONLY>
struct YWindowsPlatformProperties
	: public YGenericPlatformProperties
{
	static FORCEINLINE bool HasEditorOnlyData()
	{
		return HAS_EDITOR_DATA;
	}

	static FORCEINLINE const char* IniPlatformName()
	{
		return "Windows";
	}

	static FORCEINLINE bool IsGameOnly()
	{
		return UE_GAME;
	}

	static FORCEINLINE bool IsServerOnly()
	{
		return IS_DEDICATED_SERVER;
	}

	static FORCEINLINE bool IsClientOnly()
	{
		return IS_CLIENT_ONLY;
	}

	static FORCEINLINE const char* PlatformName()
	{
		if (IS_DEDICATED_SERVER)
		{
			return "WindowsServer";
		}

		if (HAS_EDITOR_DATA)
		{
			return "Windows";
		}

		if (IS_CLIENT_ONLY)
		{
			return "WindowsClient";
		}

		return "WindowsNoEditor";
	}

	static FORCEINLINE bool RequiresCookedData()
	{
		return !HAS_EDITOR_DATA;
	}

	static FORCEINLINE bool SupportsAudioStreaming()
	{
		return !IsServerOnly();
	}

	static FORCEINLINE bool SupportsGrayscaleSRGB()
	{
		return false; // Requires expand from G8 to RGBA
	}

	static FORCEINLINE bool SupportsMultipleGameInstances()
	{
		return true;
	}

	static FORCEINLINE bool SupportsTessellation()
	{
		return true; // DX11 compatible
	}

	static FORCEINLINE bool SupportsWindowedMode()
	{
		return true;
	}

	static FORCEINLINE bool HasFixedResolution()
	{
		return false;
	}

	static FORCEINLINE bool SupportsQuit()
	{
		return true;
	}

	static FORCEINLINE float GetVariantPriority()
	{
		if (IS_DEDICATED_SERVER)
		{
			return 0.0f;
		}

		if (HAS_EDITOR_DATA)
		{
			return 0.0f;
		}

		if (IS_CLIENT_ONLY)
		{
			return 0.0f;
		}

		return 1.0f;
	}
};
