#pragma once
#include "Core.h"
#include "SObject.h"
#include "Templates\RefCounting.h"
#include "Containers\Set.h"
extern class SObjectManager GSObjectManager;
class SObjectManager
{
public:
	SObjectManager();
	template<typename ClassType,typename...T>
	static TRefCountPtr<ClassType> ConstructInstance(T&&... Args)
	{
		//assert(ClassType::IsInstance());
		//TRefCountPtr<ClassType> Obj{new ClassType(Forward<T>(Args)...),true};
		//InstancedObjects.Add(Obj);
		TRefCountPtr<ClassType> Obj( new ClassType(Forward<T>(Args)...), true);
		//TRefCountPtr<SObject> OjbInst(new SObject());
		//InstancedObjects.Add(TRefCountPtr<SObject>(Obj.GetReference(),false));
		GSObjectManager.InstancedObjects.Add(TRefCountPtr<SObject>(Obj.GetReference(), true));
		return Obj;
	}
	template<typename ClassType,typename...T>
	static TRefCountPtr<ClassType> ConstructUnifyFromPackage(const FString& PackagePath, T&&... Args)
	{
		static_assert(ClassType::IsInstance());
		FName PackageFName(*PackagePath);
		TRefCountPtr<ClassType>* FindResult = UnifyObjects.Find(PackageFName);
		if (FindResult)
		{
			return *FindResult;
		}
		else
		{
			TRefCountPtr<ClassType> Obj(new ClassType(Forward<T>(Args...)), true);
			if (Obj->LoadFromPackage(PackagePath))
			{
				UnifyObjects.Add(MoveTemp(PackageFName), Obj);
				return Obj;
			}
			else
			{
				return nullptr;
			}
		}
	}

	void Destroy();
	void FrameDestroy();

private:
	TMap<FName, TRefCountPtr<SObject>> UnifyObjects;
	TSet<TRefCountPtr<SObject>> InstancedObjects;
	TSet<SObject*> FuckSet;
};