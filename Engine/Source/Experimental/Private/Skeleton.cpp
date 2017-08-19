#include "Skeleton.h"

Skeleton::Skeleton()
	:RootBone(Bone::InvalidBone)
{

}

std::pair<bool, Bone::BoneIDType> Skeleton::AddBone(const std::string & BoneName)
{
	if (BoneNameToBoneID.find(BoneName) != BoneNameToBoneID.end())
	{
		return std::make_pair<>(false, Bone::InvalidBone);
	}
	else
	{
		Bones.emplace_back(Bone(BoneName));
		Bone::BoneIDType BoneId = Bones.size() - 1;
		Bones[BoneId].BoneID = BoneId;
		BoneNameToBoneID.insert(std::make_pair<>(BoneName, BoneId));
		return std::make_pair<>(true, BoneId);
	}
}

const Bone* Skeleton::GetBone(const std::string &BoneName) const
{
	auto Iter = BoneNameToBoneID.find(BoneName);
	if (Iter == BoneNameToBoneID.end())
	{
		return nullptr;
	}
	else
	{
		return &Bones[Iter->second];
	}
}

Bone* Skeleton::GetBone(const std::string &BoneName)
{
	auto Iter = BoneNameToBoneID.find(BoneName);
	if (Iter == BoneNameToBoneID.end())
	{
		return nullptr;
	}
	else
	{
		return &Bones[Iter->second];
	}
}

const Bone& Skeleton::GetBone(Bone::BoneIDType BoneID) const
{
	return Bones[BoneID];
}

Bone& Skeleton::GetBone(Bone::BoneIDType BoneID)
{
	return Bones[BoneID];
}

Bone::BoneIDType Skeleton::GetBoneID(const std::string &BoneName) const
{
	auto Iter = BoneNameToBoneID.find(BoneName);
	if (Iter == BoneNameToBoneID.end())
	{
		return Bone::InvalidBone;
	}
	else
	{
		return Iter->second;
	}
}

const int Bone::InvalidBone;

Bone::Bone(const std::string & InBoneName)
	: BoneName(InBoneName)
{

}

Bone::Bone()
	:BoneID(Bone::InvalidBone)
	,ParentBoneID(Bone::InvalidBone)
{
	MatParentToBone = XMMatrixIdentity();
	MatLocalTransform = XMMatrixIdentity();
}
