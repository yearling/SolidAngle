#pragma once

#include "Core.h"
#include "SObject.h"
#include "SActor.h"
#include "YScene.h"
DECLARE_LOG_CATEGORY_EXTERN(LogSSworld, Log, All)
class SWorld :public SObject
{
public:
	SWorld();
	virtual ~SWorld();
	static constexpr bool IsInstance() { return false; };
	virtual bool LoadFromPackage(const FString & Path);
	virtual bool PostLoadOp();
	void UpdateToScene();
	TArray<TRefCountPtr<SActor>> Actors;
	TRefCountPtr<YScene> CurrentScene;
};