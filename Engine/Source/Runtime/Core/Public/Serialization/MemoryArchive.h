// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Serialization/Archive.h"
#include "Containers/SolidAngleString.h"
#include "SObject/NameTypes.h"

/**
 * Base class for serializing arbitrary data in memory.
 */
class YMemoryArchive : public YArchive
{
public:
	/**
  	 * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
	 * is in when a loading error occurs.
	 *
	 * This is overridden for the specific Archive Types
	 **/
	virtual YString GetArchiveName() const { return TEXT("YMemoryArchive"); }

	void Seek( int64 InPos ) final
	{
		Offset = InPos;
	}
	int64 Tell() final
	{
		return Offset;
	}

	using YArchive::operator<<; // For visibility of the overloads we don't override

	virtual YArchive& operator<<( class YName& N ) override
	{
		// Serialize the YName as a string
		if (IsLoading())
		{
			YString StringName;
			*this << StringName;
			N = YName(*StringName);
		}
		else
		{
			YString StringName = N.ToString();
			*this << StringName;
		}
		return *this;
	}

	virtual YArchive& operator<<( class SObject*& Res ) override
	{
		// Not supported through this archive
		check(0);
		return *this;
	}

protected:

	/** Marked as protected to avoid instantiating this class directly */
	YMemoryArchive()
		: YArchive(), Offset(0)
	{
	}

	int64				Offset;
};

