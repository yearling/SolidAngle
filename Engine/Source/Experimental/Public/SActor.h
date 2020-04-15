#pragma once
#include "Core.h"
#include "SObject.h"
#include "SComponent.h"
DECLARE_LOG_CATEGORY_EXTERN(LogSActor, Log, All)
class SActor :public SObject
{
public:
	SActor();
	virtual ~SActor();
	static constexpr  bool IsInstance() { return true; }
	virtual bool LoadFromJson(const TSharedPtr<FJsonObject>&RootJson);
	virtual bool PostLoadOp();

	template<typename T>
	void RecursiveGetTypeComponent(SComponent* CurrentComponent, SComponent::EComponentType ComponentType, TArray<T*>& InOutSelectComponents)
	{
		if (!CurrentComponent)
			return;

		if (CurrentComponent->GetComponentType() == ComponentType)
		{
			InOutSelectComponents.AddUnique(dynamic_cast<T*>(CurrentComponent));
		}

		SSceneComponent* SceneComponent = dynamic_cast<SSceneComponent*>(CurrentComponent);
		if (SceneComponent)
		{
			for (TRefCountPtr<SSceneComponent>& Child : SceneComponent->ChildrenComponents)
			{
				RecursiveGetTypeComponent(Child.GetReference(), ComponentType, InOutSelectComponents);
			}
		}
	}
	template<typename T>
	void RecurisveGetTypeComponent(SComponent::EComponentType ComponentType, TArray<T*> &InOutSelectComponents)
	{
		for (TRefCountPtr<SComponent>& Component : Components)
		{
			RecursiveGetTypeComponent(Component.GetReference(), ComponentType, InOutSelectComponents);
		}
	}

public:
	TArray<TRefCountPtr<SComponent>> Components;
};
