#pragma once
#include "YYUT.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
class Bone
{
public:
	Bone();
	Bone(const Bone&) = default;
	Bone(Bone&&) = default;
	Bone(const std::string & InBoneName);
	typedef int BoneIDType;
	static const int  InvalidBone = (int)-1;
public:
	std::string BoneName;
	BoneIDType			BoneID;
	BoneIDType			ParentBoneID;
	std::vector<BoneIDType>   ChildBones;
	FMatrix     FMatParentToBone;
	FMatrix		FMatLocalTransform;
	FMatrix     FMatCurParent;
	XMMATRIX	MatParentToBone;
	XMMATRIX	MatLocalTransform;
};

class ENGINE_API Skeleton
{
public:
	Skeleton();
	Skeleton(const Skeleton&) = default;
	Skeleton(Skeleton&&) = default;
	std::pair<bool, Bone::BoneIDType>	AddBone(const std::string & BoneName);
	const Bone* GetBone(const std::string &BoneName) const;
	Bone*		GetBone(const std::string &BoneName);
	const Bone& GetBone(Bone::BoneIDType BoneID)const ;
	Bone&		GetBone(Bone::BoneIDType BoneID);
	Bone::BoneIDType GetBoneID(const std::string &BoneName) const;
	
public:
	std::vector<Bone> Bones;
	std::unordered_map<std::string, Bone::BoneIDType> BoneNameToBoneID;
	Bone::BoneIDType		RootBone;
};

struct FVirtualBone
{
public:
	FName SourceBoneName;
	FName TargetBoneName;
	FName VirtualBoneName;

	FVirtualBone() {}

	FVirtualBone(FName InSource, FName InTarget)
		: SourceBoneName(InSource)
		, TargetBoneName(InTarget)
	{
		// VB Prefix including space after VB so that it will never collide with 
		// an actual bone name (as they cannot contain spaces)
		static FString VirtualBonePrefix(TEXT("VB "));

		VirtualBoneName = FName(*(VirtualBonePrefix + SourceBoneName.ToString() + TEXT("_") + TargetBoneName.ToString()));
	}
};
class ENGINE_API YSkeleton
{
public:
	const TArray<FVirtualBone>& GetVirtualBones() const { return VirtualBones; }
	/**
	*  Array of this skeletons virtual bones. These are new bones are links between two existing bones
	*  and are baked into all the skeletons animations
	*/
	TArray<FVirtualBone> VirtualBones;
};