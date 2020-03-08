# pragma once
#include "Core.h"
#include "YRenderResource.h"
#include "YMeshCommon.h"
#include "YPackedNormal.h"
/**
 * A set of static mesh triangles which are rendered with the same material.
 */
struct YStaticMeshSection
{
	/** The index of the material with which to render this section. */
	int32 MaterialIndex;

	/** Range of vertices and indices used when rendering this section. */
	uint32 FirstIndex;
	uint32 NumTriangles;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;

	/** If true, collision is enabled for this section. */
	bool bEnableCollision;
	/** If true, this section will cast a shadow. */
	bool bCastShadow;

	/** Constructor. */
	YStaticMeshSection()
		: MaterialIndex(0)
		, FirstIndex(0)
		, NumTriangles(0)
		, MinVertexIndex(0)
		, MaxVertexIndex(0)
		, bEnableCollision(false)
		, bCastShadow(true)
	{
	}

	/** Serializer. */
	friend FArchive& operator<<(FArchive& Ar, YStaticMeshSection& Section);
};

/** An interface to the static-mesh vertex data storage type. */
// 这层的抽象原因是：vertex 定义有很多种，根据Tangnet 和uv的数量，在VB中保存的时候，要存一个抽象的指针
// 最关键的接口是GetStride和GetDataPonter()，可以设置不同的VB类型
class YStaticMeshVertexDataInterface
{
public:

	/** Virtual destructor. */
	virtual ~YStaticMeshVertexDataInterface() {}

	/**
	* Resizes the vertex data buffer, discarding any data which no longer fits.
	* @param NumVertices - The number of vertices to allocate the buffer for.
	*/
	virtual void ResizeBuffer(uint32 NumVertices) = 0;

	/** @return The stride of the vertex data in the buffer. */
	virtual uint32 GetStride() const = 0;

	/** @return A pointer to the data in the buffer. */
	virtual uint8* GetDataPointer() = 0;

	/** @return A pointer to the FResourceArrayInterface for the vertex data. */
	virtual FResourceArrayInterface* GetResourceArray() = 0;

	/** Serializer. */
	virtual void Serialize(FArchive& Ar) = 0;
};


/** The implementation of the static mesh vertex data storage type. */
// VertexDataType 就是要放到TArray中的T
// YStaticMeshVertexData 实现了： Buffer storage, stride, GetDataPointer， isCpuAccess的功能
// 实现的时候只需要实现一个VertexDataType的类就行，
template<typename VertexDataType>
class YStaticMeshVertexData :
	public YStaticMeshVertexDataInterface,
	public TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT>
{
public:

	typedef TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT> ArrayType;

	/**
	* Constructor
	* @param InNeedsCPUAccess - true if resource array data should be CPU accessible
	*/
	YStaticMeshVertexData(bool InNeedsCPUAccess = false)
		: TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT>(InNeedsCPUAccess)
	{
	}

	/**
	* Resizes the vertex data buffer, discarding any data which no longer fits.
	*
	* @param NumVertices - The number of vertices to allocate the buffer for.
	*/
	virtual void ResizeBuffer(uint32 NumVertices)
	{
		if ((uint32)ArrayType::Num() < NumVertices)
		{
			// Enlarge the array.
			ArrayType::AddUninitialized(NumVertices - ArrayType::Num());
		}
		else if ((uint32)ArrayType::Num() > NumVertices)
		{
			// Shrink the array.
			ArrayType::RemoveAt(NumVertices, ArrayType::Num() - NumVertices);
		}
	}
	/**
	* @return stride of the vertex type stored in the resource data array
	*/
	virtual uint32 GetStride() const
	{
		return sizeof(VertexDataType);
	}
	/**
	* @return uint8 pointer to the resource data array
	*/
	virtual uint8* GetDataPointer()
	{
		return (uint8*)&(*this)[0];
	}
	/**
	* @return resource array interface access
	*/
	virtual FResourceArrayInterface* GetResourceArray()
	{
		return this;
	}
	/**
	* Serializer for this class
	*
	* @param Ar - archive to serialize to
	* @param B - data to serialize
	*/
	virtual void Serialize(FArchive& Ar)
	{
		TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT>::BulkSerialize(Ar);
	}
	/**
	* Assignment operator. This is currently the only method which allows for
	* modifying an existing resource array
	*/
	YStaticMeshVertexData<VertexDataType>& operator=(const TArray<VertexDataType>& Other)
	{
		TResourceArray<VertexDataType, VERTEXBUFFER_ALIGNMENT>::operator=(TArray<VertexDataType, TAlignedHeapAllocator<VERTEXBUFFER_ALIGNMENT> >(Other));
		return *this;
	}
};

struct YPositionVertex
{
	FVector Position;
	friend FArchive& operator<<(FArchive& Ar, YPositionVertex& V)
	{
		Ar << V.Position;
		return Ar;
	}
};

