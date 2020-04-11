#pragma once
#include "Core.h"
#include "SObject.h"
class SComponent :public SObject
{
public:
	enum EComponentType
	{
		Base,
		SceneComponent,
		StaticMeshComponent,
		LightComponenet
	};
	explicit SComponent(EComponentType Type) :ComponentType(Type) {}
protected:
	EComponentType ComponentType;
};

class SSceneComponent :public SComponent
{
public:
	SSceneComponent():SComponent(EComponentType::SceneComponent){}
	/** Current bounds of the component */
	FBoxSphereBounds Bounds;
	/** Location of the component relative to its parent */
	FVector RelativeLocation;

	/** Rotation of the component relative to its parent */
	FRotator RelativeRotation;
	/**
	*	Non-uniform scaling of the component relative to its parent.
	*	Note that scaling is always applied in local space (no shearing etc)
	*/
	FVector RelativeScale3D;
	SSceneComponent* ParentCompnent = nullptr;
	TArray<SSceneComponent*> ChildrenComponent;
	virtual void UpdateComponentToWorld();
	/** Sets the cached component to world directly. This should be used very rarely. */
	FORCEINLINE void SetComponentToWorld(const FTransform& NewComponentToWorld)
	{
		bComponentToWorldUpdated = true;
		ComponentToWorld = NewComponentToWorld;
	}

	/**
	 * Get the current component-to-world transform for this component
	 * TODO: probably deprecate this in favor of GetComponentTransform
	 */
	FORCEINLINE const FTransform& GetComponentToWorld() const
	{
		return ComponentToWorld;
	}

	/** Get the current component-to-world transform for this component */
	FORCEINLINE const FTransform& GetComponentTransform() const
	{
		return ComponentToWorld;
	}
	void UpdateComponentToWorldWithParentRecursive();
	void PropagateTransformUpdate();
	virtual void UpdateBound();
	void UpdateChildTransforms();
private:
	/** Current transform of the component, relative to the world */
	FTransform ComponentToWorld;
	bool bComponentToWorldUpdated = false;
};
