// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/OutputDevice.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "SObject/NameTypes.h"

/*-----------------------------------------------------------------------------
YOutputDeviceRedirector.
-----------------------------------------------------------------------------*/

/** The type of lines buffered by secondary threads. */
struct FBufferedLine
{
	const YString Data;
	const YName Category;
	const double Time;
	const ELogVerbosity::Type Verbosity;

	/** Initialization constructor. */
	FBufferedLine(const TCHAR* InData, const class YName& InCategory, ELogVerbosity::Type InVerbosity, const double InTime = -1)
		: Data(InData)
		, Category(InCategory)
		, Time(InTime)
		, Verbosity(InVerbosity)
	{}
};

/**
* Class used for output redirection to allow logs to show
*/
class CORE_API YOutputDeviceRedirector : public YOutputDevice
{
private:
	/** A FIFO of lines logged by non-master threads. */
	TArray<FBufferedLine> BufferedLines;

	/** A FIFO backlog of messages logged before the editor had a chance to intercept them. */
	TArray<FBufferedLine> BacklogLines;

	/** Array of output devices to redirect to */
	TArray<YOutputDevice*> OutputDevices;

	/** The master thread ID.  Logging from other threads will be buffered for processing by the master thread. */
	uint32 MasterThreadID;

	/** Whether backlogging is enabled. */
	bool bEnableBacklog;

	/** Object used for synchronization via a scoped lock */
	FCriticalSection	SynchronizationObject;

	/**
	* The unsynchronized version of FlushThreadedLogs.
	* Assumes that the caller holds a lock on SynchronizationObject.
	* @param bUseAllDevices - if true this method will use all output devices
	*/
	void UnsynchronizedFlushThreadedLogs(bool bUseAllDevices);

public:

	/** Initialization constructor. */
	YOutputDeviceRedirector();

	/**
	* Get the GLog singleton
	*/
	static YOutputDeviceRedirector* Get();

	/**
	* Adds an output device to the chain of redirections.
	*
	* @param OutputDevice	output device to add
	*/
	void AddOutputDevice(YOutputDevice* OutputDevice);

	/**
	* Removes an output device from the chain of redirections.
	*
	* @param OutputDevice	output device to remove
	*/
	void RemoveOutputDevice(YOutputDevice* OutputDevice);

	/**
	* Returns whether an output device is currently in the list of redirectors.
	*
	* @param	OutputDevice	output device to check the list against
	* @return	true if messages are currently redirected to the the passed in output device, false otherwise
	*/
	bool IsRedirectingTo(YOutputDevice* OutputDevice);

	/** Flushes lines buffered by secondary threads. */
	virtual void FlushThreadedLogs();

	/**
	*	Flushes lines buffered by secondary threads.
	*	Only used if a background thread crashed and we needed to push the callstack into the log.
	*/
	virtual void PanicFlushThreadedLogs();

	/**
	* Serializes the current backlog to the specified output device.
	* @param OutputDevice	- Output device that will receive the current backlog
	*/
	virtual void SerializeBacklog(YOutputDevice* OutputDevice);

	/**
	* Enables or disables the backlog.
	* @param bEnable	- Starts saving a backlog if true, disables and discards any backlog if false
	*/
	virtual void EnableBacklog(bool bEnable);

	/**
	* Sets the current thread to be the master thread that prints directly
	* (isn't queued up)
	*/
	virtual void SetCurrentThreadAsMasterThread();

	/**
	* Serializes the passed in data via all current output devices.
	*
	* @param	Data	Text to log
	* @param	Event	Event name used for suppression purposes
	*/
	virtual void Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category, const double Time) override;

	/**
	* Serializes the passed in data via all current output devices.
	*
	* @param	Data	Text to log
	* @param	Event	Event name used for suppression purposes
	*/
	virtual void Serialize(const TCHAR* Data, ELogVerbosity::Type Verbosity, const class YName& Category) override;

	/**
	* Passes on the flush request to all current output devices.
	*/
	void Flush() override;

	/**
	* Closes output device and cleans up. This can't happen in the destructor
	* as we might have to call "delete" which cannot be done for static/ global
	* objects.
	*/
	void TearDown() override;
};
