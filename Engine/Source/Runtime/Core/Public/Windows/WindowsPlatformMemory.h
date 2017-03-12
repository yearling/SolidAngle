#pragma once
#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "WindowsSystemIncludes.h"

class YString;
class YMalloc;
struct YGenericMemoryStats;

/**
*	Windows implementation of the FGenericPlatformMemoryStats.
*	At this moment it's just the same as the FGenericPlatformMemoryStats.
*	Can be extended as shown in the following example.
*/
struct YPlatformMemoryStats
	: public YGenericPlatformMemoryStats
{
	/** Default constructor, clears all variables. */
	YPlatformMemoryStats()
		: YGenericPlatformMemoryStats()
		, WindowsSpecificMemoryStat(0)
	{ }

	/** Memory stat specific only for Windows. */
	SIZE_T WindowsSpecificMemoryStat;
};


/**
* Windows implementation of the memory OS functions
**/
struct CORE_API YWindowsPlatformMemory
	: public YGenericPlatformMemory
{
	enum EMemoryCounterRegion
	{
		MCR_Invalid, // not memory
		MCR_Physical, // main system memory
		MCR_GPU, // memory directly a GPU (graphics card, etc)
		MCR_GPUSystem, // system memory directly accessible by a GPU
		MCR_TexturePool, // presized texture pools
		MCR_StreamingPool, // amount of texture pool available for streaming.
		MCR_UsedStreamingPool, // amount of texture pool used for streaming.
		MCR_GPUDefragPool, // presized pool of memory that can be defragmented.
		MCR_SamplePlatformSpecifcMemoryRegion,
		MCR_MAX
	};

	/**
	* Windows representation of a shared memory region
	*/
	struct FWindowsSharedMemoryRegion : public YSharedMemoryRegion
	{
		/** Returns the handle to file mapping object. */
		Windows::HANDLE GetMapping() const { return Mapping; }

		FWindowsSharedMemoryRegion(const YString& InName, uint32 InAccessMode, void* InAddress, SIZE_T InSize, Windows::HANDLE InMapping)
			: YSharedMemoryRegion(InName, InAccessMode, InAddress, InSize)
			, Mapping(InMapping)
		{}

	protected:

		/** Handle of a file mapping object */
		Windows::HANDLE				Mapping;
	};

	//~ Begin FGenericPlatformMemory Interface
	static void Init();
	static uint32 GetBackMemoryPoolSize()
	{
		/**
		* Value determined by series of tests on Fortnite with limited process memory.
		* 26MB sufficed to report all test crashes, using 32MB to have some slack.
		* If this pool is too large, use the following values to determine proper size:
		* 2MB pool allowed to report 78% of crashes.
		* 6MB pool allowed to report 90% of crashes.
		*/
		return 32 * 1024 * 1024;
	}

	static class YMalloc* BaseAllocator();
	static YPlatformMemoryStats GetStats();
	static void GetStatsForMallocProfiler(YGenericMemoryStats& out_Stats);
	static const YPlatformMemoryConstants& GetConstants();
	static bool PageProtect(void* const Ptr, const SIZE_T Size, const bool bCanRead, const bool bCanWrite);
	static void* BinnedAllocFromOS(SIZE_T Size);
	static void BinnedFreeToOS(void* Ptr, SIZE_T Size);
	static YSharedMemoryRegion* MapNamedSharedMemoryRegion(const YString& InName, bool bCreate, uint32 AccessMode, SIZE_T Size);
	static bool UnmapNamedSharedMemoryRegion(YSharedMemoryRegion * MemoryRegion);
protected:
	friend struct FGenericStatsUpdater;

	static void InternalUpdateStats(const YPlatformMemoryStats& MemoryStats);
	//~ End FGenericPlatformMemory Interface
};


typedef YWindowsPlatformMemory YPlatformMemory;