class YPositionVertexData :public YStaticMeshVertexData<YPositionVertex>
{
public:
	YPositionVertexData(bool InNeedsCUPAccess = false) :
		YStaticMeshVertexData<YPositionVertex>(InNeedsCUPAccess)
	{

	}
};



class YPositionVertexBuffer : public YVertexBuffer
{
public:

	/** Default constructor. */
	YPositionVertexBuffer();

	/** Destructor. */
	~YPositionVertexBuffer();

	/** Delete existing resources */
	void CleanUp();

	/**
	* Initializes the buffer with the given vertices, used to convert legacy layouts.
	* @param InVertices - The vertices to initialize the buffer with.
	*/
	void Init(const TArray<YStaticMeshBuildVertex>& InVertices);
	virtual void InitRHI() override;
	virtual FString GetFriendlyName() const override { return TEXT("PositionOnly Static-mesh vertices"); }
	// Vertex data accessors.
	FORCEINLINE FVector& VertexPosition(uint32 VertexIndex)
	{
		checkSlow(VertexIndex < GetNumVertices());
		return ((YPositionVertex*)(Data + VertexIndex * Stride))->Position;
	}
	FORCEINLINE const FVector& VertexPosition(uint32 VertexIndex) const
	{
		checkSlow(VertexIndex < GetNumVertices());
		return ((YPositionVertex*)(Data + VertexIndex * Stride))->Position;
	}
	// Other accessors.
	FORCEINLINE uint32 GetStride() const
	{
		return Stride;
	}
	FORCEINLINE uint32 GetNumVertices() const
	{
		return NumVertices;
	}
	void Serialize(FArchive& Ar, bool bNeedsCPUAccess);
private:

	/** The vertex data storage type */
	TUniquePtr<YPositionVertexData> VertexData;
	/** The cached vertex data pointer. */
	uint8* Data;

	/** The cached vertex stride. */
	// 因为bNdeedsCPUAccess如果是False的话，vertexData中的内容会被清空，所以要把stride，NumVertices存下来
	uint32 Stride;

	/** The cached number of vertices. */
	uint32 NumVertices;

	/** Allocates the vertex data storage type. */
	void AllocateData(bool bNeedsCPUAccess = true);
};

struct YStaticMeshTangnetVertexData
{
	YPackedNormal TangentX;
	YPackedNormal TangentZ;
	FORCEINLINE FVector GetTangentX() const
	{
		return TangentX;
	}

	FORCEINLINE FVector4 GetTangentZ() const
	{
		return TangentZ;
	}

	FORCEINLINE FVector GetTangentY() const
	{
		FVector  TanX = GetTangentX();
		FVector4 TanZ = GetTangentZ();

		return (FVector(TanZ) ^ TanX) * TanZ.W;
	}

	FORCEINLINE void SetTangents(FVector X, FVector Y, FVector Z)
	{
		TangentX = X;
		TangentZ = FVector4(Z, YGetBasisDeterminantSign(X, Y, Z));
	}
};

struct YStaticMeshUVVertexData
{
	FVector2DHalf UVs[2];
	FORCEINLINE FVector2D GetUV(uint32 TexCoordIndex) const
	{
		check(TexCoordIndex < 2);

		return UVs[TexCoordIndex];
	}

	FORCEINLINE void SetUV(uint32 TexCoordIndex, FVector2D UV)
	{
		check(TexCoordIndex < 2);

		UVs[TexCoordIndex] = UV;
	}
};

struct YStaticMeshTangentUVElement :public YStaticMeshTangnetVertexData, public YStaticMeshUVVertexData
{
	/**
	* Serializer
	*
	* @param Ar - archive to serialize with
	* @param V - vertex to serialize
	* @return archive that was used
	*/
	friend FArchive& operator<<(FArchive& Ar, YStaticMeshTangentUVElement& Vertex)
	{
		Ar << Vertex.TangentX;
		Ar << Vertex.TangentZ;

		for (uint32 UVIndex = 0; UVIndex < 2; UVIndex++)
		{
			Ar << Vertex.UVs[UVIndex];
		}

		return Ar;
	}
};

class YStaticMeshTangentUVVertexData :public YStaticMeshVertexData<YStaticMeshTangentUVElement>
{
public:
	YStaticMeshTangentUVVertexData(bool InNeedsCUPAccess = false) :
		YStaticMeshVertexData<YStaticMeshTangentUVElement>(InNeedsCUPAccess)
	{

	}
};


