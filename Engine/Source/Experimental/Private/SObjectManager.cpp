#include "SObjectManager.h"
SObjectManager GSObjectManager;

SObjectManager::SObjectManager()
{

}

void SObjectManager::Destroy()
{
	int32 DestroyUnitfyCount = 0;
	int32 UnifyLeft = 0;
	do
	{
		UnifyLeft = 0;
		DestroyUnitfyCount = 0;
		for (TMap<FName, TRefCountPtr<SObject>>::TIterator iter(UnifyObjects); iter; ++iter)
		{
			if (iter->Value->GetRefCount() == 1)
			{
				iter.RemoveCurrent();
				DestroyUnitfyCount++;
			}
			else
			{
				UnifyLeft++;
			}
		}
	} while (DestroyUnitfyCount);
	check(!UnifyLeft);

	int32 DestroyInstanceCount = 0;
	int32 InstanceLeft = 0;
	do
	{
		DestroyInstanceCount = 0;
		InstanceLeft = 0;
		for (TSet<TRefCountPtr<SObject>>::TIterator iter(InstancedObjects); iter; ++iter)
		{
			if (iter->GetRefCount() == 1)
			{
				iter.RemoveCurrent();
				DestroyInstanceCount++;
			}
			else
			{
				InstanceLeft++;
			}
		}
	} while (DestroyInstanceCount);
	check(!InstanceLeft);
}

void SObjectManager::FrameDestory()
{
	for (TMap<FName, TRefCountPtr<SObject>>::TIterator iter(UnifyObjects); iter; ++iter)
	{
		if (iter->Value->GetRefCount() == 1)
		{
			iter.RemoveCurrent();
		}
	}

	for (TSet<TRefCountPtr<SObject>>::TIterator iter(InstancedObjects); iter; ++iter)
	{
		if (iter->GetRefCount() == 1)
		{
			iter.RemoveCurrent();
		}
	}
}
