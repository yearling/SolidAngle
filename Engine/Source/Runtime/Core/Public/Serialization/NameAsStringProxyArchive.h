// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Serialization/ArchiveProxy.h"

/**
 * Implements a proxy archive that serializes YNames as string data.
 */
struct YNameAsStringProxyArchive : public YArchiveProxy
{
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InInnerArchive The inner archive to proxy.
	 */
	 YNameAsStringProxyArchive(YArchive& InInnerArchive)
		 :	YArchiveProxy(InInnerArchive)
	 { }

	 CORE_API virtual YArchive& operator<<(class YName& N);
};
