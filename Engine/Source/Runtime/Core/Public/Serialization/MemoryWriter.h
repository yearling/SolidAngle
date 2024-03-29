// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "HAL/SolidAngleMemory.h"
#include "Math/NumericLimits.h"
#include "Containers/SolidAngleString.h"
#include "SObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
#include "Serialization/MemoryArchive.h"

/**
 * Archive for storing arbitrary data to the specified memory location
 */
class YMemoryWriter : public YMemoryArchive
{
public:
	YMemoryWriter( TArray<uint8>& InBytes, bool bIsPersistent = false, bool bSetOffset = false, const YName InArchiveName = NAME_None )
	: YMemoryArchive()
	, Bytes(InBytes)
	, ArchiveName(InArchiveName)
	{
		ArIsSaving		= true;
		ArIsPersistent	= bIsPersistent;
		if (bSetOffset)
		{
			Offset = InBytes.Num();
		}
	}

	virtual void Serialize(void* Data, int64 Num) override
	{
		const int64 NumBytesToAdd = Offset + Num - Bytes.Num();
		if( NumBytesToAdd > 0 )
		{
			const int64 NewArrayCount = Bytes.Num() + NumBytesToAdd;
			if( NewArrayCount >= MAX_int32 )
			{
				UE_LOG( LogSerialization, Fatal, TEXT( "YMemoryWriter does not support data larger than 2GB. Archive name: %s." ), *ArchiveName.ToString() );
			}

			Bytes.AddUninitialized( (int32)NumBytesToAdd );
		}

		check((Offset + Num) <= Bytes.Num());
		
		if( Num )
		{
			YMemory::Memcpy( &Bytes[Offset], Data, Num );
			Offset+=Num;
		}
	}
	/**
  	 * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
	 * is in when a loading error occurs.
	 *
	 * This is overridden for the specific Archive Types
	 **/
	virtual YString GetArchiveName() const override { return TEXT("YMemoryWriter"); }

	int64 TotalSize() override
	{
		return Bytes.Num();
	}

protected:

	TArray<uint8>&	Bytes;

	/** Archive name, used to debugging, by default set to NAME_None. */
	const YName ArchiveName;
};

