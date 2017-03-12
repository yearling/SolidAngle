// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"

// Any object that is capable of taking commands.
class CORE_API YExec
{
public:
	virtual ~YExec();

	/**
	* Exec handler
	*
	* @param	InWorld	World context
	* @param	Cmd		Command to parse
	* @param	Ar		Output device to log to
	*
	* @return	true if command was handled, false otherwise
	*/
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, YOutputDevice& Ar) PURE_VIRTUAL(YExec::Exec, return false;)
};


