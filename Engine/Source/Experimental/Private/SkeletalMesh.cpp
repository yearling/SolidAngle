// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	SkeletalMesh.cpp: Unreal skeletal mesh and animation implementation.
=============================================================================*/

#include "YSkeletalMesh.h"
#include "Serialization/CustomVersion.h"
#include "UObject/FrameworkObjectVersion.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/RenderingObjectVersion.h"
#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"
#include "SkeletalMeshTypes.h"

#if WITH_EDITOR
//#include "MeshUtilities.h"

#if WITH_APEX_CLOTHING
#include "ApexClothingUtils.h"
#endif

#endif // #if WITH_EDITOR

//#include "Interfaces/ITargetPlatform.h"

#if WITH_APEX
#include "PhysXIncludes.h"
#endif// #if WITH_APEX
#include "ProfilingDebugging/ResourceSize.h"

//#include "EditorFramework/AssetImportData.h"
//#include "Engine/SkeletalMeshSocket.h"
//#include "Components/BrushComponent.h"
//#include "Streaming/UVChannelDensity.h"

#define LOCTEXT_NAMESPACE "SkeltalMesh"

DEFINE_LOG_CATEGORY(LogSkeletalMesh);
DECLARE_CYCLE_STAT(TEXT("GetShadowShapes"), STAT_GetShadowShapes, STATGROUP_Anim);

// Custom serialization version for SkeletalMesh types
struct FSkeletalMeshCustomVersion
{
	enum Type
	{
		// Before any version changes were made
		BeforeCustomVersionWasAdded = 0,
		// Remove Chunks array in FStaticLODModel and combine with Sections array
		CombineSectionWithChunk = 1,
		// Remove FRigidSkinVertex and combine with FSoftSkinVertex array
		CombineSoftAndRigidVerts = 2,
		// Need to recalc max bone influences
		RecalcMaxBoneInfluences = 3,
		// Add NumVertices that can be accessed when stripping editor data
		SaveNumVertices = 4,
		// Regenerated clothing section shadow flags from source sections
		RegenerateClothingShadowFlags = 5,
		// Share color buffer structure with StaticMesh
		UseSharedColorBufferFormat = 6,
		// Use separate buffer for skin weights
		UseSeparateSkinWeightBuffer = 7,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FSkeletalMeshCustomVersion() {}
};

const FGuid FSkeletalMeshCustomVersion::GUID(0xD78A4A00, 0xE8584697, 0xBAA819B5, 0x487D46B4);
FCustomVersionRegistration GRegisterSkeletalMeshCustomVersion(FSkeletalMeshCustomVersion::GUID, FSkeletalMeshCustomVersion::LatestVersion, TEXT("SkeletalMeshVer"));

#if WITH_APEX_CLOTHING
/*-----------------------------------------------------------------------------
	utility functions for apex clothing 
-----------------------------------------------------------------------------*/

static apex::ClothingAsset* LoadApexClothingAssetFromBlob(const TArray<uint8>& Buffer)
{
	// Wrap this blob with the APEX read stream class
	physx::PxFileBuf* Stream = GApexSDK->createMemoryReadStream( Buffer.GetData(), Buffer.Num() );
	// Create an NvParameterized serializer
	NvParameterized::Serializer* Serializer = GApexSDK->createSerializer(NvParameterized::Serializer::NST_BINARY);
	// Deserialize into a DeserializedData buffer
	NvParameterized::Serializer::DeserializedData DeserializedData;
	Serializer->deserialize( *Stream, DeserializedData );
	apex::Asset* ApexAsset = NULL;
	if( DeserializedData.size() > 0 )
	{
		// The DeserializedData has something in it, so create an APEX asset from it
		ApexAsset = GApexSDK->createAsset( DeserializedData[0], NULL);
		// Make sure it's a Clothing asset
		if (ApexAsset 
			&& ApexAsset->getObjTypeID() != GApexModuleClothing->getModuleID()
			)
		{
			GPhysCommandHandler->DeferredRelease(ApexAsset);
			ApexAsset = NULL;
		}
	}

	apex::ClothingAsset* ApexClothingAsset = static_cast<apex::ClothingAsset*>(ApexAsset);
	// Release our temporary objects
	Serializer->release();
	GApexSDK->releaseMemoryReadStream( *Stream );

	return ApexClothingAsset;
}

static bool SaveApexClothingAssetToBlob(const apex::ClothingAsset *InAsset, TArray<uint8>& OutBuffer)
{
	bool bResult = false;
	uint32 Size = 0;
	// Get the NvParameterized data for our Clothing asset
	if( InAsset != NULL )
	{
		// Create an APEX write stream
		physx::PxFileBuf* Stream = GApexSDK->createMemoryWriteStream();
		// Create an NvParameterized serializer
		NvParameterized::Serializer* Serializer = GApexSDK->createSerializer(NvParameterized::Serializer::NST_BINARY);

		const NvParameterized::Interface* AssetParameterized = InAsset->getAssetNvParameterized();
		if( AssetParameterized != NULL )
		{
			// Serialize the data into the stream
			Serializer->serialize( *Stream, &AssetParameterized, 1 );
			// Read the stream data into our buffer for UE serialzation
			Size = Stream->getFileLength();
			OutBuffer.AddUninitialized( Size );
			Stream->read( OutBuffer.GetData(), Size );
			bResult = true;
		}

		// Release our temporary objects
		Serializer->release();
		Stream->release();
	}

	return bResult;
}

#endif//#if WITH_APEX_CLOTHING

////////////////////////////////////////////////
//SIZE_T FClothingAssetData::GetResourceSize() const
//{
//	return GetResourceSizeBytes();
//}
//
//void FClothingAssetData::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) const
//{
//#if WITH_APEX_CLOTHING
//	if (ApexClothingAsset)
//	{
//		physx::PxU32 LODLevel = ApexClothingAsset->getNumGraphicalLodLevels();
//		for(physx::PxU32 LODId=0; LODId < LODLevel; ++LODId)
//		{
//			if(const apex::RenderMeshAsset* RenderAsset = ApexClothingAsset->getRenderMeshAsset(LODId))
//			{
//				apex::RenderMeshAssetStats AssetStats;
//				RenderAsset->getStats(AssetStats);
//
//				CumulativeResourceSize.AddUnknownMemoryBytes(AssetStats.totalBytes);
//			}
//		}
//	}
//
//	CumulativeResourceSize.AddUnknownMemoryBytes(ClothCollisionVolumes.GetAllocatedSize());
//	CumulativeResourceSize.AddUnknownMemoryBytes(ClothCollisionConvexPlaneIndices.GetAllocatedSize());
//	CumulativeResourceSize.AddUnknownMemoryBytes(ClothCollisionVolumePlanes.GetAllocatedSize());
//	CumulativeResourceSize.AddUnknownMemoryBytes(ClothBoneSpheres.GetAllocatedSize());
//	CumulativeResourceSize.AddUnknownMemoryBytes(BoneSphereConnections.GetAllocatedSize());
//	CumulativeResourceSize.AddUnknownMemoryBytes(ClothVisualizationInfos.GetAllocatedSize());
//#endif // #if WITH_APEX_CLOTHING
//}
//
//SIZE_T FClothingAssetData::GetResourceSizeBytes() const
//{
//	FResourceSizeEx ResSize;
//	GetResourceSizeEx(ResSize);
//	return ResSize.GetTotalMemoryBytes();
//}

/*-----------------------------------------------------------------------------
	FSkeletalMeshVertexBuffer
-----------------------------------------------------------------------------*/

/**
* Constructor
*/
FSkeletalMeshVertexBuffer::FSkeletalMeshVertexBuffer() 
:	bUseFullPrecisionUVs(false)
,	bNeedsCPUAccess(false)
,	VertexData(nullptr)
,	Data(nullptr)
,	Stride(0)
,	NumVertices(0)
,	MeshOrigin(FVector::ZeroVector)
, 	MeshExtension(FVector(1.f,1.f,1.f))
{
}

/**
* Destructor
*/
FSkeletalMeshVertexBuffer::~FSkeletalMeshVertexBuffer()
{
	CleanUp();
}

/**
* Assignment. Assumes that vertex buffer will be rebuilt 
*/
FSkeletalMeshVertexBuffer& FSkeletalMeshVertexBuffer::operator=(const FSkeletalMeshVertexBuffer& Other)
{
	CleanUp();
	bUseFullPrecisionUVs = Other.bUseFullPrecisionUVs;
	bNeedsCPUAccess = Other.bNeedsCPUAccess;
	return *this;
}

/**
* Constructor (copy)
*/
FSkeletalMeshVertexBuffer::FSkeletalMeshVertexBuffer(const FSkeletalMeshVertexBuffer& Other)
:	bUseFullPrecisionUVs(Other.bUseFullPrecisionUVs)
,	bNeedsCPUAccess(Other.bNeedsCPUAccess)
,	VertexData(nullptr)
,	Data(nullptr)
,	Stride(0)
,	NumVertices(0)
,	MeshOrigin(Other.MeshOrigin)
, 	MeshExtension(Other.MeshExtension)
{
}

/**
 * @return text description for the resource type
 */
FString FSkeletalMeshVertexBuffer::GetFriendlyName() const
{ 
	return TEXT("Skeletal-mesh vertex buffer"); 
}

/** 
 * Delete existing resources 
 */
void FSkeletalMeshVertexBuffer::CleanUp()
{
	delete VertexData;
	VertexData = nullptr;
}

void FSkeletalMeshVertexBuffer::InitRHI()
{
	check(VertexData);
	FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
	if( ResourceArray->GetResourceDataSize() > 0 )
	{
		// Create the vertex buffer.
		//FRHIResourceCreateInfo CreateInfo(ResourceArray);
		
		// BUF_ShaderResource is needed for support of the SkinCache (we could make is dependent on GEnableGPUSkinCacheShaders or are there other users?)
		//VertexBufferRHI = RHICreateVertexBuffer( ResourceArray->GetResourceDataSize(), BUF_Static | BUF_ShaderResource, CreateInfo);
		//SRVValue = RHICreateShaderResourceView(VertexBufferRHI, 4, PF_R32_FLOAT);
	}
}

void FSkeletalMeshVertexBuffer::ReleaseRHI()
{
	//FVertexBuffer::ReleaseRHI();

	//SRVValue.SafeRelease();
}


/*-----------------------------------------------------------------------------
Serialized data stripping.
-----------------------------------------------------------------------------*/
FStripDataFlags::FStripDataFlags(class FArchive& Ar, uint8 InClassFlags /*= 0*/, int32 InVersion /*= VER_UE4_OLDEST_LOADABLE_PACKAGE */)
	: GlobalStripFlags(0)
	, ClassStripFlags(0)
{
	check(InVersion >= VER_UE4_OLDEST_LOADABLE_PACKAGE);
	if (Ar.UE4Ver() >= InVersion)
	{
		if (Ar.IsCooking())
		{
			// When cooking GlobalStripFlags are automatically generated based on the current target
			// platform's properties.
			//GlobalStripFlags |= Ar.CookingTarget()->HasEditorOnlyData() ? FStripDataFlags::None : FStripDataFlags::Editor;
			//GlobalStripFlags |= Ar.CookingTarget()->IsServerOnly() ? FStripDataFlags::Server : FStripDataFlags::None;
			ClassStripFlags = InClassFlags;
		}
		Ar << GlobalStripFlags;
		Ar << ClassStripFlags;
	}
}

FStripDataFlags::FStripDataFlags(class FArchive& Ar, uint8 InGlobalFlags, uint8 InClassFlags, int32 InVersion /*= VER_UE4_OLDEST_LOADABLE_PACKAGE */)
	: GlobalStripFlags(0)
	, ClassStripFlags(0)
{
	check(InVersion >= VER_UE4_OLDEST_LOADABLE_PACKAGE);
	if (Ar.UE4Ver() >= InVersion)
	{
		if (Ar.IsCooking())
		{
			// Don't generate global strip flags and use the ones passed in by the caller.
			GlobalStripFlags = InGlobalFlags;
			ClassStripFlags = InClassFlags;
		}
		Ar << GlobalStripFlags;
		Ar << ClassStripFlags;
	}
}

/**
* Serializer for this class
* @param Ar - archive to serialize to
* @param B - data to serialize
*/
FArchive& operator<<(FArchive& Ar,FSkeletalMeshVertexBuffer& VertexBuffer)
{
	FStripDataFlags StripFlags(Ar, 0, VER_UE4_STATIC_SKELETAL_MESH_SERIALIZATION_FIX);

	Ar << VertexBuffer.NumTexCoords;
	Ar << VertexBuffer.bUseFullPrecisionUVs;

	bool bBackCompatExtraBoneInfluences = false;

	if (Ar.UE4Ver() >= VER_UE4_SUPPORT_GPUSKINNING_8_BONE_INFLUENCES && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::UseSeparateSkinWeightBuffer)
	{
		Ar << bBackCompatExtraBoneInfluences;
	}

	// Serialize MeshExtension and Origin
	// I need to save them for console to pick it up later
	Ar << VertexBuffer.MeshExtension << VertexBuffer.MeshOrigin;

	if( Ar.IsLoading() )
	{
		// allocate vertex data on load
		VertexBuffer.AllocateData();
	}

	// if Ar is counting, it still should serialize. Need to count VertexData
	if (!StripFlags.IsDataStrippedForServer() || Ar.IsCountingMemory())
	{
		// Special handling for loading old content
		if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::UseSeparateSkinWeightBuffer)
		{
			int32 ElementSize;
			Ar << ElementSize;

			int32 ArrayNum;
			Ar << ArrayNum;

			TArray<uint8> DummyBytes;
			DummyBytes.AddUninitialized(ElementSize * ArrayNum);
			Ar.Serialize(DummyBytes.GetData(), ElementSize * ArrayNum);
		}
		else
		{
			if (VertexBuffer.VertexData != NULL)
			{
				VertexBuffer.VertexData->Serialize(Ar);

				// update cached buffer info
				VertexBuffer.NumVertices = VertexBuffer.VertexData->GetNumVertices();
				VertexBuffer.Data = (VertexBuffer.NumVertices > 0) ? VertexBuffer.VertexData->GetDataPointer() : nullptr;
				VertexBuffer.Stride = VertexBuffer.VertexData->GetStride();
			}
		}
	}

	return Ar;
}

/**
* Initializes the buffer with the given vertices.
* @param InVertices - The vertices to initialize the buffer with.
*/
void FSkeletalMeshVertexBuffer::Init(const TArray<FSoftSkinVertex>& InVertices)
{
	// Make sure if this is console, use compressed otherwise, use not compressed
	AllocateData();
	
	VertexData->ResizeBuffer(InVertices.Num());
	
	if (InVertices.Num() > 0)
	{
		Data = VertexData->GetDataPointer();
		Stride = VertexData->GetStride();
		NumVertices = VertexData->GetNumVertices();
	}
	
	for( int32 VertIdx=0; VertIdx < InVertices.Num(); VertIdx++ )
	{
		const FSoftSkinVertex& SrcVertex = InVertices[VertIdx];
		SetVertexFast(VertIdx,SrcVertex);
	}
}

void FSkeletalMeshVertexBuffer::SetNeedsCPUAccess(bool bInNeedsCPUAccess)
{
	bNeedsCPUAccess = bInNeedsCPUAccess;
}

// Handy macro for allocating the correct vertex data class (which has to be known at compile time) depending on the data type and number of UVs.  
#define ALLOCATE_VERTEX_DATA_TEMPLATE( VertexDataType, NumUVs )											\
	switch(NumUVs)																						\
	{																									\
		case 1: VertexData = new TSkeletalMeshVertexData< VertexDataType<1> >(bNeedsCPUAccess); break;	\
		case 2: VertexData = new TSkeletalMeshVertexData< VertexDataType<2> >(bNeedsCPUAccess); break;	\
		case 3: VertexData = new TSkeletalMeshVertexData< VertexDataType<3> >(bNeedsCPUAccess); break;	\
		case 4: VertexData = new TSkeletalMeshVertexData< VertexDataType<4> >(bNeedsCPUAccess); break;	\
		default: UE_LOG(LogSkeletalMesh, Fatal,TEXT("Invalid number of texture coordinates"));								\
	}																									\

