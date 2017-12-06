# UESkeleton 

## FReferenceSkeleton
### 主要成员

	TArray<FMeshBoneInfo> RawRefBoneInfo; //Bones that exist in the original asset, 其中 FMeshBoneInfo{ BoneName, ParentIndex}
	TArray<FTransform> RawRefBonePose;
	TArray<FMeshBoneInfo>	FinalRefBoneInfo;//Bones for this skeleton including user added virtual bones，在RawBone的基础上添加Virtal Bones. 
	TArray<FTransform>		FinalRefBonePose;
	TMap<FName, int32>		RawNameToIndexMap;
	TMap<FName, int32>		FinalNameToIndexMap;

### FSkeletalMesh 
    /** New Reference skeleton type **/
	FReferenceSkeleton RefSkeleton;

	/** Reference skeleton precomputed bases. */
	TArray<FMatrix> RefBasesInvMatrix;


### 使用过程
    
	|- ImportSkeletalMesh
		|- ProcessImportMeshSkeleton
			|-ProcessImportMeshSkeleton 
				|- 通过从FBX中导入的FSkeletalMeshImportData::RefBoneBinary来创建FReferenceSkeleton. 