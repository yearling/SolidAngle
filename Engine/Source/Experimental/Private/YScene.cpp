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
				//û������UE������ƣ���FbxStaticMesh Export�У�GatherRenderMesh��ʱ�򣬰��ղ��ʵ������������θ�ѹʵ�ˡ�Ҳ����˵һ�����ʶ�Ӧ�������StaticMesh���е������Ρ�
				//������ͬһ�����ʣ������������ϵ�Section.
				//���ǣ��ڱ༭���п��Ը�ֵΪͬһ�����ʡ�
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
				//û������UE������ƣ���FbxStaticMesh Export�У�GatherRenderMesh��ʱ�򣬰��ղ��ʵ������������θ�ѹʵ�ˡ�Ҳ����˵һ�����ʶ�Ӧ�������StaticMesh���е������Ρ�
				//������ͬһ�����ʣ������������ϵ�Section.
				//���ǣ��ڱ༭���п��Ը�ֵΪͬһ�����ʡ�
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

