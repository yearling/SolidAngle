#include "YSkeletalMesh.h"



YSkeletalMesh::YSkeletalMesh()
	:Skeleton(nullptr)
{
	SkelMirrorAxis = EAxis::X;
	SkelMirrorFlipAxis = EAxis::Z;
#if WITH_EDITORONLY_DATA
	SelectedEditorSection = INDEX_NONE;
#endif
	ImportedResource = MakeShareable(new FSkeletalMeshResource());
}


YSkeletalMesh::~YSkeletalMesh()
{
}

