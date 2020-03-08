#include "YRenderResource.h"
void YRenderResource::InitResource()
{
	if (!bInitialized)
	{
		InitDynamicRHI();
		InitRHI();
		FPlatformMisc::MemoryBarrier(); // there are some multithreaded reads of bInitialized
		bInitialized = true;
	}
}

void YRenderResource::ReleaseResource()
{
	if (bInitialized)
	{
		ReleaseRHI();
		ReleaseDynamicRHI();
	}
}

