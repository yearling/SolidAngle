# UESkeleton 

## FReferenceSkeleton
### 主要成员

	TArray<FMeshBoneInfo> RawRefBoneInfo; //Bones that exist in the original asset, 其中 FMeshBoneInfo{ BoneName, ParentIndex}
	TArray<FTransform> RawRefBonePose;
	TArray<FMeshBoneInfo>	FinalRefBoneInfo;//Bones for this skeleton including user added virtual bones，在RawBone的基础上添加Virtal Bones。注意：FinalRefBoneInfo前面都是RawBone，VirtalBone位于RawBone之后
	TArray<FTransform>		FinalRefBonePose;
	TMap<FName, int32>		RawNameToIndexMap;
	TMap<FName, int32>		FinalNameToIndexMap;

### FSkeletalMesh 
    /** New Reference skeleton type **/
	FReferenceSkeleton RefSkeleton;

	/** Reference skeleton precomputed bases. */
	TArray<FMatrix> RefBasesInvMatrix; //大小与RefSkeleton的RawRefBoneInfo的大小一致，存的是骨骼的逆


### 使用过程
    
	|- ImportSkeletalMesh
		|- ProcessImportMeshSkeleton
			|-ProcessImportMeshSkeleton 
				|- 通过从FBX中导入的FSkeletalMeshImportData::RefBoneBinary来创建FReferenceSkeleton. 
		|- Skeleton->MergeAllBonesToBoneTree( SkeletalMesh 
			|- 取SkeletalMesh->RefSkeleton的RawBone来构建Skeletal的RawBone，然后再加上自己的VirtualBone；目的是：SkeletalMesh::FReferenceSkeletal中含有RawBone和VirtualBone,但是virtalbone可以与Skeletal中的virtualBone不一致，但RawBone要保持一致。