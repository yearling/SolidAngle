#pragma once

#include "CoreTypes.h"
#include "Misc/OutputDevice.h"

class CORE_API FOutputDeviceDebug : public YOutputDevice
{
public:
	/**
	* Serializes the passed in data unless the current event is suppressed.
	*
	* @param	Data	Text to log
	* @param	Event	Event name used for suppression purposes
	*/
	virtual void Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category, const double Time) override;

	virtual void Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category) override;

	virtual bool CanBeUsedOnAnyThread() const override
	{
		return true;
	}
};

