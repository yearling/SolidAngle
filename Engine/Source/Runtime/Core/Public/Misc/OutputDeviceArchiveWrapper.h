#pragma once

#include "CoreTypes.h"
#include "Misc/OutputDevice.h"
#include "Misc/AssertionMacros.h"

/**
* Output device wrapping any kind of YArchive.  Note: Works in any build configuration.
*/
class CORE_API YOutputDeviceArchiveWrapper : public YOutputDevice
{
public:
	/**
	* Constructor, initializing member variables.
	*
	* @param InArchive	Archive to use, must not be nullptr.  Does not take ownership of the archive, clean up or delete the archive independently!
	*/
	YOutputDeviceArchiveWrapper(YArchive* InArchive)
		: LogAr(InArchive)
	{
		check(InArchive);
	}

	// YOutputDevice interface
	virtual void Flush() override;
	virtual void Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category) override;
	// End of YOutputDevice interface

private:
	YArchive* LogAr;
};