/** 
* Allocates the vertex data storage type. 
*/
void FSkeletalMeshVertexBuffer::AllocateData()
{
	// Clear any old VertexData before allocating.
	CleanUp();

	if( !bUseFullPrecisionUVs )
	{
		ALLOCATE_VERTEX_DATA_TEMPLATE( TGPUSkinVertexFloat16Uvs, NumTexCoords );
	}
	else
	{
		ALLOCATE_VERTEX_DATA_TEMPLATE( TGPUSkinVertexFloat32Uvs, NumTexCoords );
	}
}

void FSkeletalMeshVertexBuffer::SetVertexFast(uint32 VertexIndex,const FSoftSkinVertex& SrcVertex)
{
	checkSlow(VertexIndex < GetNumVertices());
	auto* VertBase = (TGPUSkinVertexBase*)(Data + VertexIndex * Stride);
	VertBase->TangentX = SrcVertex.TangentX;
	VertBase->TangentZ = SrcVertex.TangentZ;
	// store the sign of the determinant in TangentZ.W
	VertBase->TangentZ.Vector.W = GetBasisDeterminantSignByte( SrcVertex.TangentX, SrcVertex.TangentY, SrcVertex.TangentZ );
	if( !bUseFullPrecisionUVs )
	{
		auto* Vertex = (TGPUSkinVertexFloat16Uvs<MAX_TEXCOORDS>*)VertBase;
		Vertex->Position = SrcVertex.Position;
		for( uint32 UVIndex = 0; UVIndex < NumTexCoords; ++UVIndex )
		{
			Vertex->UVs[UVIndex] = FVector2DHalf( SrcVertex.UVs[UVIndex] );
		}
	}
	else
	{
		auto* Vertex = (TGPUSkinVertexFloat32Uvs<MAX_TEXCOORDS>*)VertBase;
		Vertex->Position = SrcVertex.Position;
		for( uint32 UVIndex = 0; UVIndex < NumTexCoords; ++UVIndex )
		{
			Vertex->UVs[UVIndex] = FVector2D( SrcVertex.UVs[UVIndex] );
		}
	}
}

/**
* Convert the existing data in this mesh from 16 bit to 32 bit UVs.
* Without rebuilding the mesh (loss of precision)
*/
template<uint32 NumTexCoordsT>
void FSkeletalMeshVertexBuffer::ConvertToFullPrecisionUVsTyped()
{
	if( !bUseFullPrecisionUVs )
	{
		TArray< TGPUSkinVertexFloat32Uvs<NumTexCoordsT> > DestVertexData;
		TSkeletalMeshVertexData< TGPUSkinVertexFloat16Uvs<NumTexCoordsT> >& SrcVertexData = *(TSkeletalMeshVertexData< TGPUSkinVertexFloat16Uvs<NumTexCoordsT> >*)VertexData;			
		DestVertexData.AddUninitialized(SrcVertexData.Num());
		for( int32 VertIdx=0; VertIdx < SrcVertexData.Num(); VertIdx++ )
		{
			TGPUSkinVertexFloat16Uvs<NumTexCoordsT>& SrcVert = SrcVertexData[VertIdx];
			TGPUSkinVertexFloat32Uvs<NumTexCoordsT>& DestVert = DestVertexData[VertIdx];
			FMemory::Memcpy(&DestVert,&SrcVert,sizeof(TGPUSkinVertexBase));
			DestVert.Position = SrcVert.Position;
			for( uint32 UVIndex = 0; UVIndex < NumTexCoords; ++UVIndex )
			{
				DestVert.UVs[UVIndex] = FVector2D(SrcVert.UVs[UVIndex]);
			}

		}

		bUseFullPrecisionUVs = true;
		*this = DestVertexData;
	}
}


/*-----------------------------------------------------------------------------
FSkeletalMeshVertexAPEXClothBuffer
-----------------------------------------------------------------------------*/

/**
 * Constructor
 */
FSkeletalMeshVertexAPEXClothBuffer::FSkeletalMeshVertexAPEXClothBuffer() 
:	VertexData(nullptr),
	Data(nullptr),
	Stride(0),
	NumVertices(0)
{

}

/**
 * Destructor
 */
FSkeletalMeshVertexAPEXClothBuffer::~FSkeletalMeshVertexAPEXClothBuffer()
{
	// clean up everything
	CleanUp();
}

/**
 * Assignment. Assumes that vertex buffer will be rebuilt 
 */

FSkeletalMeshVertexAPEXClothBuffer& FSkeletalMeshVertexAPEXClothBuffer::operator=(const FSkeletalMeshVertexAPEXClothBuffer& Other)
{
	CleanUp();
	return *this;
}

/**
 * Copy Constructor
 */
FSkeletalMeshVertexAPEXClothBuffer::FSkeletalMeshVertexAPEXClothBuffer(const FSkeletalMeshVertexAPEXClothBuffer& Other)
:	VertexData(nullptr),
	Data(nullptr),
	Stride(0),
	NumVertices(0)
{

}

/**
 * @return text description for the resource type
 */
FString FSkeletalMeshVertexAPEXClothBuffer::GetFriendlyName() const
{
	return TEXT("Skeletal-mesh vertex APEX cloth mesh-mesh mapping buffer");
}

/** 
 * Delete existing resources 
 */
void FSkeletalMeshVertexAPEXClothBuffer::CleanUp()
{
	delete VertexData;
	VertexData = nullptr;
}

/**
 * Initialize the RHI resource for this vertex buffer
 */
void FSkeletalMeshVertexAPEXClothBuffer::InitRHI()
{
	check(VertexData);
	FResourceArrayInterface* ResourceArray = VertexData->GetResourceArray();
	if( ResourceArray->GetResourceDataSize() > 0 )
	{
		FRHIResourceCreateInfo CreateInfo(ResourceArray);
		VertexBufferRHI = RHICreateVertexBuffer( ResourceArray->GetResourceDataSize(), BUF_Static, CreateInfo);
	}
}

/**
 * Serializer for this class
 * @param Ar - archive to serialize to
 * @param B - data to serialize
 */
FArchive& operator<<( FArchive& Ar, FSkeletalMeshVertexAPEXClothBuffer& VertexBuffer )
{
	FStripDataFlags StripFlags(Ar, 0, VER_UE4_STATIC_SKELETAL_MESH_SERIALIZATION_FIX);

	if( Ar.IsLoading() )
	{
		VertexBuffer.AllocateData();
	}

	if (!StripFlags.IsDataStrippedForServer() || Ar.IsCountingMemory())
	{
		if( VertexBuffer.VertexData != NULL )
		{
			VertexBuffer.VertexData->Serialize( Ar );

			// update cached buffer info
			VertexBuffer.NumVertices = VertexBuffer.VertexData->GetNumVertices();
			VertexBuffer.Data = (VertexBuffer.NumVertices > 0) ? VertexBuffer.VertexData->GetDataPointer() : nullptr;
			VertexBuffer.Stride = VertexBuffer.VertexData->GetStride();
		}
	}

	return Ar;
}






/**
 * Initializes the buffer with the given vertices.
 * @param InVertices - The vertices to initialize the buffer with.
 */
void FSkeletalMeshVertexAPEXClothBuffer::Init(const TArray<FApexClothPhysToRenderVertData>& InMappingData)
{
	// Allocate new data
	AllocateData();

	// Resize the buffer to hold enough data for all passed in vertices
	VertexData->ResizeBuffer( InMappingData.Num() );

	Data = VertexData->GetDataPointer();
	Stride = VertexData->GetStride();
	NumVertices = VertexData->GetNumVertices();

	// Copy the vertices into the buffer.
	checkSlow(Stride*NumVertices == sizeof(FApexClothPhysToRenderVertData) * InMappingData.Num());
	//appMemcpy(Data, &InMappingData(0), Stride*NumVertices);
	for(int32 Index = 0;Index < InMappingData.Num();Index++)
	{
		const FApexClothPhysToRenderVertData& SourceMapping = InMappingData[Index];
		const int32 DestVertexIndex = Index;
		MappingData(DestVertexIndex) = SourceMapping;
	}
}

/** 
 * Allocates the vertex data storage type. 
 */
void FSkeletalMeshVertexAPEXClothBuffer::AllocateData()
{
	CleanUp();

	VertexData = new TSkeletalMeshVertexData<FApexClothPhysToRenderVertData>(true);
}


/*-----------------------------------------------------------------------------
FGPUSkinVertexBase
-----------------------------------------------------------------------------*/

/**
* Serializer
*
* @param Ar - archive to serialize with
*/
void TGPUSkinVertexBase::Serialize(FArchive& Ar)
{
	Ar << TangentX;
	Ar << TangentZ;
}

/*-----------------------------------------------------------------------------
	FSoftSkinVertex
-----------------------------------------------------------------------------*/

/**
* Serializer
*
* @param Ar - archive to serialize with
* @param V - vertex to serialize
* @return archive that was used
*/
FArchive& operator<<(FArchive& Ar,FSoftSkinVertex& V)
{
	Ar << V.Position;
	Ar << V.TangentX << V.TangentY << V.TangentZ;

	for( int32 UVIdx = 0; UVIdx < MAX_TEXCOORDS; ++UVIdx )
	{
		Ar << V.UVs[UVIdx];
	}

	Ar << V.Color;

	// serialize bone and weight uint8 arrays in order
	// this is required when serializing as bulk data memory (see TArray::BulkSerialize notes)
	for(uint32 InfluenceIndex = 0;InfluenceIndex < MAX_INFLUENCES_PER_STREAM;InfluenceIndex++)
	{
		Ar << V.InfluenceBones[InfluenceIndex];
	}

	if (Ar.UE4Ver() >= VER_UE4_SUPPORT_8_BONE_INFLUENCES_SKELETAL_MESHES)
	{
		for(uint32 InfluenceIndex = MAX_INFLUENCES_PER_STREAM;InfluenceIndex < MAX_TOTAL_INFLUENCES;InfluenceIndex++)
		{
			Ar << V.InfluenceBones[InfluenceIndex];
		}
	}
	else
	{
		if (Ar.IsLoading())
		{
			for(uint32 InfluenceIndex = MAX_INFLUENCES_PER_STREAM;InfluenceIndex < MAX_TOTAL_INFLUENCES;InfluenceIndex++)
			{
				V.InfluenceBones[InfluenceIndex] = 0;
			}
		}
	}

	for(uint32 InfluenceIndex = 0;InfluenceIndex < MAX_INFLUENCES_PER_STREAM;InfluenceIndex++)
	{
		Ar << V.InfluenceWeights[InfluenceIndex];
	}

	if (Ar.UE4Ver() >= VER_UE4_SUPPORT_8_BONE_INFLUENCES_SKELETAL_MESHES)
	{
		for(uint32 InfluenceIndex = MAX_INFLUENCES_PER_STREAM;InfluenceIndex < MAX_TOTAL_INFLUENCES;InfluenceIndex++)
		{
			Ar << V.InfluenceWeights[InfluenceIndex];
		}
	}
	else
	{
		if (Ar.IsLoading())
		{
			for(uint32 InfluenceIndex = MAX_INFLUENCES_PER_STREAM;InfluenceIndex < MAX_TOTAL_INFLUENCES;InfluenceIndex++)
			{
				V.InfluenceWeights[InfluenceIndex] = 0;
			}
		}
	}

	return Ar;
}

bool FSoftSkinVertex::GetRigidWeightBone(uint8& OutBoneIndex) const
{
	bool bIsRigid = false;

	for (int32 WeightIdx = 0; WeightIdx < MAX_TOTAL_INFLUENCES; WeightIdx++)
	{
		if (InfluenceWeights[WeightIdx] == 255)
		{
			bIsRigid = true;
			OutBoneIndex = InfluenceBones[WeightIdx];
			break;
		}
	}

	return bIsRigid;
}

/*-----------------------------------------------------------------------------
	FMultiSizeIndexBuffer
-------------------------------------------------------------------------------*/
FMultiSizeIndexContainer::~FMultiSizeIndexContainer()
{
	if (IndexBuffer)
	{
		delete IndexBuffer;
	}
}

/**
 * Initialize the index buffer's render resources.
 */
void FMultiSizeIndexContainer::InitResources()
{
	check(IsInGameThread());
	if( IndexBuffer )
	{
		BeginInitResource( IndexBuffer );
	}
}

/**
 * Releases the index buffer's render resources.
 */	
void FMultiSizeIndexContainer::ReleaseResources()
{
	check(IsInGameThread());
	if( IndexBuffer )
	{
		BeginReleaseResource( IndexBuffer );
	}
}

/**
 * Creates a new index buffer
 */
void FMultiSizeIndexContainer::CreateIndexBuffer(uint8 InDataTypeSize)
{
	check( IndexBuffer == NULL );
	bool bNeedsCPUAccess = true;

	DataTypeSize = InDataTypeSize;

	if (InDataTypeSize == sizeof(uint16))
	{
		IndexBuffer = new FRawStaticIndexBuffer16or32<uint16>(bNeedsCPUAccess);
	}
	else
	{
#if !DISALLOW_32BIT_INDICES
		IndexBuffer = new FRawStaticIndexBuffer16or32<uint32>(bNeedsCPUAccess);
#else
		UE_LOG(LogSkeletalMesh, Fatal, TEXT("When DISALLOW_32BIT_INDICES is defined, 32 bit indices should not be used") );
#endif
	}
}

/**
 * Repopulates the index buffer
 */
void FMultiSizeIndexContainer::RebuildIndexBuffer( const FMultiSizeIndexContainerData& InData )
{
	bool bNeedsCPUAccess = true;

	if( IndexBuffer )
	{
		delete IndexBuffer;
	}
	DataTypeSize = InData.DataTypeSize;

	if (DataTypeSize == sizeof(uint16))
	{
		IndexBuffer = new FRawStaticIndexBuffer16or32<uint16>(bNeedsCPUAccess);
	}
	else
	{
#if !DISALLOW_32BIT_INDICES
		IndexBuffer = new FRawStaticIndexBuffer16or32<uint32>(bNeedsCPUAccess);
#else
		UE_LOG(LogSkeletalMesh, Fatal, TEXT("When DISALLOW_32BIT_INDICES is defined, 32 bit indices should not be used") );
#endif
	}

	CopyIndexBuffer( InData.Indices );
}

/**
 * Returns a 32 bit version of the index buffer
 */
void FMultiSizeIndexContainer::GetIndexBuffer( TArray<uint32>& OutArray ) const
{
	check( IndexBuffer );

	OutArray.Reset();
	int32 NumIndices = IndexBuffer->Num();
	OutArray.AddUninitialized( NumIndices );

	for (int32 I = 0; I < NumIndices; ++I)
	{
		OutArray[I] = IndexBuffer->Get(I);
	}
}

/**
 * Populates the index buffer with a new set of indices
 */
void FMultiSizeIndexContainer::CopyIndexBuffer(const TArray<uint32>& NewArray)
{
	check( IndexBuffer );

	// On console the resource arrays can't have items added directly to them
	if (FPlatformProperties::HasEditorOnlyData() == false)
	{
		if (DataTypeSize == sizeof(uint16))
		{
			TArray<uint16> WordArray;
			for (int32 i = 0; i < NewArray.Num(); ++i)
			{
				WordArray.Add((uint16)NewArray[i]);
			}

			((FRawStaticIndexBuffer16or32<uint16>*)IndexBuffer)->AssignNewBuffer(WordArray);
		}
		else
		{
			((FRawStaticIndexBuffer16or32<uint32>*)IndexBuffer)->AssignNewBuffer(NewArray);
		}
	}
	else
	{
		IndexBuffer->Empty();
		for (int32 i = 0; i < NewArray.Num(); ++i)
		{
#if WITH_EDITOR
			if(DataTypeSize == sizeof(uint16) && NewArray[i] > MAX_uint16)
			{
				UE_LOG(LogSkeletalMesh, Warning, TEXT("Attempting to copy %u into a uint16 index buffer - this value will overflow to %u, use RebuildIndexBuffer to create a uint32 index buffer!"), NewArray[i], (uint16)NewArray[i]);
			}
#endif
			IndexBuffer->AddItem(NewArray[i]);
		}
	}
}

