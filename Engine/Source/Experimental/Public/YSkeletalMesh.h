#pragma once
#include "Core.h"
#include "MeshCommon.h"
#include "Skeleton.h"
#include "ReferenceSkeleton.h"

struct FSkeletalMaterial
{

		FSkeletalMaterial()
		: MaterialInterface(NULL)
		, bEnableShadowCasting_DEPRECATED(true)
		, bRecomputeTangent_DEPRECATED(false)
		, MaterialSlotName(NAME_None)
	{

	}

	FSkeletalMaterial(class UMaterialInterface* InMaterialInterface
		, bool bInEnableShadowCasting = true
		, bool bInRecomputeTangent = false
		, FName InMaterialSlotName = NAME_None
		, FName InImportedMaterialSlotName = NAME_None)
		: MaterialInterface(InMaterialInterface)
		, bEnableShadowCasting_DEPRECATED(bInEnableShadowCasting)
		, bRecomputeTangent_DEPRECATED(bInRecomputeTangent)
		, MaterialSlotName(InMaterialSlotName)
	{

	}

	friend FArchive& operator<<(FArchive& Ar, FSkeletalMaterial& Elem);

	 friend bool operator==(const FSkeletalMaterial& LHS, const FSkeletalMaterial& RHS);
	 friend bool operator==(const FSkeletalMaterial& LHS, const UMaterialInterface& RHS);
	 friend bool operator==(const UMaterialInterface& LHS, const FSkeletalMaterial& RHS);

		class UMaterialInterface *	MaterialInterface;
		bool						bEnableShadowCasting_DEPRECATED;
		bool						bRecomputeTangent_DEPRECATED;

	/*This name should be use by the gameplay to avoid error if the skeletal mesh Materials array topology change*/
		FName						MaterialSlotName;
		FMeshUVChannelInfo			UVChannelData;
};

class YSkeletalMesh
{
public:
	YSkeletalMesh();
	virtual ~YSkeletalMesh();
	YSkeleton* Skeleton;
	FReferenceSkeleton RefSkeleton;
	TArray<FSkeletalMaterial> Materials;
};

