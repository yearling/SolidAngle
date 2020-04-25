#include "SComponent.h"
#include "SObjectManager.h"
#include "SStaticMeshComponent.h"
#include "JsonTypeHelper.h"
void SSceneComponent::UpdateComponentToWorld()
{
	UpdateComponentToWorldWithParentRecursive();
}

void SSceneComponent::UpdateComponentToWorldWithParentRecursive()
{
	if (ParentCompnent && !ParentCompnent->bComponentToWorldUpdated)
	{
		ParentCompnent->UpdateComponentToWorld();
		if (bComponentToWorldUpdated)
		{
			return;
		}
	}
	bComponentToWorldUpdated = true;
	FTransform NewTransform(NoInit);
	{
		const FTransform RelativeTransform(RelativeRotation, RelativeLocation, RelativeScale3D);
		if (ParentCompnent)
		{
			NewTransform = RelativeTransform * ParentCompnent->GetComponentTransform();
		}
		else
		{
			NewTransform = RelativeTransform;
		}
	}

	ComponentToWorld = NewTransform;
	PropagateTransformUpdate();
}

void SSceneComponent::PropagateTransformUpdate()
{
	UpdateBound();
	UpdateChildTransforms();
}

void SSceneComponent::UpdateBound()
{
	Bounds.BoxExtent = FVector(0, 0, 0);
	Bounds.Origin = ComponentToWorld.GetTranslation();
	Bounds.SphereRadius = 0.0;
}

void SSceneComponent::UpdateChildTransforms()
{
	for (SSceneComponent* child : ChildrenComponents)
	{
		child->UpdateComponentToWorld();
	}
}


bool SSceneComponent::LoadFromJson(const TSharedPtr<FJsonObject>&RootJson)
{
	bool bSuccess = true;
	bSuccess &= ConverJsonValueToFVector(RootJson->Values["Scale"], RelativeScale3D);
	bSuccess &= ConverJsonValueToFRotator(RootJson->Values["Rotation"], RelativeRotation);
	bSuccess &= ConverJsonValueToFVector(RootJson->Values["Translation"], RelativeLocation);
	auto Children = RootJson->Values.Find("Children");
	if (Children&& Children->IsValid())
	{
		auto& JsonChildren = (*Children)->AsArray();
		for (auto& Child : JsonChildren)
		{
			auto& ChildComponetJson = Child->AsObject();
			for (auto& KeyPair : ChildComponetJson->Values)
			{
				TRefCountPtr<SComponent> NewComponent = SComponent::LoadFromNamedJson(KeyPair.Key, KeyPair.Value->AsObject());
				int32 AddIndex = ChildrenComponents.Add(TRefCountPtr<SSceneComponent>(dynamic_cast<SSceneComponent*>(NewComponent.GetReference())));
				ChildrenComponents[AddIndex]->ParentCompnent = this;
			}
		}
	}
	return bSuccess;
}

bool SSceneComponent::PostLoadOp()
{
	UpdateComponentToWorld();
	bool bChildSuccess = true;
	for (TRefCountPtr<SSceneComponent>& Child : ChildrenComponents)
	{
		bChildSuccess &= Child->PostLoadOp();
	}
	return bChildSuccess;
}

//Load Utilities


TRefCountPtr<SComponent> SComponent::LoadFromNamedJson(const FString& ComponentName, const TSharedPtr<FJsonObject>& RootJson)
{
	if (ComponentName == TEXT("SceneComponent"))
	{
		TRefCountPtr<SSceneComponent>  SceneComponent = SObjectManager::ConstructInstance<SSceneComponent>();
		if (!SceneComponent->LoadFromJson(RootJson))
		{
			return TRefCountPtr<SComponent>(nullptr);
		}
		return TRefCountPtr<SComponent>(SceneComponent.GetReference());
	}
	else if (ComponentName == TEXT("StaticMeshComponent"))
	{
		TRefCountPtr<SStaticMeshComponent>  StaticMeshComponent = SObjectManager::ConstructInstance<SStaticMeshComponent>();
		if (!StaticMeshComponent->LoadFromJson(RootJson))
		{
			return TRefCountPtr<SComponent>(nullptr);
		}
		return TRefCountPtr<SComponent>(StaticMeshComponent.GetReference());
	}
	return TRefCountPtr<SComponent>(nullptr);
}

SComponent::EComponentType SComponent::GetComponentType() const
{
	return ComponentType;
}