void FMultiSizeIndexContainer::Serialize(FArchive& Ar, bool bNeedsCPUAccess)
{
	DECLARE_SCOPE_CYCLE_COUNTER( TEXT("FMultiSizeIndexContainer::Serialize"), STAT_MultiSizeIndexContainer_Serialize, STATGROUP_LoadTime );
	if (Ar.UE4Ver() < VER_UE4_KEEP_SKEL_MESH_INDEX_DATA)
	{
		bool bOldNeedsCPUAccess = true;
		Ar << bOldNeedsCPUAccess;
	}
	Ar << DataTypeSize;

	if (!IndexBuffer)
	{
		if (DataTypeSize == sizeof(uint16))
		{
			IndexBuffer = new FRawStaticIndexBuffer16or32<uint16>(bNeedsCPUAccess);
		}
		else
		{
#if !DISALLOW_32BIT_INDICES
			IndexBuffer = new FRawStaticIndexBuffer16or32<uint32>(bNeedsCPUAccess);
#else
			UE_LOG(LogSkeletalMesh, Fatal, TEXT("When DISALLOW_32BIT_INDICES is defined, 32 bit indices should not be used") );
#endif
		}
	}
	
	IndexBuffer->Serialize( Ar );
}

#if WITH_EDITOR
/**
 * Retrieves index buffer related data
 */
void FMultiSizeIndexContainer::GetIndexBufferData( FMultiSizeIndexContainerData& OutData ) const
{
	OutData.DataTypeSize = DataTypeSize;
	GetIndexBuffer( OutData.Indices );
}

FMultiSizeIndexContainer::FMultiSizeIndexContainer(const FMultiSizeIndexContainer& Other)
: DataTypeSize(sizeof(uint16))
, IndexBuffer(nullptr)
{
	// Cant copy this index buffer, assumes it will be rebuilt later
	IndexBuffer = nullptr;
}

FMultiSizeIndexContainer& FMultiSizeIndexContainer::operator=(const FMultiSizeIndexContainer& Buffer)
{
	// Cant copy this index buffer.  Delete the index buffer type.
	// assumes it will be rebuilt later
	if( IndexBuffer )
	{
		delete IndexBuffer;
		IndexBuffer = nullptr;
	}

	return *this;
}
#endif


/*-----------------------------------------------------------------------------
	FSkelMeshSection
-----------------------------------------------------------------------------*/

// Custom serialization version for RecomputeTangent
struct FRecomputeTangentCustomVersion
{
	enum Type
	{
		// Before any version changes were made in the plugin
		BeforeCustomVersionWasAdded = 0,
		// We serialize the RecomputeTangent Option
		RuntimeRecomputeTangent = 1,
		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FRecomputeTangentCustomVersion() {}
};

const FGuid FRecomputeTangentCustomVersion::GUID(0x5579F886, 0x933A4C1F, 0x83BA087B, 0x6361B92F);
// Register the custom version with core
FCustomVersionRegistration GRegisterRecomputeTangentCustomVersion(FRecomputeTangentCustomVersion::GUID, FRecomputeTangentCustomVersion::LatestVersion, TEXT("RecomputeTangentCustomVer"));

/** Legacy 'rigid' skin vertex */
struct FLegacyRigidSkinVertex
{
	FVector			Position;
	FPackedNormal	TangentX,	// Tangent, U-direction
		TangentY,	// Binormal, V-direction
		TangentZ;	// Normal
	FVector2D		UVs[MAX_TEXCOORDS]; // UVs
	FColor			Color;		// Vertex color.
	uint8			Bone;

	friend FArchive& operator<<(FArchive& Ar, FLegacyRigidSkinVertex& V)
	{
		Ar << V.Position;
		Ar << V.TangentX << V.TangentY << V.TangentZ;

		for (int32 UVIdx = 0; UVIdx < MAX_TEXCOORDS; ++UVIdx)
		{
			Ar << V.UVs[UVIdx];
		}

		Ar << V.Color;
		Ar << V.Bone;

		return Ar;
	}

	/** Util to convert from legacy */
	void ConvertToSoftVert(FSoftSkinVertex& DestVertex)
	{
		DestVertex.Position = Position;
		DestVertex.TangentX = TangentX;
		DestVertex.TangentY = TangentY;
		DestVertex.TangentZ = TangentZ;
		// store the sign of the determinant in TangentZ.W
		DestVertex.TangentZ.Vector.W = GetBasisDeterminantSignByte(TangentX, TangentY, TangentZ);

		// copy all texture coordinate sets
		FMemory::Memcpy(DestVertex.UVs, UVs, sizeof(FVector2D)*MAX_TEXCOORDS);

		DestVertex.Color = Color;
		DestVertex.InfluenceBones[0] = Bone;
		DestVertex.InfluenceWeights[0] = 255;
		for (int32 InfluenceIndex = 1; InfluenceIndex < MAX_TOTAL_INFLUENCES; InfluenceIndex++)
		{
			DestVertex.InfluenceBones[InfluenceIndex] = 0;
			DestVertex.InfluenceWeights[InfluenceIndex] = 0;
		}
	}
};



// Serialization.
FArchive& operator<<(FArchive& Ar,FSkelMeshSection& S)
{
	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);

	// When data is cooked for server platform some of the
	// variables are not serialized so that they're always
	// set to their initial values (for safety)
	FStripDataFlags StripFlags( Ar );

	Ar << S.MaterialIndex;

	Ar.UsingCustomVersion(FSkeletalMeshCustomVersion::GUID);
	if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSectionWithChunk)
	{
		uint16 DummyChunkIndex;
		Ar << DummyChunkIndex;
	}

	if (!StripFlags.IsDataStrippedForServer())
	{
		Ar << S.BaseIndex;
	}
		
	if (!StripFlags.IsDataStrippedForServer())
	{
		Ar << S.NumTriangles;
	}
		
	Ar << S.TriangleSorting;

	// for clothing info
	if( Ar.UE4Ver() >= VER_UE4_APEX_CLOTH )
	{
		Ar << S.bDisabled;
		Ar << S.CorrespondClothSectionIndex;
	}

	if( Ar.UE4Ver() >= VER_UE4_APEX_CLOTH_LOD )
	{
		Ar << S.bEnableClothLOD_DEPRECATED;
	}

	Ar.UsingCustomVersion(FRecomputeTangentCustomVersion::GUID);
	if (Ar.CustomVer(FRecomputeTangentCustomVersion::GUID) >= FRecomputeTangentCustomVersion::RuntimeRecomputeTangent)
	{
		Ar << S.bRecomputeTangent;
	}

	if (Ar.CustomVer(FEditorObjectVersion::GUID) >= FEditorObjectVersion::RefactorMeshEditorMaterials)
	{
		Ar << S.bCastShadow;
	}
	else
	{
		S.bCastShadow = true;
	}

	if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) >= FSkeletalMeshCustomVersion::CombineSectionWithChunk)
	{

		if (!StripFlags.IsDataStrippedForServer())
		{
			// This is so that BaseVertexIndex is never set to anything else that 0 (for safety)
			Ar << S.BaseVertexIndex;
		}

		if (!StripFlags.IsEditorDataStripped())
		{
			// For backwards compat, read rigid vert array into array
			TArray<FLegacyRigidSkinVertex> LegacyRigidVertices;
			if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
			{
				Ar << LegacyRigidVertices;
			}

			Ar << S.SoftVertices;

			// Once we have read in SoftVertices, convert and insert legacy rigid verts (if present) at start
			const int32 NumRigidVerts = LegacyRigidVertices.Num();
			if (NumRigidVerts > 0 && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
			{
				S.SoftVertices.InsertUninitialized(0, NumRigidVerts);

				for (int32 VertIdx = 0; VertIdx < NumRigidVerts; VertIdx++)
				{
					LegacyRigidVertices[VertIdx].ConvertToSoftVert(S.SoftVertices[VertIdx]);
				}
			}
		}

		// If loading content newer than CombineSectionWithChunk but older than SaveNumVertices, update NumVertices here
		if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::SaveNumVertices)
		{
			if (!StripFlags.IsDataStrippedForServer())
			{
				S.NumVertices = S.SoftVertices.Num();
			}
			else
			{
				UE_LOG(LogSkeletalMesh, Warning, TEXT("Cannot set FSkelMeshSection::NumVertices for older content, loading in non-editor build."));
				S.NumVertices = 0;
			}
		}

		Ar << S.BoneMap;

		if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) >= FSkeletalMeshCustomVersion::SaveNumVertices)
		{
			Ar << S.NumVertices;
		}

		// Removed NumRigidVertices and NumSoftVertices
		if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
		{
			int32 DummyNumRigidVerts, DummyNumSoftVerts;
			Ar << DummyNumRigidVerts;
			Ar << DummyNumSoftVerts;

			if (DummyNumRigidVerts + DummyNumSoftVerts != S.SoftVertices.Num())
			{
				UE_LOG(LogSkeletalMesh, Error, TEXT("Legacy NumSoftVerts + NumRigidVerts != SoftVertices.Num()"));
			}
		}

		Ar << S.MaxBoneInfluences;

#if WITH_EDITOR
		// If loading content where we need to recalc 'max bone influences' instead of using loaded version, do that now
		if (!StripFlags.IsEditorDataStripped() && Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::RecalcMaxBoneInfluences)
		{
			S.CalcMaxBoneInfluences();
		}
#endif

		Ar << S.ApexClothMappingData;
		Ar << S.PhysicalMeshVertices;
		Ar << S.PhysicalMeshNormals;
		Ar << S.CorrespondClothAssetIndex;
		Ar << S.ClothAssetSubmeshIndex;
	}

	return Ar;
}

void FMorphTargetVertexInfoBuffers::InitRHI()
{
	if (PerVertexInfoList.Num() > 0)
	{
		FRHIResourceCreateInfo CreateInfo;
		void* PerVertexInfoListVBData = nullptr;
		PerVertexInfoVB = RHICreateAndLockVertexBuffer(PerVertexInfoList.GetAllocatedSize(), BUF_Static | BUF_ShaderResource, CreateInfo, PerVertexInfoListVBData);
		FMemory::Memcpy(PerVertexInfoListVBData, PerVertexInfoList.GetData(), PerVertexInfoList.GetAllocatedSize());
		RHIUnlockVertexBuffer(PerVertexInfoVB);
		PerVertexInfoSRV = RHICreateShaderResourceView(PerVertexInfoVB, sizeof(uint32), PF_R32_UINT);

		void* FlattenedDeltasVBData = nullptr;
		FlattenedDeltasVB = RHICreateAndLockVertexBuffer(FlattenedDeltaList.GetAllocatedSize(), BUF_Static | BUF_ShaderResource, CreateInfo, FlattenedDeltasVBData);
		FMemory::Memcpy(FlattenedDeltasVBData, FlattenedDeltaList.GetData(), FlattenedDeltaList.GetAllocatedSize());
		RHIUnlockVertexBuffer(FlattenedDeltasVB);
		FlattenedDeltasSRV = RHICreateShaderResourceView(FlattenedDeltasVB, sizeof(uint32), PF_R32_UINT);

		NumInfluencedVerticesByMorphs = (uint32)PerVertexInfoList.Num();

		PerVertexInfoList.Empty();
		FlattenedDeltaList.Empty();
	}
}

void FMorphTargetVertexInfoBuffers::ReleaseRHI()
{
	PerVertexInfoVB.SafeRelease();
	PerVertexInfoSRV.SafeRelease();
	FlattenedDeltasVB.SafeRelease();
	FlattenedDeltasSRV.SafeRelease();
}

/*-----------------------------------------------------------------------------
	FStaticLODModel
-----------------------------------------------------------------------------*/

/** Legacy Chunk struct, now merged with FSkelMeshSection */
struct FLegacySkelMeshChunk
{
	uint32 BaseVertexIndex;
	TArray<FSoftSkinVertex> SoftVertices;
	TArray<FApexClothPhysToRenderVertData> ApexClothMappingData;
	TArray<FVector> PhysicalMeshVertices;
	TArray<FVector> PhysicalMeshNormals;
	TArray<FBoneIndexType> BoneMap;
	int32 MaxBoneInfluences;

	int16 CorrespondClothAssetIndex;
	int16 ClothAssetSubmeshIndex;

	FLegacySkelMeshChunk()
		: BaseVertexIndex(0)
		, MaxBoneInfluences(4)
		, CorrespondClothAssetIndex(INDEX_NONE)
		, ClothAssetSubmeshIndex(INDEX_NONE)
	{}

	void CopyToSection(FSkelMeshSection& Section)
	{
		Section.BaseVertexIndex = BaseVertexIndex;
		Section.SoftVertices = SoftVertices;
		Section.ApexClothMappingData = ApexClothMappingData;
		Section.PhysicalMeshVertices = PhysicalMeshVertices;
		Section.PhysicalMeshNormals = PhysicalMeshNormals;
		Section.BoneMap = BoneMap;
		Section.MaxBoneInfluences = MaxBoneInfluences;
		Section.CorrespondClothAssetIndex = CorrespondClothAssetIndex;
		Section.ClothAssetSubmeshIndex = ClothAssetSubmeshIndex;
	}


	friend FArchive& operator<<(FArchive& Ar, FLegacySkelMeshChunk& C)
	{
		FStripDataFlags StripFlags(Ar);

		if (!StripFlags.IsDataStrippedForServer())
		{
			// This is so that BaseVertexIndex is never set to anything else that 0 (for safety)
			Ar << C.BaseVertexIndex;
		}
		if (!StripFlags.IsEditorDataStripped())
		{
			// For backwards compat, read rigid vert array into array
			TArray<FLegacyRigidSkinVertex> LegacyRigidVertices;
			if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
			{
				Ar << LegacyRigidVertices;
			}

			Ar << C.SoftVertices;

			// Once we have read in SoftVertices, convert and insert legacy rigid verts (if present) at start
			const int32 NumRigidVerts = LegacyRigidVertices.Num();
			if (NumRigidVerts > 0 && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
			{
				C.SoftVertices.InsertUninitialized(0, NumRigidVerts);

				for (int32 VertIdx = 0; VertIdx < NumRigidVerts; VertIdx++)
				{
					LegacyRigidVertices[VertIdx].ConvertToSoftVert(C.SoftVertices[VertIdx]);
				}
			}
		}
		Ar << C.BoneMap;

		// Removed NumRigidVertices and NumSoftVertices, just use array size
		if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSoftAndRigidVerts)
		{
			int32 DummyNumRigidVerts, DummyNumSoftVerts;
			Ar << DummyNumRigidVerts;
			Ar << DummyNumSoftVerts;

			if (DummyNumRigidVerts + DummyNumSoftVerts != C.SoftVertices.Num())
			{
				UE_LOG(LogSkeletalMesh, Error, TEXT("Legacy NumSoftVerts + NumRigidVerts != SoftVertices.Num()"));
			}
		}

		Ar << C.MaxBoneInfluences;


		if (Ar.UE4Ver() >= VER_UE4_APEX_CLOTH)
		{
			Ar << C.ApexClothMappingData;
			Ar << C.PhysicalMeshVertices;
			Ar << C.PhysicalMeshNormals;
			Ar << C.CorrespondClothAssetIndex;
			Ar << C.ClothAssetSubmeshIndex;
		}

		return Ar;
	}
};

