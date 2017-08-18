// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GenericPlatform/GenericPlatformMemory.h"
#include "HAL/PlatformMemory.h"
#include "Misc/AssertionMacros.h"
#include "Math/UnrealMathUtility.h"
#include "Containers/StringConv.h"
#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "Stats/Stats.h"
#include "Containers/Ticker.h"
#include "Misc/FeedbackContext.h"
#include "Async/Async.h"
#include "HAL/MallocAnsi.h"
#include "GenericPlatform/GenericPlatformMemoryPoolStats.h"
#include "HAL/MemoryMisc.h"
#include "Misc/CoreDelegates.h"


DEFINE_STAT(MCR_Physical);
DEFINE_STAT(MCR_GPU);
DEFINE_STAT(MCR_TexturePool);
DEFINE_STAT(MCR_StreamingPool);
DEFINE_STAT(MCR_UsedStreamingPool);

DEFINE_STAT(STAT_TotalPhysical);
DEFINE_STAT(STAT_TotalVirtual);
DEFINE_STAT(STAT_PageSize);
DEFINE_STAT(STAT_TotalPhysicalGB);

DEFINE_STAT(STAT_AvailablePhysical);
DEFINE_STAT(STAT_AvailableVirtual);
DEFINE_STAT(STAT_UsedPhysical);
DEFINE_STAT(STAT_PeakUsedPhysical);
DEFINE_STAT(STAT_UsedVirtual);
DEFINE_STAT(STAT_PeakUsedVirtual);

/** Helper class used to update platform memory stats. */
struct FGenericStatsUpdater
{
	/** Called once per second, enqueues stats update. */
	static bool EnqueueUpdateStats(float /*InDeltaTime*/)
	{
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, []()
		{
			DoUpdateStats();
		});
		return true; // Tick again
	}

	/** Gathers and sets all platform memory statistics into the corresponding stats. */
	static void DoUpdateStats()
	{
		// This is slow, so do it on the task graph.
		YPlatformMemoryStats MemoryStats = YPlatformMemory::GetStats();
		SET_MEMORY_STAT(STAT_TotalPhysical, MemoryStats.TotalPhysical);
		SET_MEMORY_STAT(STAT_TotalVirtual, MemoryStats.TotalVirtual);
		SET_MEMORY_STAT(STAT_PageSize, MemoryStats.PageSize);
		SET_MEMORY_STAT(STAT_TotalPhysicalGB, MemoryStats.TotalPhysicalGB);

		SET_MEMORY_STAT(STAT_AvailablePhysical, MemoryStats.AvailablePhysical);
		SET_MEMORY_STAT(STAT_AvailableVirtual, MemoryStats.AvailableVirtual);
		SET_MEMORY_STAT(STAT_UsedPhysical, MemoryStats.UsedPhysical);
		SET_MEMORY_STAT(STAT_PeakUsedPhysical, MemoryStats.PeakUsedPhysical);
		SET_MEMORY_STAT(STAT_UsedVirtual, MemoryStats.UsedVirtual);
		SET_MEMORY_STAT(STAT_PeakUsedVirtual, MemoryStats.PeakUsedVirtual);

		// Platform specific stats.
		YPlatformMemory::InternalUpdateStats(MemoryStats);
	}
};

YGenericPlatformMemoryStats::YGenericPlatformMemoryStats()
	: YGenericPlatformMemoryConstants(YPlatformMemory::GetConstants())
	, AvailablePhysical(0)
	, AvailableVirtual(0)
	, UsedPhysical(0)
	, PeakUsedPhysical(0)
	, UsedVirtual(0)
	, PeakUsedVirtual(0)
{}

bool YGenericPlatformMemory::bIsOOM = false;
uint64 YGenericPlatformMemory::OOMAllocationSize = 0;
uint32 YGenericPlatformMemory::OOMAllocationAlignment = 0;
YGenericPlatformMemory::EMemoryAllocatorToUse YGenericPlatformMemory::AllocatorToUse = Platform;
void* YGenericPlatformMemory::BackupOOMMemoryPool = nullptr;


