#include "SComponent.h"

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
			NewTransform = RelativeTransform * ParentCompnent->GetComponentToWorld();
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
	for (SSceneComponent* child : ChildrenComponent)
	{
		child->UpdateComponentToWorld();
	}
}