void FStaticLODModel::Serialize( FArchive& Ar, UObject* Owner, int32 Idx )
{
	DECLARE_SCOPE_CYCLE_COUNTER( TEXT("FStaticLODModel::Serialize"), STAT_StaticLODModel_Serialize, STATGROUP_LoadTime );

	const uint8 LodAdjacencyStripFlag = 1;
	//FStripDataFlags StripFlags( Ar, Ar.IsCooking() && !Ar.CookingTarget()->SupportsFeature(ETargetPlatformFeatures::Tessellation) ? LodAdjacencyStripFlag : 0 );
	FStripDataFlags StripFlags( Ar, LodAdjacencyStripFlag );

	// Skeletal mesh buffers are kept in CPU memory after initialization to support merging of skeletal meshes.
	bool bKeepBuffersInCPUMemory = true;
	static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FreeSkeletalMeshBuffers"));
	if(CVar)
	{
		bKeepBuffersInCPUMemory = !CVar->GetValueOnAnyThread();
	}

	if (StripFlags.IsDataStrippedForServer())
	{
		TArray<FSkelMeshSection> TempSections;
		Ar << TempSections;

		FMultiSizeIndexContainer	TempMultiSizeIndexContainer;
		TempMultiSizeIndexContainer.Serialize(Ar, bKeepBuffersInCPUMemory);

		TArray<FBoneIndexType> TempActiveBoneIndices;
		Ar << TempActiveBoneIndices;
	}
	else
	{
		Ar << Sections;
		MultiSizeIndexContainer.Serialize(Ar, bKeepBuffersInCPUMemory);
		Ar << ActiveBoneIndices;
	}

	// Array of Sections for backwards compat
	Ar.UsingCustomVersion(FSkeletalMeshCustomVersion::GUID);
	if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::CombineSectionWithChunk)
	{
		TArray<FLegacySkelMeshChunk> LegacyChunks;

		Ar << LegacyChunks;

		check(LegacyChunks.Num() == Sections.Num());
		for (int32 ChunkIdx = 0; ChunkIdx < LegacyChunks.Num(); ChunkIdx++)
		{
			FSkelMeshSection& Section = Sections[ChunkIdx];

			LegacyChunks[ChunkIdx].CopyToSection(Section);

			// Set NumVertices for older content on load
			if (!StripFlags.IsDataStrippedForServer())
			{
				Section.NumVertices = Section.SoftVertices.Num();
			}
			else
			{
				UE_LOG(LogSkeletalMesh, Warning, TEXT("Cannot set FSkelMeshSection::NumVertices for older content, loading in non-editor build."));
				Section.NumVertices = 0;
			}
		}
	}
	
	// no longer in use
	{
		uint32 LegacySize = 0;
		Ar << LegacySize;
	}

	if (!StripFlags.IsDataStrippedForServer())
	{
		Ar << NumVertices;
	}
	Ar << RequiredBones;

	if( !StripFlags.IsEditorDataStripped() )
	{
		RawPointIndices.Serialize( Ar, Owner );
	}

	if (StripFlags.IsDataStrippedForServer())
	{
		TArray<int32> TempMeshToImportVertexMap;
		Ar << TempMeshToImportVertexMap;

		int32 TempMaxImportVertex;
		Ar << TempMaxImportVertex;
	}
	else
	{
		Ar << MeshToImportVertexMap;
		Ar << MaxImportVertex;
	}

	if( !StripFlags.IsDataStrippedForServer() )
	{
		//YSkeletalMesh* SkelMeshOwner = CastChecked<YSkeletalMesh>(Owner);
		YSkeletalMesh* SkelMeshOwner = nullptr;

		if( Ar.IsLoading() )
		{
			// set cpu skinning flag on the vertex buffer so that the resource arrays know if they need to be CPU accessible
			//bool bNeedsCPUAccess = bKeepBuffersInCPUMemory || SkelMeshOwner->GetImportedResource()->RequiresCPUSkinning(GMaxRHIFeatureLevel);
			bool bNeedsCPUAccess = false;
			VertexBufferGPUSkin.SetNeedsCPUAccess(bNeedsCPUAccess);
			SkinWeightVertexBuffer.SetNeedsCPUAccess(bNeedsCPUAccess);
		}
		Ar << NumTexCoords;
		Ar << VertexBufferGPUSkin;

		if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) >= FSkeletalMeshCustomVersion::UseSeparateSkinWeightBuffer)
		{
			Ar << SkinWeightVertexBuffer;
		}

		//if( SkelMeshOwner->bHasVertexColors)
		//{
		//	// Handling for old color buffer data
		//	if (Ar.IsLoading() && Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::UseSharedColorBufferFormat)
		//	{
		//		TArray<FColor> OldColors;
		//		FStripDataFlags LegacyColourStripFlags(Ar, 0, VER_UE4_STATIC_SKELETAL_MESH_SERIALIZATION_FIX);
		//		OldColors.BulkSerialize(Ar);
		//	}
		//	else
		//	{
		//		ColorVertexBuffer.Serialize(Ar, bKeepBuffersInCPUMemory);
		//	}
		//}

		if ( !StripFlags.IsClassDataStripped( LodAdjacencyStripFlag ) )
		{
			AdjacencyMultiSizeIndexContainer.Serialize(Ar,bKeepBuffersInCPUMemory);
		}

		if ( Ar.UE4Ver() >= VER_UE4_APEX_CLOTH && HasApexClothData() )
		{
			Ar << APEXClothVertexBuffer;
		}

		// validate sections and reset incorrect sorting mode
		if ( Ar.IsLoading() )
		{
			const int32 kNumIndicesPerPrimitive = 3;
			const int32 kNumSetsOfIndices = 2;
			for (int32 IdxSection = 0, PreLastSection = Sections.Num() - 1; IdxSection < PreLastSection; ++IdxSection)
			{
				FSkelMeshSection & Section = Sections[ IdxSection ];
				if (TRISORT_CustomLeftRight == Section.TriangleSorting)
				{
					uint32 IndicesInSection = Sections[ IdxSection + 1 ].BaseIndex - Section.BaseIndex;
					if (Section.NumTriangles * kNumIndicesPerPrimitive * kNumSetsOfIndices > IndicesInSection)
					{
					/*	UE_LOG(LogSkeletalMesh, Warning, TEXT( "Section %d in LOD model %d of object %s doesn't have enough indices (%d, while %d are needed) to allow TRISORT_CustomLeftRight mode, resetting to TRISORT_None" ),
							IdxSection, Idx, *Owner->GetName(),
							IndicesInSection, Section.NumTriangles * kNumIndicesPerPrimitive * kNumSetsOfIndices
							);*/
						Section.TriangleSorting = TRISORT_None;
					}
				}
			}
			if (Sections.Num() > 0)
			{
				// last section is special case
				FSkelMeshSection & Section = Sections[Sections.Num() - 1];
				if (TRISORT_CustomLeftRight == Section.TriangleSorting)
				{
					uint32 IndicesInSection = MultiSizeIndexContainer.GetIndexBuffer()->Num() - Sections[Sections.Num() - 1].BaseIndex;
					if (Section.NumTriangles * kNumIndicesPerPrimitive * kNumSetsOfIndices > IndicesInSection)
					{
					/*	UE_LOG(LogSkeletalMesh, Warning, TEXT("Section %d in LOD model %d of object %s doesn't have enough indices (%d, while %d are needed) to allow TRISORT_CustomLeftRight mode, resetting to TRISORT_None"),
							Sections.Num() - 1, Idx, *Owner->GetName(),
							IndicesInSection, Section.NumTriangles * kNumIndicesPerPrimitive * kNumSetsOfIndices
							);*/
						Section.TriangleSorting = TRISORT_None;
					}
				}
			}
		}
	}
}

void FStaticLODModel::InitResources(bool bNeedsVertexColors, int32 LODIndex, TArray<UMorphTarget*>& InMorphTargets)
{
	//INC_DWORD_STAT_BY( STAT_SkeletalMeshIndexMemory, MultiSizeIndexContainer.IsIndexBufferValid() ? (MultiSizeIndexContainer.GetIndexBuffer()->Num() * MultiSizeIndexContainer.GetDataTypeSize()) : 0 );
	
	MultiSizeIndexContainer.InitResources();

	//INC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, VertexBufferGPUSkin.GetVertexDataSize() );
	BeginInitResource(&VertexBufferGPUSkin);

	//INC_DWORD_STAT_BY(STAT_SkeletalMeshVertexMemory, SkinWeightVertexBuffer.GetVertexDataSize());
	BeginInitResource(&SkinWeightVertexBuffer);

	if( bNeedsVertexColors )
	{	
		// Only init the color buffer if the mesh has vertex colors
		//INC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, ColorVertexBuffer.GetAllocatedSize() );
		//BeginInitResource(&ColorVertexBuffer);
	}

	if ( HasApexClothData() )
	{
		// Only init the color buffer if the mesh has vertex colors
		//INC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, APEXClothVertexBuffer.GetVertexDataSize() );
		BeginInitResource(&APEXClothVertexBuffer);
	}

	if( RHISupportsTessellation(GMaxRHIShaderPlatform) ) 
	{
		AdjacencyMultiSizeIndexContainer.InitResources();
		//INC_DWORD_STAT_BY( STAT_SkeletalMeshIndexMemory, AdjacencyMultiSizeIndexContainer.IsIndexBufferValid() ? (AdjacencyMultiSizeIndexContainer.GetIndexBuffer()->Num() * AdjacencyMultiSizeIndexContainer.GetDataTypeSize()) : 0 );
	}

	if (RHISupportsComputeShaders(GMaxRHIShaderPlatform) && InMorphTargets.Num() > 0)
	{
		// Populate the arrays to be filled in later in the render thread

		// Auxiliary mapping for affected vertex indices by morph to its weights
		TMap<uint32, TArray<FMorphTargetVertexInfoBuffers::FFlattenedDelta>> AuxDeltaList;

		int32 TotalNumDeltas = 0;
		for (int32 AnimIdx = 0; AnimIdx < InMorphTargets.Num(); ++AnimIdx)
		{
			UMorphTarget* MorphTarget = InMorphTargets[AnimIdx];

			int32 NumSrcDeltas = 0;
			/*	FMorphTargetDelta* SrcDelta = MorphTarget->GetMorphTargetDelta(LODIndex, NumSrcDeltas);
				for (int32 SrcDeltaIndex = 0; SrcDeltaIndex < NumSrcDeltas; ++SrcDeltaIndex, ++SrcDelta)
				{
					TArray<FMorphTargetVertexInfoBuffers::FFlattenedDelta>& FlattenedDeltas = AuxDeltaList.FindOrAdd(SrcDelta->SourceIdx);
					FMorphTargetVertexInfoBuffers::FFlattenedDelta* NewDelta = new(FlattenedDeltas) FMorphTargetVertexInfoBuffers::FFlattenedDelta;
					NewDelta->PosDelta = SrcDelta->PositionDelta;
					NewDelta->TangentDelta = SrcDelta->TangentZDelta;
					NewDelta->WeightIndex = AnimIdx;
					++TotalNumDeltas;
				}*/
		}

		MorphTargetVertexInfoBuffers.FlattenedDeltaList.Empty(TotalNumDeltas);
		MorphTargetVertexInfoBuffers.PerVertexInfoList.AddUninitialized(AuxDeltaList.Num());
		int32 StartDelta = 0;
		FMorphTargetVertexInfoBuffers::FPerVertexInfo* NewPerVertexInfo = MorphTargetVertexInfoBuffers.PerVertexInfoList.GetData();
		for (auto& Pair : AuxDeltaList)
		{
			NewPerVertexInfo->DestVertexIndex = Pair.Key;
			NewPerVertexInfo->StartDelta = StartDelta;
			NewPerVertexInfo->NumDeltas = Pair.Value.Num();
			MorphTargetVertexInfoBuffers.FlattenedDeltaList.Append(Pair.Value);
			StartDelta += Pair.Value.Num();
			++NewPerVertexInfo;
		}

		if (AuxDeltaList.Num() > 0)
		{
			BeginInitResource(&MorphTargetVertexInfoBuffers);
		}
	}
}

void FStaticLODModel::ReleaseResources()
{
	/*DEC_DWORD_STAT_BY( STAT_SkeletalMeshIndexMemory, MultiSizeIndexContainer.IsIndexBufferValid() ? (MultiSizeIndexContainer.GetIndexBuffer()->Num() * MultiSizeIndexContainer.GetDataTypeSize()) : 0 );
	DEC_DWORD_STAT_BY( STAT_SkeletalMeshIndexMemory, AdjacencyMultiSizeIndexContainer.IsIndexBufferValid() ? (AdjacencyMultiSizeIndexContainer.GetIndexBuffer()->Num() * AdjacencyMultiSizeIndexContainer.GetDataTypeSize()) : 0 );
	DEC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, VertexBufferGPUSkin.GetVertexDataSize());
	DEC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, SkinWeightVertexBuffer.GetVertexDataSize());
	DEC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, ColorVertexBuffer.GetAllocatedSize() );
	DEC_DWORD_STAT_BY( STAT_SkeletalMeshVertexMemory, APEXClothVertexBuffer.GetVertexDataSize() );
*/
	MultiSizeIndexContainer.ReleaseResources();
	AdjacencyMultiSizeIndexContainer.ReleaseResources();

	BeginReleaseResource(&VertexBufferGPUSkin);
	BeginReleaseResource(&SkinWeightVertexBuffer);
	//BeginReleaseResource(&ColorVertexBuffer);
	BeginReleaseResource(&APEXClothVertexBuffer);
	BeginReleaseResource(&MorphTargetVertexInfoBuffers);
}

int32 FStaticLODModel::GetTotalFaces() const
{
	int32 TotalFaces = 0;
	for(int32 i=0; i<Sections.Num(); i++)
	{
		TotalFaces += Sections[i].NumTriangles;
	}

	return TotalFaces;
}

void FStaticLODModel::GetSectionFromVertexIndex(int32 InVertIndex, int32& OutSectionIndex, int32& OutVertIndex, bool& bOutHasExtraBoneInfluences) const
{
	OutSectionIndex = 0;
	OutVertIndex = 0;
	bOutHasExtraBoneInfluences = false;

	int32 VertCount = 0;

	// Iterate over each chunk
	for(int32 SectionCount = 0; SectionCount < Sections.Num(); SectionCount++)
	{
		const FSkelMeshSection& Section = Sections[SectionCount];
		OutSectionIndex = SectionCount;

		// Is it in Soft vertex range?
		if(InVertIndex < VertCount + Section.GetNumVertices())
		{
			OutVertIndex = InVertIndex - VertCount;
			bOutHasExtraBoneInfluences = SkinWeightVertexBuffer.HasExtraBoneInfluences();
			return;
		}
		VertCount += Section.GetNumVertices();
	}

	// InVertIndex should always be in some chunk!
	//check(false);
}

void FStaticLODModel::GetVertices(TArray<FSoftSkinVertex>& Vertices) const
{
	Vertices.Empty(NumVertices);
	Vertices.AddUninitialized(NumVertices);
		
	// Initialize the vertex data
	// All chunks are combined into one (rigid first, soft next)
	FSoftSkinVertex* DestVertex = (FSoftSkinVertex*)Vertices.GetData();
	for(int32 SectionIndex = 0; SectionIndex < Sections.Num(); SectionIndex++)
	{
		const FSkelMeshSection& Section = Sections[SectionIndex];
		FMemory::Memcpy(DestVertex, Section.SoftVertices.GetData(), Section.SoftVertices.Num() * sizeof(FSoftSkinVertex));
		DestVertex += Section.SoftVertices.Num();
	}
}

void FStaticLODModel::GetApexClothMappingData(TArray<FApexClothPhysToRenderVertData>& MappingData) const
{
	for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); SectionIndex++)
	{
		const FSkelMeshSection& Section = Sections[SectionIndex];

		if(Section.ApexClothMappingData.Num() == 0 )
		{
			int32 PrevNum = MappingData.Num();

			MappingData.AddZeroed(Section.SoftVertices.Num());
			for(int32 i=PrevNum; i<MappingData.Num(); i++)
			{
				MappingData[i].PositionBaryCoordsAndDist[0] = 1.0f;
				MappingData[i].NormalBaryCoordsAndDist[0] = 1.0f;
				MappingData[i].TangentBaryCoordsAndDist[0] = 1.0f;
				// set max number to verify this is not the clothing section
				MappingData[i].SimulMeshVertIndices[0] = 0xFFFF;
			}
		}
		else
		{
			MappingData += Section.ApexClothMappingData;
		}
	}
}