void YGenericPlatformMemory::SetupMemoryPools()
{
	SET_MEMORY_STAT(MCR_Physical, 0); // "unlimited" physical memory, we still need to make this call to set the short name, etc
	SET_MEMORY_STAT(MCR_GPU, 0); // "unlimited" GPU memory, we still need to make this call to set the short name, etc
	SET_MEMORY_STAT(MCR_TexturePool, 0); // "unlimited" Texture memory, we still need to make this call to set the short name, etc
	SET_MEMORY_STAT(MCR_StreamingPool, 0);
	SET_MEMORY_STAT(MCR_UsedStreamingPool, 0);

	// if the platform chooses to have a BackupOOM pool, create it now
	if (YPlatformMemory::GetBackMemoryPoolSize() > 0)
	{
		BackupOOMMemoryPool = YPlatformMemory::BinnedAllocFromOS(YPlatformMemory::GetBackMemoryPoolSize());
	}
}

void YGenericPlatformMemory::Init()
{
	SetupMemoryPools();

#if	STATS
	// Stats are updated only once per second.
	const float PollingInterval = 1.0f;
	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateStatic(&FGenericStatsUpdater::EnqueueUpdateStats), PollingInterval);

	// Update for the first time.
	FGenericStatsUpdater::DoUpdateStats();
#endif // STATS
}

void YGenericPlatformMemory::OnOutOfMemory(uint64 Size, uint32 Alignment)
{
	// Update memory stats before we enter the crash handler.
	OOMAllocationSize = Size;
	OOMAllocationAlignment = Alignment;

	// only call this code one time - if already OOM, abort
	if (bIsOOM)
	{
		return;
	}
	bIsOOM = true;

	YPlatformMemoryStats PlatformMemoryStats = YPlatformMemory::GetStats();
	if (BackupOOMMemoryPool)
	{
		YPlatformMemory::BinnedFreeToOS(BackupOOMMemoryPool, YPlatformMemory::GetBackMemoryPoolSize());
		UE_LOG(LogMemory, Warning, TEXT("Freeing %d bytes from backup pool to handle out of memory."), YPlatformMemory::GetBackMemoryPoolSize());
	}

	UE_LOG(LogMemory, Warning, TEXT("MemoryStats:")\
		TEXT("\n\tAvailablePhysical %llu")\
		TEXT("\n\t AvailableVirtual %llu")\
		TEXT("\n\t     UsedPhysical %llu")\
		TEXT("\n\t PeakUsedPhysical %llu")\
		TEXT("\n\t      UsedVirtual %llu")\
		TEXT("\n\t  PeakUsedVirtual %llu"),
		(uint64)PlatformMemoryStats.AvailablePhysical,
		(uint64)PlatformMemoryStats.AvailableVirtual,
		(uint64)PlatformMemoryStats.UsedPhysical,
		(uint64)PlatformMemoryStats.PeakUsedPhysical,
		(uint64)PlatformMemoryStats.UsedVirtual,
		(uint64)PlatformMemoryStats.PeakUsedVirtual);
	if (GWarn)
	{
		GMalloc->DumpAllocatorStats(*GWarn);
	}

	// let any registered handlers go
	FCoreDelegates::OnOutOYMemory.Broadcast();

	UE_LOG(LogMemory, Fatal, TEXT("Ran out of memory allocating %llu bytes with alignment %u"), Size, Alignment);
}

FMalloc* YGenericPlatformMemory::BaseAllocator()
{
	return new FMallocAnsi();
}

YPlatformMemoryStats YGenericPlatformMemory::GetStats()
{
	UE_LOG(LogMemory, Warning, TEXT("YGenericPlatformMemory::GetStats not implemented on this platform"));
	return YPlatformMemoryStats();
}

