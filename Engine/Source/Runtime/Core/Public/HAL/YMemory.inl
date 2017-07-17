// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#if !defined(YMemory_INLINE_FUNCTION_DECORATOR)
#define YMemory_INLINE_FUNCTION_DECORATOR 
#endif

#if !defined(YMemory_INLINE_GMalloc)
#define YMemory_INLINE_GMalloc GMalloc
#endif

struct YMemory;
struct FScopedMallocTimer;

YMemory_INLINE_FUNCTION_DECORATOR void* YMemory::Malloc(SIZE_T Count, uint32 Alignment)
{
	if (!YMemory_INLINE_GMalloc)
	{
		return MallocExternal(Count, Alignment);
	}
	DoGamethreadHook(0);
	FScopedMallocTimer Timer(0);
	return YMemory_INLINE_GMalloc->Malloc(Count, Alignment);
}

YMemory_INLINE_FUNCTION_DECORATOR void* YMemory::Realloc(void* Original, SIZE_T Count, uint32 Alignment)
{
	if (!YMemory_INLINE_GMalloc)
	{
		return ReallocExternal(Original, Count, Alignment);
	}
	DoGamethreadHook(1);
	FScopedMallocTimer Timer(1);
	return YMemory_INLINE_GMalloc->Realloc(Original, Count, Alignment);
}

YMemory_INLINE_FUNCTION_DECORATOR void YMemory::Free(void* Original)
{
	if (!Original)
	{
		FScopedMallocTimer Timer(3);
		return;
	}

	if (!YMemory_INLINE_GMalloc)
	{
		FreeExternal(Original);
		return;
	}
	DoGamethreadHook(2);
	FScopedMallocTimer Timer(2);
	YMemory_INLINE_GMalloc->Free(Original);
}

YMemory_INLINE_FUNCTION_DECORATOR SIZE_T YMemory::GetAllocSize(void* Original)
{
	if (!YMemory_INLINE_GMalloc)
	{
		return GetAllocSizeExternal(Original);
	}

	SIZE_T Size = 0;
	return YMemory_INLINE_GMalloc->GetAllocationSize(Original, Size) ? Size : 0;
}

YMemory_INLINE_FUNCTION_DECORATOR SIZE_T YMemory::QuantizeSize(SIZE_T Count, uint32 Alignment)
{
	if (!YMemory_INLINE_GMalloc)
	{
		return Count;
	}
	return YMemory_INLINE_GMalloc->QuantizeSize(Count, Alignment);
}