void FStaticLODModel::BuildVertexBuffers(uint32 BuildFlags)
{
	bool bUseFullPrecisionUVs = (BuildFlags & EVertexFlags::UseFullPrecisionUVs) != 0;
	bool bHasVertexColors = (BuildFlags & EVertexFlags::HasVertexColors) != 0;

	TArray<FSoftSkinVertex> Vertices;
	GetVertices(Vertices);

	// match UV precision for mesh vertex buffer to setting from parent mesh
	VertexBufferGPUSkin.SetUseFullPrecisionUVs(bUseFullPrecisionUVs);
	// keep the buffer in CPU memory
	VertexBufferGPUSkin.SetNeedsCPUAccess(true);
	// Set the number of texture coordinate sets
	VertexBufferGPUSkin.SetNumTexCoords( NumTexCoords );
	// init vertex buffer with the vertex array
	VertexBufferGPUSkin.Init(Vertices);

	// Init skin weight buffer
	SkinWeightVertexBuffer.SetNeedsCPUAccess(true);
	SkinWeightVertexBuffer.SetHasExtraBoneInfluences(DoSectionsNeedExtraBoneInfluences());
	SkinWeightVertexBuffer.Init(Vertices);

	// Init the color buffer if this mesh has vertex colors.
	if( bHasVertexColors && Vertices.Num() > 0 )
	{
		ColorVertexBuffer.InitFromColorArray(&Vertices[0].Color, Vertices.Num(), sizeof(FSoftSkinVertex));
	}

	if( HasApexClothData() )
	{
		TArray<FApexClothPhysToRenderVertData> MappingData;
		GetApexClothMappingData(MappingData);
		APEXClothVertexBuffer.Init(MappingData);
	}
}

void FStaticLODModel::SortTriangles( FVector SortCenter, bool bUseSortCenter, int32 SectionIndex, ETriangleSortOption NewTriangleSorting )
{
#if WITH_EDITOR
	FSkelMeshSection& Section = Sections[SectionIndex];
	if( NewTriangleSorting == Section.TriangleSorting )
	{
		return;
	}

	if( NewTriangleSorting == TRISORT_CustomLeftRight )
	{
		// Make a second copy of index buffer data for this section
		int32 NumNewIndices = Section.NumTriangles*3;
		MultiSizeIndexContainer.GetIndexBuffer()->Insert(Section.BaseIndex, NumNewIndices);
		FMemory::Memcpy( MultiSizeIndexContainer.GetIndexBuffer()->GetPointerTo(Section.BaseIndex), MultiSizeIndexContainer.GetIndexBuffer()->GetPointerTo(Section.BaseIndex+NumNewIndices), NumNewIndices*MultiSizeIndexContainer.GetDataTypeSize() );

		// Fix up BaseIndex for indices in other sections
		for( int32 OtherSectionIdx = 0; OtherSectionIdx < Sections.Num(); OtherSectionIdx++ )
		{
			if( Sections[OtherSectionIdx].BaseIndex > Section.BaseIndex )
			{
				Sections[OtherSectionIdx].BaseIndex += NumNewIndices;
			}
		}
	}
	else if( Section.TriangleSorting == TRISORT_CustomLeftRight )
	{
		// Remove the second copy of index buffer data for this section
		int32 NumRemovedIndices = Section.NumTriangles*3;
		MultiSizeIndexContainer.GetIndexBuffer()->Remove(Section.BaseIndex, NumRemovedIndices);
		// Fix up BaseIndex for indices in other sections
		for( int32 OtherSectionIdx = 0; OtherSectionIdx < Sections.Num(); OtherSectionIdx++ )
		{
			if( Sections[OtherSectionIdx].BaseIndex > Section.BaseIndex )
			{
				Sections[OtherSectionIdx].BaseIndex -= NumRemovedIndices;
			}
		}
	}

	TArray<FSoftSkinVertex> Vertices;
	GetVertices(Vertices);

	switch( NewTriangleSorting )
	{
	case TRISORT_None:
		{
			TArray<uint32> Indices;
			MultiSizeIndexContainer.GetIndexBuffer( Indices );
			//SortTriangles_None( Section.NumTriangles, Vertices.GetData(), Indices.GetData() + Section.BaseIndex );
			MultiSizeIndexContainer.CopyIndexBuffer( Indices );
		}
		break;
	case TRISORT_CenterRadialDistance:
		{
			TArray<uint32> Indices;
			MultiSizeIndexContainer.GetIndexBuffer( Indices );
			if (bUseSortCenter)
			{
				//SortTriangles_CenterRadialDistance( SortCenter, Section.NumTriangles, Vertices.GetData(), Indices.GetData() + Section.BaseIndex );
			}
			else
			{
				//SortTriangles_CenterRadialDistance( Section.NumTriangles, Vertices.GetData(), Indices.GetData() + Section.BaseIndex );
			}
			MultiSizeIndexContainer.CopyIndexBuffer( Indices );
		}
		break;
	case TRISORT_Random:
		{
			TArray<uint32> Indices;
			MultiSizeIndexContainer.GetIndexBuffer( Indices );
			//SortTriangles_Random( Section.NumTriangles, Vertices.GetData(), Indices.GetData() + Section.BaseIndex );
			MultiSizeIndexContainer.CopyIndexBuffer( Indices );
		}
		break;
	case TRISORT_MergeContiguous:
		{
			TArray<uint32> Indices;
			MultiSizeIndexContainer.GetIndexBuffer( Indices );
			//SortTriangles_MergeContiguous( Section.NumTriangles, NumVertices, Vertices.GetData(), Indices.GetData() + Section.BaseIndex );
			MultiSizeIndexContainer.CopyIndexBuffer( Indices );
		}
		break;
	case TRISORT_Custom:
	case TRISORT_CustomLeftRight:
		break;
	}

	Section.TriangleSorting = NewTriangleSorting;
#endif
}

void FStaticLODModel::ReleaseCPUResources()
{	
	if(!GIsEditor && !IsRunningCommandlet())
	{
		if(MultiSizeIndexContainer.IsIndexBufferValid())
		{
			MultiSizeIndexContainer.GetIndexBuffer()->Empty();
		}
		if(AdjacencyMultiSizeIndexContainer.IsIndexBufferValid())
		{
			AdjacencyMultiSizeIndexContainer.GetIndexBuffer()->Empty();
		}
		if(VertexBufferGPUSkin.IsVertexDataValid())
		{
			VertexBufferGPUSkin.CleanUp();
		}
		if (SkinWeightVertexBuffer.IsWeightDataValid())
		{
			SkinWeightVertexBuffer.CleanUp();
		}
	}
}

SIZE_T FStaticLODModel::GetResourceSize() const
{
	return GetResourceSizeBytes();
}

