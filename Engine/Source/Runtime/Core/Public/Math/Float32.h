// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

/**
* 32 bit float components
*/
class YFloat32
{
public:

	union
	{
		struct
		{
#if PLATFORM_LITTLE_ENDIAN
			uint32	Mantissa : 23;
			uint32	Exponent : 8;
			uint32	Sign : 1;			
#else
			uint32	Sign : 1;
			uint32	Exponent : 8;
			uint32	Mantissa : 23;			
#endif
		} Components;

		float	FloatValue;
	};

	/**
	 * Constructor
	 * 
	 * @param InValue value of the float.
	 */
	YFloat32( float InValue=0.0f );
};


FORCEINLINE YFloat32::YFloat32( float InValue)
	: FloatValue(InValue)
{ }