class YStaticMeshTangentUVVertexBuffer :public YVertexBuffer
{
	YStaticMeshTangentUVVertexBuffer();
	~YStaticMeshTangentUVVertexBuffer();
	void CleanUp();
	void Init(const TArray<YStaticMeshBuildVertex>& InVertices);
	void Serialize(FArchive& Ar, bool bNeedsCPUAccess);
	FORCEINLINE FVector4 VertexTangentX(uint32 VertexIndex) const
	{
	}
	FORCEINLINE FVector VertexTangentZ(uint32 VertexIndex) const
	{
	}
	FORCEINLINE FVector VertexTangentY(uint32 VertexIndex) const
	{
	}
	FORCEINLINE void SetVertexTangents(uint32 VertexIndex, FVector X, FVector Y, FVector Z)
	{
	}
	FORCEINLINE void SetVertexUV(uint32 VertexIndex, uint32 UVIndex, const FVector2D& Vec2D)
	{
	}
	FORCEINLINE FVector2D GetVertexUV(uint32 VertexIndex, uint32 UVIndex) const
	{
	}
	FORCEINLINE uint32 GetStride() const
	{
		return Stride;
	}

	FORCEINLINE uint32 GetNumVertices() const
	{
		return NumVertices;
	}

	FORCEINLINE uint32 GetNumTexCoords() const
	{
		return 2;
	}

	const uint8* GetRawVertexData() const
	{
		check(Data != NULL);
		return Data;
	}

	virtual void InitRHI() override;
	virtual FString GetFriendlyName() const override { return TEXT("Static-mesh vertices"); }
private:

	/** The vertex data storage type */
	TUniquePtr<YStaticMeshTangentUVVertexData> VertexData;

	/** The number of texcoords/vertex in the buffer. */
	//uint32 NumTexCoords;

	/** The cached vertex data pointer. */
	uint8* Data;

	/** The cached vertex stride. */
	uint32 Stride;

	/** The cached number of vertices. */
	uint32 NumVertices;

	/** Corresponds to UStaticMesh::UseFullPrecisionUVs. if true then 32 bit UVs are used */
	bool bUseFullPrecisionUVs;

	/** If true then RGB10A2 is used to store tangent else RGBA8 */
	bool bUseHighPrecisionTangentBasis;

	/** Allocates the vertex data storage type. */
	void AllocateData(bool bNeedsCPUAccess = true);
};

namespace EYIndexBufferStride
{
	enum Type
	{
		/** Forces all indices to be 16-bit. */
		Force16Bit = 1,
		/** Forces all indices to be 32-bit. */
		Force32Bit = 2,
		/** Use 16 bits unless an index exceeds MAX_uint16. */
		AutoDetect = 3
	};
}

/**
 * An array view in to a static index buffer. Allows access to the underlying
 * indices regardless of their type without a copy.
 */
class YIndexArrayView
{
public:
	/** Default constructor. */
	YIndexArrayView()
		: UntypedIndexData(NULL)
		, NumIndices(0)
		, b32Bit(false)
	{
	}

	/**
	 * Initialization constructor.
	 * @param InIndexData	A pointer to untyped index data.
	 * @param InNumIndices	The number of indices stored in the untyped index data.
	 * @param bIn32Bit		True if the data is stored as an array of uint32, false for uint16.
	 */
	YIndexArrayView(const void* InIndexData, int32 InNumIndices, bool bIn32Bit)
		: UntypedIndexData(InIndexData)
		, NumIndices(InNumIndices)
		, b32Bit(bIn32Bit)
	{
	}

	/** Common array access semantics. */
	uint32 operator[](int32 i) { return (uint32)(b32Bit ? ((const uint32*)UntypedIndexData)[i] : ((const uint16*)UntypedIndexData)[i]); }
	uint32 operator[](int32 i) const { return (uint32)(b32Bit ? ((const uint32*)UntypedIndexData)[i] : ((const uint16*)UntypedIndexData)[i]); }
	FORCEINLINE int32 Num() const { return NumIndices; }

private:
	/** Pointer to the untyped index data. */
	const void* UntypedIndexData;
	/** The number of indices stored in the untyped index data. */
	int32 NumIndices;
	/** True if the data is stored as an array of uint32, false for uint16. */
	bool b32Bit;
};


class YRawStaticIndexBuffer :public YIndexBuffer
{
	YRawStaticIndexBuffer(bool InNeedsCPUAccess = false);
	void SetIndices(const TArray<uint32>& InIndices, EYIndexBufferStride::Type DesiredStride);
	void GetCopy(TArray<uint32>& OutIndices) const;
	YIndexArrayView GetArrayView() const;
	FORCEINLINE int32 GetNumIndices() const
	{
		return b32Bit ? (IndexStorage.Num() / 4) : (IndexStorage.Num() / 2);
	}
	FORCEINLINE uint32 GetAllocatedSize() const
	{
		return IndexStorage.GetAllocatedSize();
	}
	void Serialize(FArchive& Ar, bool bNeedsCPUAccess);

	// FRenderResource interface.
	virtual void InitRHI() override;

	inline bool Is32Bit() const { return b32Bit; }

	const uint8* GetRawBuffer() { return &IndexStorage[0]; }
private:
	/** Storage for indices. */
	TResourceArray<uint8, INDEXBUFFER_ALIGNMENT> IndexStorage;
	/** 32bit or 16bit? */
	bool b32Bit = false;
};