void FStaticLODModel::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) const
{
	CumulativeResourceSize.AddUnknownMemoryBytes(Sections.GetAllocatedSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(ActiveBoneIndices.GetAllocatedSize()); 
	CumulativeResourceSize.AddUnknownMemoryBytes(RequiredBones.GetAllocatedSize());

	if(MultiSizeIndexContainer.IsIndexBufferValid())
	{
		const FRawStaticIndexBuffer16or32Interface* IndexBuffer = MultiSizeIndexContainer.GetIndexBuffer();
		if (IndexBuffer)
		{
			CumulativeResourceSize.AddUnknownMemoryBytes(IndexBuffer->GetResourceDataSize()); 
		}
	}

	if(AdjacencyMultiSizeIndexContainer.IsIndexBufferValid())
	{
		const FRawStaticIndexBuffer16or32Interface* AdjacentIndexBuffer = AdjacencyMultiSizeIndexContainer.GetIndexBuffer();
		if(AdjacentIndexBuffer)
		{
			CumulativeResourceSize.AddUnknownMemoryBytes(AdjacentIndexBuffer->GetResourceDataSize());
		}
	}

	CumulativeResourceSize.AddUnknownMemoryBytes(VertexBufferGPUSkin.GetVertexDataSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(SkinWeightVertexBuffer.GetVertexDataSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(ColorVertexBuffer.GetAllocatedSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(APEXClothVertexBuffer.GetVertexDataSize());

	CumulativeResourceSize.AddUnknownMemoryBytes(RawPointIndices.GetBulkDataSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(LegacyRawPointIndices.GetBulkDataSize());
	CumulativeResourceSize.AddUnknownMemoryBytes(MeshToImportVertexMap.GetAllocatedSize());

	// I suppose we add everything we could
	CumulativeResourceSize.AddUnknownMemoryBytes(sizeof(int32));
}

SIZE_T FStaticLODModel::GetResourceSizeBytes() const
{
	FResourceSizeEx ResSize;
	GetResourceSizeEx(ResSize);
	return ResSize.GetTotalMemoryBytes();
}

void FStaticLODModel::RebuildIndexBuffer(FMultiSizeIndexContainerData* IndexBufferData, FMultiSizeIndexContainerData* AdjacencyIndexBufferData)
{
	if (IndexBufferData)
	{
		MultiSizeIndexContainer.RebuildIndexBuffer(*IndexBufferData);
	}

	if (AdjacencyIndexBufferData)
	{
		AdjacencyMultiSizeIndexContainer.RebuildIndexBuffer(*AdjacencyIndexBufferData);
	}
}

#if WITH_EDITOR
void FStaticLODModel::RebuildIndexBuffer()
{
	// The index buffer needs to be rebuilt on copy.
	FMultiSizeIndexContainerData IndexBufferData;
	MultiSizeIndexContainer.GetIndexBufferData(IndexBufferData);

	FMultiSizeIndexContainerData AdjacencyIndexBufferData;
	AdjacencyMultiSizeIndexContainer.GetIndexBufferData(AdjacencyIndexBufferData);

	RebuildIndexBuffer(&IndexBufferData, &AdjacencyIndexBufferData);
}
#endif
/*-----------------------------------------------------------------------------
FStaticMeshSourceData
-----------------------------------------------------------------------------*/

/**
 * FSkeletalMeshSourceData - Source triangles and render data, editor-only.
 */
class FSkeletalMeshSourceData
{
public:
	FSkeletalMeshSourceData();
	~FSkeletalMeshSourceData();

#if WITH_EDITOR
	/** Initialize from static mesh render data. */
	void Init( const class YSkeletalMesh* SkeletalMesh, FStaticLODModel& LODModel );

	/** Retrieve render data. */
	FORCEINLINE FStaticLODModel* GetModel() { return LODModel; }
#endif // #if WITH_EDITOR

#if WITH_EDITORONLY_DATA
	/** Free source data. */
	void Clear();
#endif // WITH_EDITORONLY_DATA

	/** Returns true if the source data has been initialized. */
	FORCEINLINE bool IsInitialized() const { return LODModel != NULL; }

	/** Serialization. */
	void Serialize( FArchive& Ar, YSkeletalMesh* SkeletalMesh );

private:
	FStaticLODModel* LODModel;
};


FSkeletalMeshSourceData::FSkeletalMeshSourceData() : LODModel(nullptr)
{
}

FSkeletalMeshSourceData::~FSkeletalMeshSourceData()
{
	delete LODModel;
	LODModel = nullptr;
}

#if WITH_EDITOR

/** Initialize from static mesh render data. */
void FSkeletalMeshSourceData::Init( const YSkeletalMesh* SkeletalMesh, FStaticLODModel& InLODModel )
{
	check( LODModel == NULL );

	/** Bulk data arrays need to be locked before a copy can be made. */
	InLODModel.RawPointIndices.Lock( LOCK_READ_ONLY );
	InLODModel.LegacyRawPointIndices.Lock( LOCK_READ_ONLY );

	/** Allocate a new LOD model to hold the data and copy everything over. */
	LODModel = new FStaticLODModel();
	*LODModel = InLODModel;

	/** Unlock the arrays as the copy has been made. */
	InLODModel.RawPointIndices.Unlock();
	InLODModel.LegacyRawPointIndices.Unlock();

	/** The index buffer needs to be rebuilt on copy. */
	FMultiSizeIndexContainerData IndexBufferData, AdjacencyIndexBufferData;
	InLODModel.MultiSizeIndexContainer.GetIndexBufferData( IndexBufferData );
	InLODModel.AdjacencyMultiSizeIndexContainer.GetIndexBufferData(AdjacencyIndexBufferData);
	LODModel->RebuildIndexBuffer( &IndexBufferData, &AdjacencyIndexBufferData );

	/** Vertex buffers also need to be rebuilt. Source data is always stored with full precision position data. */
	//LODModel->BuildVertexBuffers(SkeletalMesh->GetVertexBufferFlags());
}

#endif // #if WITH_EDITOR

#if WITH_EDITORONLY_DATA

/** Free source data. */
void FSkeletalMeshSourceData::Clear()
{
	delete LODModel;
	LODModel = nullptr;
}

#endif // #if WITH_EDITORONLY_DATA

/** Serialization. */
void FSkeletalMeshSourceData::Serialize( FArchive& Ar, YSkeletalMesh* SkeletalMesh )
{
	DECLARE_SCOPE_CYCLE_COUNTER( TEXT("FSkeletalMeshSourceData::Serialize"), STAT_SkeletalMeshSourceData_Serialize, STATGROUP_LoadTime );

	if ( Ar.IsLoading() )
	{
		bool bHaveSourceData = false;
		Ar << bHaveSourceData;
		if ( bHaveSourceData )
		{
			if (LODModel != nullptr)
			{
				delete LODModel;
				LODModel = nullptr;
			}
			LODModel = new FStaticLODModel();
			//LODModel->Serialize( Ar, SkeletalMesh, INDEX_NONE );
		}
	}
	else
	{
		bool bHaveSourceData = IsInitialized();
		Ar << bHaveSourceData;
		if ( bHaveSourceData )
		{
			//LODModel->Serialize( Ar, SkeletalMesh, INDEX_NONE );
		}
	}
}



/*-----------------------------------------------------------------------------
FreeSkeletalMeshBuffersSinkCallback
-----------------------------------------------------------------------------*/

void FreeSkeletalMeshBuffersSinkCallback()
{
	// If r.FreeSkeletalMeshBuffers==1 then CPU buffer copies are to be released.
	static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FreeSkeletalMeshBuffers"));
	bool bFreeSkeletalMeshBuffers = CVar->GetValueOnGameThread() == 1;
	if(bFreeSkeletalMeshBuffers)
	{
		FlushRenderingCommands();
	/*	for (TObjectIterator<YSkeletalMesh> It;It;++It)
		{
			if (!It->GetImportedResource()->RequiresCPUSkinning(GMaxRHIFeatureLevel))
			{
				It->ReleaseCPUResources();
			}
		}*/
	}
}

/*-----------------------------------------------------------------------------
USkeletalMesh
-----------------------------------------------------------------------------*/

/**
* Calculate max # of bone influences used by this skel mesh chunk
*/
void FSkelMeshSection::CalcMaxBoneInfluences()
{
	// if we only have rigid verts then there is only one bone
	MaxBoneInfluences = 1;
	// iterate over all the soft vertices for this chunk and find max # of bones used
	for( int32 VertIdx=0; VertIdx < SoftVertices.Num(); VertIdx++ )
	{
		FSoftSkinVertex& SoftVert = SoftVertices[VertIdx];

		// calc # of bones used by this soft skinned vertex
		int32 BonesUsed=0;
		for( int32 InfluenceIdx=0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++ )
		{
			if( SoftVert.InfluenceWeights[InfluenceIdx] > 0 )
			{
				BonesUsed++;
			}
		}
		// reorder bones so that there aren't any unused influence entries within the [0,BonesUsed] range
		for( int32 InfluenceIdx=0; InfluenceIdx < BonesUsed; InfluenceIdx++ )
		{
			if( SoftVert.InfluenceWeights[InfluenceIdx] == 0 )
			{
				for( int32 ExchangeIdx=InfluenceIdx+1; ExchangeIdx < MAX_TOTAL_INFLUENCES; ExchangeIdx++ )
				{
					if( SoftVert.InfluenceWeights[ExchangeIdx] != 0 )
					{
						Exchange(SoftVert.InfluenceWeights[InfluenceIdx],SoftVert.InfluenceWeights[ExchangeIdx]);
						Exchange(SoftVert.InfluenceBones[InfluenceIdx],SoftVert.InfluenceBones[ExchangeIdx]);
						break;
					}
				}
			}
		}

		// maintain max bones used
		MaxBoneInfluences = FMath::Max(MaxBoneInfluences,BonesUsed);			
	}
}

/*-----------------------------------------------------------------------------
	FClothingAssetData
-----------------------------------------------------------------------------*/

//FArchive& operator<<(FArchive& Ar, FClothingAssetData& A)
//{
//	// Serialization to load and save ApexClothingAsset
//	if( Ar.IsLoading() )
//	{
//		uint32 AssetSize;
//		Ar << AssetSize;
//
//		if( AssetSize > 0 )
//		{
//			// Load the binary blob data
//			TArray<uint8> Buffer;
//			Buffer.AddUninitialized( AssetSize );
//			Ar.Serialize( Buffer.GetData(), AssetSize );
//#if WITH_APEX_CLOTHING
//			A.ApexClothingAsset = LoadApexClothingAssetFromBlob(Buffer);
//#endif //#if WITH_APEX_CLOTHING
//		}
//	}
//	else
//	if( Ar.IsSaving() )
//	{
//#if WITH_APEX_CLOTHING
//		if (A.ApexClothingAsset)
//		{
//			TArray<uint8> Buffer;
//			SaveApexClothingAssetToBlob(A.ApexClothingAsset, Buffer);
//			uint32 AssetSize = Buffer.Num();
//			Ar << AssetSize;
//			Ar.Serialize(Buffer.GetData(), AssetSize);
//		}
//		else
//#endif// #if WITH_APEX_CLOTHING
//		{
//			uint32 AssetSize = 0;
//			Ar << AssetSize;
//		}
//	}
//
//	return Ar;
//}

/*-----------------------------------------------------------------------------
	FSkeletalMeshResource
-----------------------------------------------------------------------------*/

FSkeletalMeshResource::FSkeletalMeshResource()
	: bInitialized(false)
{
}

void FSkeletalMeshResource::InitResources(bool bNeedsVertexColors, TArray<UMorphTarget*>& InMorphTargets)
{
	if (!bInitialized)
	{
		// initialize resources for each lod
		for( int32 LODIndex = 0;LODIndex < LODModels.Num();LODIndex++ )
		{
			LODModels[LODIndex].InitResources(bNeedsVertexColors, LODIndex, InMorphTargets);
		}
		bInitialized = true;
	}
}

void FSkeletalMeshResource::ReleaseResources()
{
	if (bInitialized)
	{
		// release resources for each lod
		for( int32 LODIndex = 0;LODIndex < LODModels.Num();LODIndex++ )
		{
			LODModels[LODIndex].ReleaseResources();
		}
		bInitialized = false;
	}
}

void FSkeletalMeshResource::Serialize(FArchive& Ar, YSkeletalMesh* Owner)
{
	DECLARE_SCOPE_CYCLE_COUNTER( TEXT("FSkeletalMeshResource::Serialize"), STAT_SkeletalMeshResource_Serialize, STATGROUP_LoadTime );

	//LODModels.Serialize(Ar,Owner);
}

bool FSkeletalMeshResource::HasExtraBoneInfluences() const
{
	for (int32 LODIndex = 0; LODIndex < LODModels.Num(); ++LODIndex)
	{
		const FStaticLODModel& Model = LODModels[LODIndex];
		if (Model.DoSectionsNeedExtraBoneInfluences())
		{
			return true;
		}
	}

	return false;
}

int32 FSkeletalMeshResource::GetMaxBonesPerSection() const
{
	int32 MaxBonesPerSection = 0;
	for (int32 LODIndex = 0; LODIndex < LODModels.Num(); ++LODIndex)
	{
		const FStaticLODModel& Model = LODModels[LODIndex];
		for (int32 SectionIndex = 0; SectionIndex < Model.Sections.Num(); ++SectionIndex)
		{
			MaxBonesPerSection = FMath::Max<int32>(MaxBonesPerSection, Model.Sections[SectionIndex].BoneMap.Num());
		}
	}
	return MaxBonesPerSection;
}

bool FSkeletalMeshResource::RequiresCPUSkinning(ERHIFeatureLevel::Type FeatureLevel) const
{
	const int32 MaxGPUSkinBones = GetFeatureLevelMaxNumberOfBones(FeatureLevel);
	const int32 MaxBonesPerChunk = GetMaxBonesPerSection();
	// Do CPU skinning if we need too many bones per chunk, or if we have too many influences per vertex on lower end
	return (MaxBonesPerChunk > MaxGPUSkinBones) || (HasExtraBoneInfluences() && FeatureLevel < ERHIFeatureLevel::ES3_1);
}

SIZE_T FSkeletalMeshResource::GetResourceSize()
{
	return GetResourceSizeBytes();
}

void FSkeletalMeshResource::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	for(int32 LODIndex = 0; LODIndex < LODModels.Num(); ++LODIndex)
	{
		const FStaticLODModel& Model = LODModels[LODIndex];
		Model.GetResourceSizeEx(CumulativeResourceSize);
	}
}

SIZE_T FSkeletalMeshResource::GetResourceSizeBytes()
{
	FResourceSizeEx ResSize;
	GetResourceSizeEx(ResSize);
	return ResSize.GetTotalMemoryBytes();
}

#if WITH_EDITORONLY_DATA
void FSkeletalMeshResource::SyncUVChannelData(const TArray<FSkeletalMaterial>& ObjectData)
{
	TSharedPtr< TArray<FMeshUVChannelInfo> > UpdateData = TSharedPtr< TArray<FMeshUVChannelInfo> >(new TArray<FMeshUVChannelInfo>);
	UpdateData->Empty(ObjectData.Num());

	for (const FSkeletalMaterial& SkeletalMaterial : ObjectData)
	{
		UpdateData->Add(SkeletalMaterial.UVChannelData);
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		SyncUVChannelData,
		FSkeletalMeshResource*, This, this,
		TSharedPtr< TArray<FMeshUVChannelInfo> >, Data, UpdateData,
		{
			FMemory::Memswap(&This->UVChannelDataPerMaterial, Data.Get(), sizeof(TArray<FMeshUVChannelInfo>));
		} );
}
#endif

///**
//* Updates morph material usage for materials referenced by each LOD entry
//*
//* @param bNeedsMorphUsage - true if the materials used by this skeletal mesh need morph target usage
//*/
//void FSkeletalMeshSceneProxy::UpdateMorphMaterialUsage_GameThread(bool bNeedsMorphUsage)
//{
//	if( bNeedsMorphUsage != bMaterialsNeedMorphUsage_GameThread )
//	{
//		// keep track of current morph material usage for the proxy
//		bMaterialsNeedMorphUsage_GameThread = bNeedsMorphUsage;
//
//		TSet<UMaterialInterface*> MaterialsToSwap;
//		for (auto It = MaterialsInUse_GameThread.CreateConstIterator(); It; ++It)
//		{
//			UMaterialInterface* Material = *It;
//			if (Material)
//			{
//				const bool bCheckMorphUsage = !bMaterialsNeedMorphUsage_GameThread || (bMaterialsNeedMorphUsage_GameThread && Material->CheckMaterialUsage_Concurrent(MATUSAGE_MorphTargets));
//				const bool bCheckSkelUsage = Material->CheckMaterialUsage_Concurrent(MATUSAGE_SkeletalMesh);
//				// make sure morph material usage and default skeletal usage are both valid
//				if( !bCheckMorphUsage || !bCheckSkelUsage  )
//				{
//					MaterialsToSwap.Add(Material);
//				}
//			}
//		}
//
//		// update the new LODSections on the render thread proxy
//		if (MaterialsToSwap.Num())
//		{
//			ENQUEUE_UNIQUE_RENDER_COMMAND_FOURPARAMETER(
//			UpdateSkelProxyLODSectionElementsCmd,
//				TSet<UMaterialInterface*>,MaterialsToSwap,MaterialsToSwap,
//				UMaterialInterface*,DefaultMaterial,UMaterial::GetDefaultMaterial(MD_Surface),
//				ERHIFeatureLevel::Type, FeatureLevel, GetScene().GetFeatureLevel(),
//			FSkeletalMeshSceneProxy*,SkelMeshSceneProxy,this,
//			{
//					for( int32 LodIdx=0; LodIdx < SkelMeshSceneProxy->LODSections.Num(); LodIdx++ )
//					{
//						FLODSectionElements& LODSection = SkelMeshSceneProxy->LODSections[LodIdx];
//						for( int32 SectIdx=0; SectIdx < LODSection.SectionElements.Num(); SectIdx++ )
//						{
//							FSectionElementInfo& SectionElement = LODSection.SectionElements[SectIdx];
//							if( MaterialsToSwap.Contains(SectionElement.Material) )
//							{
//								// fallback to default material if needed
//								SectionElement.Material = DefaultMaterial;
//							}
//						}
//					}
//					SkelMeshSceneProxy->MaterialRelevance |= DefaultMaterial->GetRelevance(FeatureLevel);
//			});
//		}
//	}
//}

#if WITH_EDITORONLY_DATA
//
//bool FSkeletalMeshSceneProxy::GetPrimitiveDistance(int32 LODIndex, int32 SectionIndex, const FVector& ViewOrigin, float& PrimitiveDistance) const
//{
//
//	if (FPrimitiveSceneProxy::GetPrimitiveDistance(LODIndex, SectionIndex, ViewOrigin, PrimitiveDistance))
//	{
//		const float OneOverDistanceMultiplier = 1.f / FMath::Max<float>(SMALL_NUMBER, StreamingDistanceMultiplier);
//		PrimitiveDistance *= OneOverDistanceMultiplier;
//		return true;
//	}
//	return false;
//}
//
//bool FSkeletalMeshSceneProxy::GetMeshUVDensities(int32 LODIndex, int32 SectionIndex, FVector4& WorldUVDensities) const
//{
//	if (LODSections.IsValidIndex(LODIndex) && LODSections[LODIndex].SectionElements.IsValidIndex(SectionIndex))
//	{
//		// The LOD-section data is stored per material index as it is only used for texture streaming currently.
//		const int32 MaterialIndex = LODSections[LODIndex].SectionElements[SectionIndex].UseMaterialIndex;
//		if (SkelMeshResource && SkelMeshResource->UVChannelDataPerMaterial.IsValidIndex(MaterialIndex))
//		{
//			const float TransformScale = GetLocalToWorld().GetMaximumAxisScale();
//			const float* LocalUVDensities = SkelMeshResource->UVChannelDataPerMaterial[MaterialIndex].LocalUVDensities;
//
//			WorldUVDensities.Set(
//				LocalUVDensities[0] * TransformScale,
//				LocalUVDensities[1] * TransformScale,
//				LocalUVDensities[2] * TransformScale,
//				LocalUVDensities[3] * TransformScale);
//
//			return true;
//		}
//	}
//	return FPrimitiveSceneProxy::GetMeshUVDensities(LODIndex, SectionIndex, WorldUVDensities);
//}
//
//bool FSkeletalMeshSceneProxy::GetMaterialTextureScales(int32 LODIndex, int32 SectionIndex, const FMaterialRenderProxy* MaterialRenderProxy, FVector4* OneOverScales, FIntVector4* UVChannelIndices) const
//{
//	if (LODSections.IsValidIndex(LODIndex) && LODSections[LODIndex].SectionElements.IsValidIndex(SectionIndex))
//	{
//		const UMaterialInterface* Material = LODSections[LODIndex].SectionElements[SectionIndex].Material;
//		if (Material)
//		{
//			// This is thread safe because material texture data is only updated while the renderthread is idle.
//			for (const FMaterialTextureInfo TextureData : Material->GetTextureStreamingData())
//			{
//				const int32 TextureIndex = TextureData.TextureIndex;
//				if (TextureData.IsValid(true))
//				{
//					OneOverScales[TextureIndex / 4][TextureIndex % 4] = 1.f / TextureData.SamplingScale;
//					UVChannelIndices[TextureIndex / 4][TextureIndex % 4] = TextureData.UVChannelIndex;
//				}
//			}
//			return true;
//		}
//	}
//	return false;
//}
#endif

FBoxSphereBounds YSkeletalMesh::GetBounds()
{
	return ExtendedBounds;
}

FBoxSphereBounds YSkeletalMesh::GetImportedBounds()
{
	return ImportedBounds;
}
void YSkeletalMesh::SetImportedBounds(const FBoxSphereBounds& InBounds)
{
	ImportedBounds = InBounds;
	CalculateExtendedBounds();
}
void YSkeletalMesh::CalculateExtendedBounds()
{
	FBoxSphereBounds CalculatedBounds = ImportedBounds;

	// Convert to Min and Max
	FVector Min = CalculatedBounds.Origin - CalculatedBounds.BoxExtent;
	FVector Max = CalculatedBounds.Origin + CalculatedBounds.BoxExtent;
	// Apply bound extensions
	Min -= NegativeBoundsExtension;
	Max += PositiveBoundsExtension;
	// Convert back to Origin, Extent and update SphereRadius
	CalculatedBounds.Origin = (Min + Max) / 2;
	CalculatedBounds.BoxExtent = (Max - Min) / 2;
	CalculatedBounds.SphereRadius = CalculatedBounds.BoxExtent.GetAbsMax();

	ExtendedBounds = CalculatedBounds;
}
void YSkeletalMesh::BeginDestroy()
{
	//Super::BeginDestroy();

	// remove the cache of link up
	if (Skeleton)
	{
		//Skeleton->RemoveLinkup(this);
	}

#if WITH_APEX_CLOTHING
	// release clothing assets
	for (FClothingAssetData& Data : ClothingAssets)
	{
		if (Data.ApexClothingAsset)
		{
			GPhysCommandHandler->DeferredRelease(Data.ApexClothingAsset);
			Data.ApexClothingAsset = nullptr;
		}
	}
#endif // #if WITH_APEX_CLOTHING

	// Release the mesh's render resources.
	ReleaseResources();
}
void YSkeletalMesh::ReleaseResources()
{
	ImportedResource->ReleaseResources();
	// insert a fence to signal when these commands completed
	//ReleaseResourcesFence.BeginFence();
}
bool YSkeletalMesh::IsReadyForFinishDestroy()
{
	// see if we have hit the resource flush fence
	//return ReleaseResourcesFence.IsFenceComplete();
	return true;
}

void YSkeletalMesh::Serialize(FArchive& Ar)
{
	//DECLARE_SCOPE_CYCLE_COUNTER(TEXT("USkeletalMesh::Serialize"), STAT_SkeletalMesh_Serialize, STATGROUP_LoadTime);

	//Super::Serialize(Ar);

	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);
	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
	Ar.UsingCustomVersion(FRenderingObjectVersion::GUID);

	FStripDataFlags StripFlags(Ar);

	Ar << ImportedBounds;
	Ar << Materials;

	Ar << RefSkeleton;

	// Serialize the default resource.
	ImportedResource->Serialize(Ar, this);

	// Build adjacency information for meshes that have not yet had it built.
#if WITH_EDITOR
	for (int32 LODIndex = 0; LODIndex < ImportedResource->LODModels.Num(); ++LODIndex)
	{
		FStaticLODModel& LODModel = ImportedResource->LODModels[LODIndex];

		if (!LODModel.AdjacencyMultiSizeIndexContainer.IsIndexBufferValid()
#if WITH_APEX_CLOTHING
			|| (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_APEX_CLOTH_TESSELLATION && LODModel.HasApexClothData())
#endif // WITH_APEX_CLOTHING
			)
		{
			TArray<FSoftSkinVertex> Vertices;
			FMultiSizeIndexContainerData IndexData;
			FMultiSizeIndexContainerData AdjacencyIndexData;
			//IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");

			//UE_LOG(LogSkeletalMesh, Warning, TEXT("Building adjacency information for skeletal mesh '%s'. Please resave the asset."), *GetPathName());
			LODModel.GetVertices(Vertices);
			LODModel.MultiSizeIndexContainer.GetIndexBufferData(IndexData);
			AdjacencyIndexData.DataTypeSize = IndexData.DataTypeSize;
			//MeshUtilities.BuildSkeletalAdjacencyIndexBuffer(Vertices, LODModel.NumTexCoords, IndexData.Indices, AdjacencyIndexData.Indices);
			//LODModel.AdjacencyMultiSizeIndexContainer.RebuildIndexBuffer(AdjacencyIndexData);
		}
	}
#endif // #if WITH_EDITOR

	// make sure we're counting properly
	if (!Ar.IsLoading() && !Ar.IsSaving())
	{
		Ar << RefBasesInvMatrix;
	}

	if (Ar.UE4Ver() < VER_UE4_REFERENCE_SKELETON_REFACTOR)
	{
		TMap<FName, int32> DummyNameIndexMap;
		Ar << DummyNameIndexMap;
	}

	//@todo legacy
	TArray<UObject*> DummyObjs;
	Ar << DummyObjs;

	if (Ar.IsLoading() && Ar.CustomVer(FRenderingObjectVersion::GUID) < FRenderingObjectVersion::TextureStreamingMeshUVChannelData)
	{
		TArray<float> CachedStreamingTextureFactors;
		Ar << CachedStreamingTextureFactors;
	}

	if (!StripFlags.IsEditorDataStripped())
	{
		FSkeletalMeshSourceData& SkelSourceData = *(FSkeletalMeshSourceData*)(&SourceData);
		SkelSourceData.Serialize(Ar, this);
	}

#if WITH_EDITORONLY_DATA
	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON && !AssetImportData)
	{
		// AssetImportData should always be valid
		//AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}

	// SourceFilePath and SourceFileTimestamp were moved into a subobject
	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_ADDED_FBX_ASSET_IMPORT_DATA && AssetImportData)
	{
		// AssetImportData should always have been set up in the constructor where this is relevant
		//FAssetImportInfo Info;
		//Info.Insert(FAssetImportInfo::FSourceFile(SourceFilePath_DEPRECATED));
		//AssetImportData->SourceData = MoveTemp(Info);

		SourceFilePath_DEPRECATED = TEXT("");
		SourceFileTimestamp_DEPRECATED = TEXT("");
	}
#endif // WITH_EDITORONLY_DATA
	if (Ar.UE4Ver() >= VER_UE4_APEX_CLOTH)
	{
		// Serialize non-UPROPERTY ApexClothingAsset data.
		//for (int32 Idx = 0; Idx < ClothingAssets.Num(); Idx++)
		//{
			//Ar << ClothingAssets[Idx];
		//}

		if (Ar.UE4Ver() < VER_UE4_REFERENCE_SKELETON_REFACTOR)
		{
			RebuildRefSkeletonNameToIndexMap();
		}
	}

	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_MOVE_SKELETALMESH_SHADOWCASTING)
	{
		// Previous to this version, shadowcasting flags were stored in the LODInfo array
		// now they're in the Materials array so we need to move them over
		MoveDeprecatedShadowFlagToMaterials();
	}
#if WITH_EDITORONLY_DATA
	if (Ar.UE4Ver() < VER_UE4_SKELETON_ASSET_PROPERTY_TYPE_CHANGE)
	{
		//PreviewAttachedAssetContainer.SaveAttachedObjectsFromDeprecatedProperties();
	}
#endif

	if (bEnablePerPolyCollision)
	{
		//Ar << BodySetup;
	}

#if WITH_EDITORONLY_DATA
	if (Ar.CustomVer(FEditorObjectVersion::GUID) < FEditorObjectVersion::RefactorMeshEditorMaterials)
	{
		MoveMaterialFlagsToSections();
	}
#endif

#if WITH_APEX_CLOTHING && WITH_EDITORONLY_DATA
	if (GIsEditor && Ar.IsLoading() && Ar.CustomVer(FFrameworkObjectVersion::GUID) < FFrameworkObjectVersion::AddInternalClothingGraphicalSkinning)
	{
		ApexClothingUtils::BackupClothingDataFromSkeletalMesh(this);
		ApexClothingUtils::ReapplyClothingDataToSkeletalMesh(this);
	}
#endif

#if WITH_EDITORONLY_DATA
	bRequiresLODScreenSizeConversion = Ar.CustomVer(FFrameworkObjectVersion::GUID) < FFrameworkObjectVersion::LODsUseResolutionIndependentScreenSize;
	bRequiresLODHysteresisConversion = Ar.CustomVer(FFrameworkObjectVersion::GUID) < FFrameworkObjectVersion::LODHysteresisUseResolutionIndependentScreenSize;
#endif
}

FArchive& operator<<(FArchive& Ar, FMeshUVChannelInfo& ChannelData)
{
	Ar << ChannelData.bInitialized;
	Ar << ChannelData.bOverrideDensities;

	for (int32 CoordIndex = 0; CoordIndex < TEXSTREAM_MAX_NUM_UVCHANNELS; ++CoordIndex)
	{
		Ar << ChannelData.LocalUVDensities[CoordIndex];
	}

	return Ar;

}
FArchive& operator<<(FArchive& Ar, FSkeletalMaterial& Elem)
{
	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);

	//Ar << Elem.MaterialInterface;

	//Use the automatic serialization instead of this custom operator
	if (Ar.CustomVer(FEditorObjectVersion::GUID) >= FEditorObjectVersion::RefactorMeshEditorMaterials)
	{
		Ar << Elem.MaterialSlotName;
#if WITH_EDITORONLY_DATA
		/*if (!Ar.IsCooking() || Ar.CookingTarget()->HasEditorOnlyData())
		{
			Ar << Elem.ImportedMaterialSlotName;
		}*/
#endif //#if WITH_EDITORONLY_DATA
	}
	else
	{
		if (Ar.UE4Ver() >= VER_UE4_MOVE_SKELETALMESH_SHADOWCASTING)
		{
			Ar << Elem.bEnableShadowCasting_DEPRECATED;
		}

		Ar.UsingCustomVersion(FRecomputeTangentCustomVersion::GUID);
		if (Ar.CustomVer(FRecomputeTangentCustomVersion::GUID) >= FRecomputeTangentCustomVersion::RuntimeRecomputeTangent)
		{
			Ar << Elem.bRecomputeTangent_DEPRECATED;
		}
	}

	if (!Ar.IsLoading() || Ar.CustomVer(FRenderingObjectVersion::GUID) >= FRenderingObjectVersion::TextureStreamingMeshUVChannelData)
	{
		Ar << Elem.UVChannelData;
	}

	return Ar;
}
void YSkeletalMesh::MoveDeprecatedShadowFlagToMaterials()
{

}

