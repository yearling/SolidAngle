// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "HAL/SolidAngleMemory.h"
#include "Math/SolidAngleMathUtility.h"
#include "Containers/SolidAngleString.h"
#include "Serialization/MemoryArchive.h"

/**
 * Archive for reading arbitrary data from the specified memory location
 */
class YMemoryReader final : public YMemoryArchive
{
public:
	/**
  	 * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
	 * is in when a loading error occurs.
	 *
	 * This is overridden for the specific Archive Types
	 **/
	virtual YString GetArchiveName() const { return TEXT("YMemoryReader"); }

	int64 TotalSize()
	{
		return YMath::Min((int64)Bytes.Num(), LimitSize);
	}

	void Serialize( void* Data, int64 Num )
	{
		if (Num && !ArIsError)
		{
			// Only serialize if we have the requested amount of data
			if (Offset + Num <= TotalSize())
			{
				YMemory::Memcpy( Data, &Bytes[Offset], Num );
				Offset += Num;
			}
			else
			{
				ArIsError = true;
			}
		}
	}

	YMemoryReader( const TArray<uint8>& InBytes, bool bIsPersistent = false )
	: YMemoryArchive()
	, Bytes(InBytes)
	, LimitSize(INT64_MAX)
	{
		ArIsLoading		= true;
		ArIsPersistent	= bIsPersistent;
	}

	/** With this method it's possible to attach data behind some serialized data. */
	void SetLimitSize(int64 NewLimitSize)
	{
		LimitSize = NewLimitSize;
	}

protected:

	const TArray<uint8>& Bytes;
	int64 LimitSize;
};

