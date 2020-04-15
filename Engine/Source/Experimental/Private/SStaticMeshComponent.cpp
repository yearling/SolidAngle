#include "Core.h"
#include "SStaticMeshComponent.h"
#include "Json.h"
#include "SObjectManager.h"

DEFINE_LOG_CATEGORY(logSStaticMeshCompoent);
SStaticMeshComponent::SStaticMeshComponent() :IsVisiable(true)
{
	ComponentType = EComponentType::StaticMeshComponent;
}

bool SStaticMeshComponent::PostLoadOp()
{
	SSceneComponent::PostLoadOp();
	StaticMesh->InitRenderResource();
	return true;
}

void SStaticMeshComponent::UpdateBound()
{
	if (GetStaticMesh())
	{
		// Graphics bounds.
		Bounds = GetStaticMesh()->GetBounds().TransformBy(GetComponentTransform());
	}
	else
	{
		SSceneComponent::UpdateBound();
	}
}

TRefCountPtr<SStaticMesh> SStaticMeshComponent::GetStaticMesh() const
{
	return StaticMesh;
}

bool SStaticMeshComponent::LoadFromJson(const TSharedPtr<FJsonObject>&RootJson)
{
	auto& SceneComponetJson = RootJson->Values["SceneComponent"]->AsObject();
	if (!SSceneComponent::LoadFromJson(SceneComponetJson))
	{
		return false;
	}
	FString ModelPackage = RootJson->Values["Model"]->AsString();
	StaticMesh = SObjectManager::ConstructUnifyFromPackage<SStaticMesh>(ModelPackage);
	IsVisiable = RootJson->Values["Visible"]->AsBool();
	auto& MaterialInsArray = RootJson->Values["MaterialInstances"]->AsArray();
	for (auto& MaterialInsObj : MaterialInsArray)
	{
		FString MaterialInsPackagePath = MaterialInsObj->AsString();
		TRefCountPtr<SMaterialInstance> MaterialIns = SObjectManager::ConstructUnifyFromPackage<SMaterialInstance>(MaterialInsPackagePath);
		Materials.Add(MaterialIns);
	}
	return true;
}