void YSkeletalMesh::RebuildRefSkeletonNameToIndexMap()
{
	TArray<FBoneIndexType> DuplicateBones;
	// Make sure we have no duplicate bones. Some content got corrupted somehow. :(
	//RefSkeleton.RemoveDuplicateBones(this, DuplicateBones);

	// If we have removed any duplicate bones, we need to fix up any broken LODs as well.
	// Duplicate bones are given from highest index to lowest. 
	// so it's safe to decrease indices for children, we're not going to lose the index of the remaining duplicate bones.
	for (int32 Index = 0; Index < DuplicateBones.Num(); Index++)
	{
		const FBoneIndexType& DuplicateBoneIndex = DuplicateBones[Index];
		for (int32 LodIndex = 0; LodIndex < LODInfo.Num(); LodIndex++)
		{
			FStaticLODModel & ThisLODModel = ImportedResource->LODModels[LodIndex];
			{
				int32 FoundIndex;
				if (ThisLODModel.RequiredBones.Find(DuplicateBoneIndex, FoundIndex))
				{
					ThisLODModel.RequiredBones.RemoveAt(FoundIndex, 1);
					// we need to shift indices of the remaining bones.
					for (int32 j = FoundIndex; j < ThisLODModel.RequiredBones.Num(); j++)
					{
						ThisLODModel.RequiredBones[j] = ThisLODModel.RequiredBones[j] - 1;
					}
				}
			}

			{
				int32 FoundIndex;
				if (ThisLODModel.ActiveBoneIndices.Find(DuplicateBoneIndex, FoundIndex))
				{
					ThisLODModel.ActiveBoneIndices.RemoveAt(FoundIndex, 1);
					// we need to shift indices of the remaining bones.
					for (int32 j = FoundIndex; j < ThisLODModel.ActiveBoneIndices.Num(); j++)
					{
						ThisLODModel.ActiveBoneIndices[j] = ThisLODModel.ActiveBoneIndices[j] - 1;
					}
				}
			}
		}
	}

	// Rebuild name table.
	RefSkeleton.RebuildNameToIndexMap();
}

void YSkeletalMesh::MoveMaterialFlagsToSections()
{
	//No LOD we cant set the value
	if (LODInfo.Num() == 0)
	{
		return;
	}

	for (FStaticLODModel &StaticLODModel : ImportedResource->LODModels)
	{
		for (int32 SectionIndex = 0; SectionIndex < StaticLODModel.Sections.Num(); ++SectionIndex)
		{
			FSkelMeshSection &Section = StaticLODModel.Sections[SectionIndex];
			//Prior to FEditorObjectVersion::RefactorMeshEditorMaterials Material index match section index
			if (Materials.IsValidIndex(SectionIndex))
			{
				Section.bCastShadow = Materials[SectionIndex].bEnableShadowCasting_DEPRECATED;

				Section.bRecomputeTangent = Materials[SectionIndex].bRecomputeTangent_DEPRECATED;
			}
			else
			{
				//Default cast shadow to true this is a fail safe code path it should not go here if the data
				//is valid
				Section.bCastShadow = true;
				//Recompute tangent is serialize prior to FEditorObjectVersion::RefactorMeshEditorMaterials
				// We just keep the serialize value
			}
		}
	}
}


void YSkeletalMesh::PostInitProperties()
{
//#if WITH_EDITORONLY_DATA
//	if (!HasAnyFlags(RF_ClassDefaultObject))
//	{
//		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
//	}
//#endif
//	Super::PostInitProperties();
}


void YSkeletalMesh::PostLoad()
{
	//Super::PostLoad();

	// If LODInfo is missing - create array of correct size.
	if (LODInfo.Num() != ImportedResource->LODModels.Num())
	{
		LODInfo.Empty(ImportedResource->LODModels.Num());
		LODInfo.AddZeroed(ImportedResource->LODModels.Num());

		for (int32 i = 0; i < LODInfo.Num(); i++)
		{
			LODInfo[i].LODHysteresis = 0.02f;
		}
	}

	int32 TotalLODNum = LODInfo.Num();
	for (int32 LodIndex = 0; LodIndex < TotalLODNum; LodIndex++)
	{
		FSkeletalMeshLODInfo& ThisLODInfo = LODInfo[LodIndex];
		FStaticLODModel& ThisLODModel = ImportedResource->LODModels[LodIndex];

		// Presize the per-section TriangleSortSettings array
		if (ThisLODInfo.TriangleSortSettings.Num() > ThisLODModel.Sections.Num())
		{
			ThisLODInfo.TriangleSortSettings.RemoveAt(ThisLODModel.Sections.Num(), ThisLODInfo.TriangleSortSettings.Num() - ThisLODModel.Sections.Num());
		}
		else
			if (ThisLODModel.Sections.Num() > ThisLODInfo.TriangleSortSettings.Num())
			{
				ThisLODInfo.TriangleSortSettings.AddZeroed(ThisLODModel.Sections.Num() - ThisLODInfo.TriangleSortSettings.Num());
			}

#if WITH_EDITOR
		//if (ThisLODInfo.ReductionSettings.BonesToRemove_DEPRECATED.Num() > 0)
		//{
		//	for (auto& BoneToRemove : ThisLODInfo.ReductionSettings.BonesToRemove_DEPRECATED)
		//	{
		//		AddBoneToReductionSetting(LodIndex, BoneToRemove.BoneName);
		//	}

		//	// since in previous system, we always removed from previous LOD, I'm adding this 
		//	// here for previous LODs
		//	for (int32 CurLodIndx = LodIndex + 1; CurLodIndx < TotalLODNum; ++CurLodIndx)
		//	{
		//		AddBoneToReductionSetting(CurLodIndx, ThisLODInfo.RemovedBones);
		//	}

		//	// we don't apply this change here, but this will be applied when you re-gen simplygon
		//	ThisLODInfo.ReductionSettings.BonesToRemove_DEPRECATED.Empty();
		//}
#endif
	}

	// Revert to using 32 bit Float UVs on hardware that doesn't support rendering with 16 bit Float UVs 
	if (!bUseFullPrecisionUVs && !GVertexElementTypeSupport.IsSupported(VET_Half2))
	{
		bUseFullPrecisionUVs = true;
		// convert each LOD level to 32 bit UVs
		for (int32 LODIdx = 0; LODIdx < ImportedResource->LODModels.Num(); LODIdx++)
		{
			FStaticLODModel& LODModel = ImportedResource->LODModels[LODIdx];
			// Determine the correct version of ConvertToFullPrecisionUVs based on the number of UVs in the vertex buffer
			const uint32 NumTexCoords = LODModel.VertexBufferGPUSkin.GetNumTexCoords();
			switch (NumTexCoords)
			{
			case 1: LODModel.VertexBufferGPUSkin.ConvertToFullPrecisionUVs<1>(); break;
			case 2: LODModel.VertexBufferGPUSkin.ConvertToFullPrecisionUVs<2>(); break;
			case 3: LODModel.VertexBufferGPUSkin.ConvertToFullPrecisionUVs<3>(); break;
			case 4: LODModel.VertexBufferGPUSkin.ConvertToFullPrecisionUVs<4>(); break;
			}
		}
	}

#if WITH_EDITORONLY_DATA
	// Rebuild vertex buffers if needed
	/*if (GetLinkerCustomVersion(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::UseSeparateSkinWeightBuffer)
	{
		FSkeletalMeshResource* Resource = GetImportedResource();
		if (Resource && FPlatformProperties::HasEditorOnlyData())
		{
			uint32 VertexFlags = GetVertexBufferFlags();
			for (int32 LODIndex = 0; LODIndex < Resource->LODModels.Num(); LODIndex++)
			{
				Resource->LODModels[LODIndex].BuildVertexBuffers(VertexFlags);
			}
		}
	}*/
#endif // WITH_EDITOR

	// initialize rendering resources
	if (FApp::CanEverRender())
	{
		InitResources();
	}
	else
	{
		// Update any missing data when cooking.
		UpdateUVChannelData(false);
	}

	CalculateInvRefMatrices();

	// init morph targets
	InitMorphTargets();

#if WITH_APEX_CLOTHING
	// load clothing section collision
	for (int32 AssetIdx = 0; AssetIdx < ClothingAssets.Num(); AssetIdx++)
	{
		if (ClothingAssets[AssetIdx].ApexClothingAsset)
		{
			LoadClothCollisionVolumes(AssetIdx, ClothingAssets[AssetIdx].ApexClothingAsset);
		}
#if WITH_EDITOR
		// Remove any clothing sections that have invalid APEX data.
		// This can occur if you load and re-save content in a build with APEX disabled.
		else
		{
			// Actually need to call ApexClothingUtils::Restore... here but we can't as it's in the editor package

			ClothingAssets.RemoveAt(AssetIdx);
			--AssetIdx;
		}
#endif // WITH_EDITOR
	}

	// validate influences for existing clothing
	if (FSkeletalMeshResource* SkelResource = GetImportedResource())
	{
		for (int32 LODIndex = 0; LODIndex < SkelResource->LODModels.Num(); ++LODIndex)
		{
			FStaticLODModel& CurLODModel = SkelResource->LODModels[LODIndex];

			for (int32 SectionIdx = 0; SectionIdx < CurLODModel.Sections.Num(); SectionIdx++)
			{
				FSkelMeshSection& CurSection = CurLODModel.Sections[SectionIdx];

				if (CurSection.CorrespondClothSectionIndex != INDEX_NONE && CurSection.MaxBoneInfluences > MAX_INFLUENCES_PER_STREAM)
				{
					UE_LOG(LogSkeletalMesh, Warning, TEXT("Section %d for LOD %d in skeletal mesh %s has clothing associated but has %d influences. Clothing only supports a maximum of %d influences - reduce influences on chunk and reimport mesh."),
						SectionIdx,
						LODIndex,
						*GetName(),
						CurSection.MaxBoneInfluences,
						MAX_INFLUENCES_PER_STREAM);
				}
			}
		}
	}

#endif // WITH_APEX_CLOTHING

	/*if (GetLinkerUE4Version() < VER_UE4_REFERENCE_SKELETON_REFACTOR)
	{
		RebuildRefSkeletonNameToIndexMap();
	}*/

#if WITH_APEX_CLOTHING
	//We can't build apex map until this point because old content needs to call RebuildNameToIndexMap
	if (GetLinkerUE4Version() >= VER_UE4_APEX_CLOTH)
	{
		BuildApexToUnrealBoneMapping();
	}
#endif

	/*if (GetLinkerUE4Version() < VER_UE4_SORT_ACTIVE_BONE_INDICES)
	{
		for (int32 LodIndex = 0; LodIndex < LODInfo.Num(); LodIndex++)
		{
			FStaticLODModel & ThisLODModel = ImportedResource->LODModels[LodIndex];
			ThisLODModel.ActiveBoneIndices.Sort();
		}
	}*/

#if WITH_EDITORONLY_DATA
	if (RetargetBasePose.Num() == 0)
	{
		RetargetBasePose = RefSkeleton.GetRefBonePose();
	}
#endif

	// Bounds have been loaded - apply extensions.
	CalculateExtendedBounds();

	const bool bRebuildNameMap = false;
	RefSkeleton.RebuildRefSkeleton(Skeleton, bRebuildNameMap);

	//if (GetLinkerCustomVersion(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::RegenerateClothingShadowFlags)
	//{
	//	if (FSkeletalMeshResource* MeshResource = GetImportedResource())
	//	{
	//		for (FStaticLODModel& LodModel : MeshResource->LODModels)
	//		{
	//			for (FSkelMeshSection& Section : LodModel.Sections)
	//			{
	//				if (Section.HasApexClothData())
	//				{
	//					check(LodModel.Sections.IsValidIndex(Section.CorrespondClothSectionIndex));

	//					FSkelMeshSection& OriginalSection = LodModel.Sections[Section.CorrespondClothSectionIndex];
	//					Section.bCastShadow = OriginalSection.bCastShadow;
	//				}
	//			}
	//		}
	//	}
	//}

#if WITH_EDITORONLY_DATA
	if (bRequiresLODScreenSizeConversion || bRequiresLODHysteresisConversion)
	{
		// Convert screen area to screen size
		ConvertLegacyLODScreenSize();
	}
#endif

}