void YGenericPlatformMemory::GetStatsForMallocProfiler(FGenericMemoryStats& out_Stats)
{
#if	STATS
	YPlatformMemoryStats Stats = YPlatformMemory::GetStats();

	// Base common stats for all platforms.
	out_Stats.Add(GET_STATDESCRIPTION(STAT_TotalPhysical), Stats.TotalPhysical);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_TotalVirtual), Stats.TotalVirtual);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_PageSize), Stats.PageSize);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_TotalPhysicalGB), (SIZE_T)Stats.TotalPhysicalGB);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_AvailablePhysical), Stats.AvailablePhysical);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_AvailableVirtual), Stats.AvailableVirtual);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_UsedPhysical), Stats.UsedPhysical);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_PeakUsedPhysical), Stats.PeakUsedPhysical);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_UsedVirtual), Stats.UsedVirtual);
	out_Stats.Add(GET_STATDESCRIPTION(STAT_PeakUsedVirtual), Stats.PeakUsedVirtual);
#endif // STATS
}

const YPlatformMemoryConstants& YGenericPlatformMemory::GetConstants()
{
	UE_LOG(LogMemory, Warning, TEXT("YGenericPlatformMemory::GetConstants not implemented on this platform"));
	static YPlatformMemoryConstants MemoryConstants;
	return MemoryConstants;
}

uint32 YGenericPlatformMemory::GetPhysicalGBRam()
{
	return YPlatformMemory::GetConstants().TotalPhysicalGB;
}

bool YGenericPlatformMemory::PageProtect(void* const Ptr, const SIZE_T Size, const bool bCanRead, const bool bCanWrite)
{
	UE_LOG(LogMemory, Verbose, TEXT("YGenericPlatformMemory::PageProtect not implemented on this platform"));
	return false;
}

void* YGenericPlatformMemory::BinnedAllocFromOS(SIZE_T Size)
{
	UE_LOG(LogMemory, Error, TEXT("YGenericPlatformMemory::BinnedAllocFromOS not implemented on this platform"));
	return nullptr;
}

void YGenericPlatformMemory::BinnedFreeToOS(void* Ptr, SIZE_T Size)
{
	UE_LOG(LogMemory, Error, TEXT("YGenericPlatformMemory::BinnedFreeToOS not implemented on this platform"));
}

void YGenericPlatformMemory::DumpStats(class FOutputDevice& Ar)
{
	const float InvMB = 1.0f / 1024.0f / 1024.0f;
	YPlatformMemoryStats MemoryStats = YPlatformMemory::GetStats();
#if !NO_LOGGING
	const FName CategorFName(LogMemory.GetCategorFName());
#else
	const FName CategorFName(TEXT("LogMemory"));
#endif
	Ar.CategorizedLogf(CategorFName, ELogVerbosity::Log, TEXT("Platform Memory Stats for %s"), ANSI_TO_TCHAR(FPlatformProperties::PlatformName()));
	Ar.CategorizedLogf(CategorFName, ELogVerbosity::Log, TEXT("Process Physical Memory: %.2f MB used, %.2f MB peak"), MemoryStats.UsedPhysical*InvMB, MemoryStats.PeakUsedPhysical*InvMB);
	Ar.CategorizedLogf(CategorFName, ELogVerbosity::Log, TEXT("Process Virtual Memory: %.2f MB used, %.2f MB peak"), MemoryStats.UsedVirtual*InvMB, MemoryStats.PeakUsedVirtual*InvMB);

	Ar.CategorizedLogf(CategorFName, ELogVerbosity::Log, TEXT("Physical Memory: %.2f MB used,  %.2f MB free, %.2f MB total"),
		(MemoryStats.TotalPhysical - MemoryStats.AvailablePhysical)*InvMB, MemoryStats.AvailablePhysical*InvMB, MemoryStats.TotalPhysical*InvMB);
	Ar.CategorizedLogf(CategorFName, ELogVerbosity::Log, TEXT("Virtual Memory: %.2f MB used,  %.2f MB free, %.2f MB total"),
		(MemoryStats.TotalVirtual - MemoryStats.AvailableVirtual)*InvMB, MemoryStats.AvailablePhysical*InvMB, MemoryStats.TotalVirtual*InvMB);

}

