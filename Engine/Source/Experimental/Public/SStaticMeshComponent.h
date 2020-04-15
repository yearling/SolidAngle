#pragma once
#include "Core.h"
#include "SComponent.h"
#include "SStaticMesh.h"
DECLARE_LOG_CATEGORY_EXTERN(logSStaticMeshCompoent, Log, All)
class SStaticMeshComponent :public SSceneComponent
{
public:
	static constexpr  bool IsInstance()
	{
		return true;
	};
	SStaticMeshComponent();
	virtual bool PostLoadOp();
	virtual void UpdateBound();
	TRefCountPtr<SStaticMesh> GetStaticMesh()const;
	TRefCountPtr<SStaticMesh> StaticMesh;
	bool IsVisiable;
	virtual bool LoadFromJson(const TSharedPtr<FJsonObject>&RootJson);
	TArray<TRefCountPtr<SMaterialInstance>> Materials;
};
