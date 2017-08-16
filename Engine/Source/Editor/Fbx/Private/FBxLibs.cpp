// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FbxLibs.h"
#include "FbxImporter.h"
//#include "FbxExporter.h"

//-------------------------------------------------------------------------
// Memory management callback functions used by the FBX SDK
//-------------------------------------------------------------------------
void* MyMalloc(size_t pSize)       
{
	return YMemory::Malloc(pSize);
}

void* MyCalloc(size_t pCount,size_t pSize)
{
	void* Alloc = YMemory::Malloc(pCount*pSize);
	return YMemory::Memzero(Alloc, pCount*pSize);
}

void* MyRealloc(void* pData, size_t pSize)
{
	return YMemory::Realloc(pData, pSize);
}

void  MyFree(void* pData)
{
	YMemory::Free(pData);
}


void LoadFBxLibraries()
{
	// Specify global memory handler callbacks to be used by the FBX SDK
	FbxSetMallocHandler(	&MyMalloc);
	FbxSetCallocHandler(	&MyCalloc);
	FbxSetReallocHandler(	&MyRealloc);
	FbxSetFreeHandler(		&MyFree);
}

void UnloadFBxLibraries()
{
	UnFbx::FFbxImporter::DeleteInstance();
	//UnFbx::FFbxExporter::DeleteInstance();

	// Hack: After we have freed our fbx sdk instance we need to set back to the default fbx memory handlers. 
	// This is required because there are some allocations made in the FBX dllmain before it is possible to set up our custom allocators
	// If this is not done, memory created by one allocator will be freed by another
	FbxSetMallocHandler(	FbxGetDefaultMallocHandler() );
	FbxSetCallocHandler(	FbxGetDefaultCallocHandler() );
	FbxSetReallocHandler(	FbxGetDefaultReallocHandler() );
	FbxSetFreeHandler(		FbxGetDefaultFreeHandler() );
}
