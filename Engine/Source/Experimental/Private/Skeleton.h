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
	XMMATRIX    MatParentToBone;
	XMMATRIX	MatLocalTransform;
	XMMATRIX    MatCurParent;

};

class Skeleton
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