void YGenericPlatformMemory::DumpPlatformAndAllocatorStats(class FOutputDevice& Ar)
{
	YPlatformMemory::DumpStats(Ar);
	GMalloc->DumpAllocatorStats(Ar);
}

void YGenericPlatformMemory::MemswapGreaterThan8(void* RESTRICT Ptr1, void* RESTRICT Ptr2, SIZE_T Size)
{
	union PtrUnion
	{
		void*   PtrVoid;
		uint8*  Ptr8;
		uint16* Ptr16;
		uint32* Ptr32;
		uint64* Ptr64;
		UPTRINT PtrUint;
	};

	PtrUnion Union1 = { Ptr1 };
	PtrUnion Union2 = { Ptr2 };

	checkf(Union1.PtrVoid && Union2.PtrVoid, TEXT("Pointers must be non-null: %p, %p"), Union1.PtrVoid, Union2.PtrVoid);

	// We may skip up to 7 bytes below, so better make sure that we're swapping more than that
	// (8 is a common case that we also want to inline before we this call, so skip that too)
	check(Size > 8);

	if (Union1.PtrUint & 1)
	{
		Valswap(*Union1.Ptr8++, *Union2.Ptr8++);
		Size -= 1;
	}
	if (Union1.PtrUint & 2)
	{
		Valswap(*Union1.Ptr16++, *Union2.Ptr16++);
		Size -= 2;
	}
	if (Union1.PtrUint & 4)
	{
		Valswap(*Union1.Ptr32++, *Union2.Ptr32++);
		Size -= 4;
	}

	uint32 CommonAlignment = YMath::Min(YMath::CountTrailingZeros(Union1.PtrUint - Union2.PtrUint), 3u);
	switch (CommonAlignment)
	{
	default:
		for (; Size >= 8; Size -= 8)
		{
			Valswap(*Union1.Ptr64++, *Union2.Ptr64++);
		}

	case 2:
		for (; Size >= 4; Size -= 4)
		{
			Valswap(*Union1.Ptr32++, *Union2.Ptr32++);
		}

	case 1:
		for (; Size >= 2; Size -= 2)
		{
			Valswap(*Union1.Ptr16++, *Union2.Ptr16++);
		}

	case 0:
		for (; Size >= 1; Size -= 1)
		{
			Valswap(*Union1.Ptr8++, *Union2.Ptr8++);
		}
	}
}

YGenericPlatformMemory::YSharedMemoryRegion::YSharedMemoryRegion(const FString& InName, uint32 InAccessMode, void* InAddress, SIZE_T InSize)
	: AccessMode(InAccessMode)
	, Address(InAddress)
	, Size(InSize)
{
	FCString::Strcpy(Name, sizeof(Name) - 1, *InName);
}

YGenericPlatformMemory::YSharedMemoryRegion * YGenericPlatformMemory::MapNamedSharedMemoryRegion(const FString& Name, bool bCreate, uint32 AccessMode, SIZE_T Size)
{
	UE_LOG(LogHAL, Error, TEXT("YGenericPlatformMemory::MapNamedSharedMemoryRegion not implemented on this platform"));
	return NULL;
}

bool YGenericPlatformMemory::UnmapNamedSharedMemoryRegion(YSharedMemoryRegion * MemoryRegion)
{
	UE_LOG(LogHAL, Error, TEXT("YGenericPlatformMemory::UnmapNamedSharedMemoryRegion not implemented on this platform"));
	return false;
}


void YGenericPlatformMemory::InternalUpdateStats(const YPlatformMemoryStats& MemoryStats)
{
	// Generic method is empty. Implement at platform level.
}
