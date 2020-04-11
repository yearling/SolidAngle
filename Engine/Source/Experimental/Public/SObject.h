#pragma once
#include "Core.h"
class SObject
{
public:
	virtual ~SObject();
	static constexpr  bool IsInstance() 
	{
		return true;
	};
	FORCEINLINE uint32 AddRef() const
	{
		int32 NewValue = NumRefs.Increment();
		checkSlow(NewValue > 0);
		return uint32(NewValue);
	}
	FORCEINLINE uint32 Release() const
	{
		int32 NewValue = NumRefs.Decrement();
		if (NewValue == 0)
		{
			delete this;
		}
		checkSlow(NewValue >= 0);
		return uint32(NewValue);
	}
	FORCEINLINE_DEBUGGABLE uint32 GetRefCount() const
	{
		int32 CurrentValue = NumRefs.GetValue();
		checkSlow(CurrentValue >= 0);
		return uint32(CurrentValue);
	}
	SObject(const SObject&) = delete;
	SObject(SObject&&) = default;
	SObject& operator=(const SObject&) = delete;
	SObject& operator=(SObject&&) = default;
	virtual bool LoadFromPackage(const FString & Path);
	virtual void SaveToPackage(const FString & Path);
	virtual FArchive& Serialize(FArchive& Ar);
protected:
	SObject();
private:
	mutable FThreadSafeCounter NumRefs;
	friend class SObjectManager;
};