/**
* Returns a one line description of an object for viewing in the thumbnail view of the generic browser
*/
FString YSkeletalMesh::GetDesc()
{
	FSkeletalMeshResource* Resource = GetImportedResource();
	check(Resource->LODModels.Num() > 0);
	return FString::Printf(TEXT("%d Triangles, %d Bones"), Resource->LODModels[0].GetTotalFaces(), RefSkeleton.GetRawBoneNum());
}

/**
* This will return detail info about this specific object. (e.g. AudioComponent will return the name of the cue,
* ParticleSystemComponent will return the name of the ParticleSystem)  The idea here is that in many places
* you have a component of interest but what you really want is some characteristic that you can use to track
* down where it came from.
*/
FString YSkeletalMesh::GetDetailedInfoInternal() const
{
	//return GetPathName(nullptr);
	return "";
}

void YSkeletalMesh::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	//Super::GetResourceSizeEx(CumulativeResourceSize);

	if (ImportedResource.IsValid())
	{
		ImportedResource->GetResourceSizeEx(CumulativeResourceSize);
	}

	if (CumulativeResourceSize.GetResourceSizeMode() == EResourceSizeMode::Inclusive)
	{
		for (const auto& MorphTarget : MorphTargets)
		{
			//MorphTarget->GetResourceSizeEx(CumulativeResourceSize);
		}

	/*	for (const auto& ClothingAsset : ClothingAssets)
		{
			ClothingAsset.GetResourceSizeEx(CumulativeResourceSize);
		}
*/
		TSet<UMaterialInterface*> UniqueMaterials;
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			UMaterialInterface* Material = Materials[MaterialIndex].MaterialInterface;
			bool bAlreadyCounted = false;
			UniqueMaterials.Add(Material, &bAlreadyCounted);
			if (!bAlreadyCounted && Material)
			{
				//Material->GetResourceSizeEx(CumulativeResourceSize);
			}
		}

#if WITH_EDITORONLY_DATA
		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RetargetBasePose.GetAllocatedSize());
#endif

		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RefBasesInvMatrix.GetAllocatedSize());
		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(RefSkeleton.GetDataSize());

		//if (BodySetup)
		//{
		//	BodySetup->GetResourceSizeEx(CumulativeResourceSize);
		//}

		//if (PhysicsAsset)
		//{
		//	PhysicsAsset->GetResourceSizeEx(CumulativeResourceSize);
		//}
	}
}


bool YSkeletalMesh::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool bInUseAllTriData)
{
#if 0
#if WITH_EDITORONLY_DATA

	// Fail if no mesh or not per poly collision
	if (!ImportedResource.IsValid() || !bEnablePerPolyCollision)
	{
		return false;
	}

	const FStaticLODModel& Model = ImportedResource->LODModels[0];

	{
		// Copy all verts into collision vertex buffer.
		CollisionData->Vertices.Empty();
		CollisionData->Vertices.AddUninitialized(Model.NumVertices);
		const uint32 NumSections = Model.Sections.Num();

		for (uint32 SectionIdx = 0; SectionIdx < NumSections; ++SectionIdx)
		{
			const FSkelMeshSection& Section = Model.Sections[SectionIdx];
			{
				//soft
				const uint32 SoftOffset = Section.GetVertexBufferIndex();
				const uint32 NumSoftVerts = Section.GetNumVertices();
				for (uint32 SoftIdx = 0; SoftIdx < NumSoftVerts; ++SoftIdx)
				{
					CollisionData->Vertices[SoftIdx + SoftOffset] = Section.SoftVertices[SoftIdx].Position;
				}
			}

		}
	}

	{
		// Copy indices into collision index buffer
		const FMultiSizeIndexContainer& IndexBufferContainer = Model.MultiSizeIndexContainer;

		TArray<uint32> Indices;
		IndexBufferContainer.GetIndexBuffer(Indices);

		const uint32 NumTris = Indices.Num() / 3;
		CollisionData->Indices.Empty();
		CollisionData->Indices.Reserve(NumTris);

		FTriIndices TriIndex;
		for (int32 SectionIndex = 0; SectionIndex < Model.Sections.Num(); ++SectionIndex)
		{
			const FSkelMeshSection& Section = Model.Sections[SectionIndex];

			const uint32 OnePastLastIndex = Section.BaseIndex + Section.NumTriangles * 3;

			for (uint32 i = Section.BaseIndex; i < OnePastLastIndex; i += 3)
			{
				TriIndex.v0 = Indices[i];
				TriIndex.v1 = Indices[i + 1];
				TriIndex.v2 = Indices[i + 2];

				CollisionData->Indices.Add(TriIndex);
				CollisionData->MaterialIndices.Add(Section.MaterialIndex);
			}
		}
	}

	CollisionData->bFlipNormals = true;
	CollisionData->bDeformableMesh = true;

	// We only have a valid TriMesh if the CollisionData has vertices AND indices. For meshes with disabled section collision, it
	// can happen that the indices will be empty, in which case we do not want to consider that as valid trimesh data
	return CollisionData->Vertices.Num() > 0 && CollisionData->Indices.Num() > 0;
#else // #if WITH_EDITORONLY_DATA
	return false;
#endif // #if WITH_EDITORONLY_DATA
#endif 
	return false;
}
bool YSkeletalMesh::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return bEnablePerPolyCollision;
}

void YSkeletalMesh::InitResources()
{
	UpdateUVChannelData(false);
	ImportedResource->InitResources(bHasVertexColors, MorphTargets);
}

void YSkeletalMesh::UpdateUVChannelData(bool bRebuildAll)
{
#if WITH_EDITORONLY_DATA
	// Once cooked, the data requires to compute the scales will not be CPU accessible.
	FSkeletalMeshResource* Resource = GetImportedResource();
	if (FPlatformProperties::HasEditorOnlyData() && Resource)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			FMeshUVChannelInfo& UVChannelData = Materials[MaterialIndex].UVChannelData;

			// Skip it if we want to keep it.
			if (UVChannelData.bInitialized && (!bRebuildAll || UVChannelData.bOverrideDensities))
				continue;

			float WeightedUVDensities[TEXSTREAM_MAX_NUM_UVCHANNELS] = { 0, 0, 0, 0 };
			float Weights[TEXSTREAM_MAX_NUM_UVCHANNELS] = { 0, 0, 0, 0 };

			for (const FStaticLODModel& LODModel : Resource->LODModels)
			{
				for (const FSkelMeshSection& SectionInfo : LODModel.Sections)
				{
					if (SectionInfo.MaterialIndex != MaterialIndex)
						continue;

					//AccumulateUVDensities(WeightedUVDensities, Weights, LODModel, SectionInfo);
				}
			}

			UVChannelData.bInitialized = true;
			UVChannelData.bOverrideDensities = false;
			for (int32 CoordinateIndex = 0; CoordinateIndex < TEXSTREAM_MAX_NUM_UVCHANNELS; ++CoordinateIndex)
			{
				UVChannelData.LocalUVDensities[CoordinateIndex] = (Weights[CoordinateIndex] > KINDA_SMALL_NUMBER) ? (WeightedUVDensities[CoordinateIndex] / Weights[CoordinateIndex]) : 0;
			}
		}

		Resource->SyncUVChannelData(Materials);
	}
#endif
}

void YSkeletalMesh::InitMorphTargets()
{
	MorphTargetIndexMap.Empty();

	//for (int32 Index = 0; Index < MorphTargets.Num(); ++Index)
	//{
	//	UMorphTarget* MorphTarget = MorphTargets[Index];
	//	FName const ShapeName = MorphTarget->GetFName();
	//	if (MorphTargetIndexMap.Find(ShapeName) == nullptr)
	//	{
	//		MorphTargetIndexMap.Add(ShapeName, Index);

	//		// register as morphtarget curves
	//		if (Skeleton)
	//		{
	//			FSmartName CurveName;
	//			CurveName.DisplayName = ShapeName;

	//			// verify will make sure it adds to the curve if not found
	//			// the reason of using this is to make sure it works in editor/non-editor
	//			Skeleton->VerifySmartName(USkeleton::AnimCurveMappingName, CurveName);
	//			Skeleton->AccumulateCurveMetaData(ShapeName, false, true);
	//		}
	//	}
	//}
}

float ComputeBoundsScreenSize(const FVector4& BoundsOrigin, const float SphereRadius, const FVector4& ViewOrigin, const FMatrix& ProjMatrix)
{
	const float Dist = FVector::Dist(BoundsOrigin, ViewOrigin);

	// Get projection multiple accounting for view scaling.
	const float ScreenMultiple = FMath::Max(0.5f * ProjMatrix.M[0][0], 0.5f * ProjMatrix.M[1][1]);

	// Calculate screen-space projected radius
	const float ScreenRadius = ScreenMultiple * SphereRadius / FMath::Max(1.0f, Dist);

	// For clarity, we end up comparing the diameter
	return ScreenRadius * 2.0f;
}


#if WITH_EDITORONLY_DATA
void YSkeletalMesh::ConvertLegacyLODScreenSize()
{
	if (LODInfo.Num() == 1)
	{
		// Only one LOD
		LODInfo[0].ScreenSize = 1.0f;
	}
	else
	{
		// Use 1080p, 90 degree FOV as a default, as this should not cause runtime regressions in the common case.
		// LODs will appear different in Persona, however.
		const float HalfFOV = PI * 0.25f;
		const float ScreenWidth = 1920.0f;
		const float ScreenHeight = 1080.0f;
		const FPerspectiveMatrix ProjMatrix(HalfFOV, ScreenWidth, ScreenHeight, 1.0f);
		FBoxSphereBounds Bounds = GetBounds();

		// Multiple models, we should have LOD screen area data.
		for (int32 LODIndex = 0; LODIndex < LODInfo.Num(); ++LODIndex)
		{
			FSkeletalMeshLODInfo& LODInfoEntry = LODInfo[LODIndex];

			if (bRequiresLODScreenSizeConversion)
			{
				if (LODInfoEntry.ScreenSize == 0.0f)
				{
					LODInfoEntry.ScreenSize = 1.0f;
				}
				else
				{
					// legacy screen size was scaled by a fixed constant of 320.0f, so its kinda arbitrary. Convert back to distance based metric first.
					const float ScreenDepth = FMath::Max(ScreenWidth / 2.0f * ProjMatrix.M[0][0], ScreenHeight / 2.0f * ProjMatrix.M[1][1]) * Bounds.SphereRadius / (LODInfoEntry.ScreenSize * 320.0f);

					// Now convert using the query function
					LODInfoEntry.ScreenSize = ComputeBoundsScreenSize(FVector::ZeroVector, Bounds.SphereRadius, FVector(0.0f, 0.0f, ScreenDepth), ProjMatrix);
				}
			}

			if (bRequiresLODHysteresisConversion)
			{
				if (LODInfoEntry.LODHysteresis != 0.0f)
				{
					// Also convert the hysteresis as if it was a screen size topo
					const float ScreenHysteresisDepth = FMath::Max(ScreenWidth / 2.0f * ProjMatrix.M[0][0], ScreenHeight / 2.0f * ProjMatrix.M[1][1]) * Bounds.SphereRadius / (LODInfoEntry.LODHysteresis * 320.0f);
					LODInfoEntry.LODHysteresis = ComputeBoundsScreenSize(FVector::ZeroVector, Bounds.SphereRadius, FVector(0.0f, 0.0f, ScreenHysteresisDepth), ProjMatrix);
				}
			}
		}
	}
}
#endif

void YSkeletalMesh::CalculateInvRefMatrices()
{
	const int32 NumRealBones = RefSkeleton.GetRawBoneNum();

	if (RefBasesInvMatrix.Num() != NumRealBones)
	{
		RefBasesInvMatrix.Empty(NumRealBones);
		RefBasesInvMatrix.AddUninitialized(NumRealBones);

		// Reset cached mesh-space ref pose
		CachedComposedRefPoseMatrices.Empty(NumRealBones);
		CachedComposedRefPoseMatrices.AddUninitialized(NumRealBones);

		// Precompute the Mesh.RefBasesInverse.
		for (int32 b = 0; b < NumRealBones; b++)
		{
			// Render the default pose.
			CachedComposedRefPoseMatrices[b] = GetRefPoseMatrix(b);

			// Construct mesh-space skeletal hierarchy.
			if (b > 0)
			{
				int32 Parent = RefSkeleton.GetRawParentIndex(b);
				CachedComposedRefPoseMatrices[b] = CachedComposedRefPoseMatrices[b] * CachedComposedRefPoseMatrices[Parent];
			}

			FVector XAxis, YAxis, ZAxis;

			CachedComposedRefPoseMatrices[b].GetScaledAxes(XAxis, YAxis, ZAxis);
			if (XAxis.IsNearlyZero(SMALL_NUMBER) &&
				YAxis.IsNearlyZero(SMALL_NUMBER) &&
				ZAxis.IsNearlyZero(SMALL_NUMBER))
			{
				// this is not allowed, warn them 
				UE_LOG(LogSkeletalMesh, Warning, TEXT("Reference Pose for joint (%s) includes NIL matrix. Zero scale isn't allowed on ref pose. "), *RefSkeleton.GetBoneName(b).ToString());
			}

			// Precompute inverse so we can use from-refpose-skin vertices.
			RefBasesInvMatrix[b] = CachedComposedRefPoseMatrices[b].Inverse();
		}

#if WITH_EDITORONLY_DATA
		if (RetargetBasePose.Num() == 0)
		{
			RetargetBasePose = RefSkeleton.GetRawRefBonePose();
		}
#endif // WITH_EDITORONLY_DATA
	}
}

FMatrix YSkeletalMesh::GetRefPoseMatrix(int32 BoneIndex) const
{
	check(BoneIndex >= 0 && BoneIndex < RefSkeleton.GetRawBoneNum());
	FTransform BoneTransform = RefSkeleton.GetRawRefBonePose()[BoneIndex];
	// Make sure quaternion is normalized!
	BoneTransform.NormalizeRotation();
	return BoneTransform.ToMatrixWithScale();
}


#undef LOCTEXT_NAMESPACE


