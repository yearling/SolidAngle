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
	template<typename ClassType, typename...T>
	static TRefCountPtr<ClassType> ConstructInstance(T&&... Args)
	{
		check(ClassType::IsInstance());
		TRefCountPtr<ClassType> Obj(new ClassType(Forward<T>(Args)...), true);
		GSObjectManager.InstancedObjects.Add(TRefCountPtr<SObject>(Obj.GetReference(), true));
		return Obj;
	}

	template<typename ClassType, typename...T>
	static TRefCountPtr<ClassType> ConstructUnifyFromPackage(const FString& PackagePath, T&&... Args)
	{
		check(!ClassType::IsInstance());
		FString PackagePathNoSuffix = PackagePath;
		FPaths::NormalizeFilename(PackagePathNoSuffix);
		FName PackageFName(*(FPaths::GetBaseFilename(PackagePathNoSuffix, false)));
		TRefCountPtr<SObject>* FindResult = GSObjectManager.UnifyObjects.Find(PackageFName);
		if (FindResult)
		{
			return TRefCountPtr<ClassType>(dynamic_cast<ClassType*>((*FindResult).GetReference()), true);
		}
		else
		{
			TRefCountPtr<ClassType> Obj((new ClassType(Forward<T>(Args)...)), true);
			if (Obj->LoadFromPackage(PackagePath))
			{
				GSObjectManager.UnifyObjects.Add(PackageFName, TRefCountPtr<SObject>(Obj.GetReference(), true));
				return Obj;
			}
			else
			{
				return nullptr;
			}
		}
	}

	//template<typename ClassType,typename Pa, typename...T>
	//static TRefCountPtr<ClassType> ConstructUnifyFromPackage(Pa Var, T&&... Args)
	//{
	//	return	ConstructUnifyFromPackage<ClassType>(Var, Forward<T>(Args...));
	//}

	void Destroy();
	void FrameDestroy();

private:
	TMap<FName, TRefCountPtr<SObject>> UnifyObjects;
	TSet<TRefCountPtr<SObject>> InstancedObjects;
	TSet<SObject*> FuckSet;
};