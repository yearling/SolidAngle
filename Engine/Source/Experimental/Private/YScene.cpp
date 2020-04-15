#include "YScene.h"
YScene::YScene()
{

}


void YScene::RegisterToScene(TRefCountPtr<SStaticMesh> InMesh)
{
	StaticMeshArray.Add(InMesh);
	//opaque
	for (int32 LODIndex = 0; LODIndex < InMesh->RenderData->LODResources.Num(); ++LODIndex)
	{
		YStaticMeshLODResources& LODResource = InMesh->RenderData->LODResources[LODIndex];
		TArray<YStaticMeshSection>& Sections = LODResource.Sections;
		for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex)
		{
			YStaticMeshSection& Section = Sections[SectionIndex];
			int32 MaterialIndex = Section.MaterialIndex;
			//YMaterialInterface* Material = InMesh->StaticMaterials[MaterialIndex].MaterialInterface;
			YMaterialInterface* Material = nullptr;
			{
				YMeshBatch* MeshBatch = new(StaticMeshOpaquePrimitives)YMeshBatch();
				//没看明白UE这点的设计，在FbxStaticMesh Export中，GatherRenderMesh的时候，按照材质的索引把三角形给压实了。也就是说一个材质对应的是这个StaticMesh所有的三角形。
				//不存在同一个材质，会有两个以上的Section.
				//但是：在编辑器中可以赋值为同一个材质。
				MeshBatch->Material = Material;
				MeshBatch->VertexPositionBuffer = &LODResource.PositionVertexBuffer;
				MeshBatch->VertexBuffer = &LODResource.VertexBuffer;
				MeshBatch->Elements[0].FirstIndex = Section.FirstIndex;
				MeshBatch->Elements[0].NumPrimitives = Section.NumTriangles;
				//MeshBatch->Elements[0].BaseVertexIndex = Section.FirstIndex;
				MeshBatch->Elements[0].MinVertexIndex = Section.MinVertexIndex;
				MeshBatch->Elements[0].MaxVertexIndex = Section.MaxVertexIndex;
				MeshBatch->Elements[0].IndexBuffer = &LODResource.IndexBuffer;
			}
		}
	}
	//translucent
}

void YScene::RegisterToScene(SStaticMeshComponent *MeshCompoent)
{
	check(MeshCompoent&& MeshCompoent->StaticMesh);
	if (!MeshCompoent->IsVisiable)
		return;
	TRefCountPtr<SStaticMesh> &InMesh = MeshCompoent->StaticMesh;
	//opaque
	for (int32 LODIndex = 0; LODIndex < InMesh->RenderData->LODResources.Num(); ++LODIndex)
	{
		YStaticMeshLODResources& LODResource = InMesh->RenderData->LODResources[LODIndex];
		TArray<YStaticMeshSection>& Sections = LODResource.Sections;
		for (int32 SectionIndex = 0; SectionIndex < Sections.Num(); ++SectionIndex)
		{
			YStaticMeshSection& Section = Sections[SectionIndex];
			int32 MaterialIndex = Section.MaterialIndex;
			//YMaterialInterface* Material = InMesh->StaticMaterials[MaterialIndex].MaterialInterface;
			YMaterialInterface* Material = MeshCompoent->Materials[MaterialIndex].GetReference();
			{
				YMeshBatch* MeshBatch = new(StaticMeshOpaquePrimitives)YMeshBatch();
				//没看明白UE这点的设计，在FbxStaticMesh Export中，GatherRenderMesh的时候，按照材质的索引把三角形给压实了。也就是说一个材质对应的是这个StaticMesh所有的三角形。
				//不存在同一个材质，会有两个以上的Section.
				//但是：在编辑器中可以赋值为同一个材质。
				MeshBatch->Material = Material;
				MeshBatch->LocalToWorld = MeshCompoent->GetComponentTransform();
				MeshBatch->VertexPositionBuffer = &LODResource.PositionVertexBuffer;
				MeshBatch->VertexBuffer = &LODResource.VertexBuffer;
				MeshBatch->Elements[0].FirstIndex = Section.FirstIndex;
				MeshBatch->Elements[0].NumPrimitives = Section.NumTriangles;
				//MeshBatch->Elements[0].BaseVertexIndex = Section.FirstIndex;
				MeshBatch->Elements[0].MinVertexIndex = Section.MinVertexIndex;
				MeshBatch->Elements[0].MaxVertexIndex = Section.MaxVertexIndex;
				MeshBatch->Elements[0].IndexBuffer = &LODResource.IndexBuffer;
			}
		}
	}
}

void YScene::Clear()
{
	StaticMeshArray.Empty();
	StaticMeshOpaquePrimitives.Empty();
	StaticMeshMaskPrimitives.Empty();
	StaticMeshTrunslucentPrimitives.Empty();
}

