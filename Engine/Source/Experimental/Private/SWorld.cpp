#include "SWorld.h"
#include "SObjectManager.h"
#include "SStaticMeshComponent.h"

DEFINE_LOG_CATEGORY(LogSSworld);

SWorld::SWorld()
{

}

SWorld::~SWorld()
{

}

bool SWorld::LoadFromPackage(const FString & Path)
{
	if (FPaths::GetExtension(Path).Equals(TEXT("json"), ESearchCase::IgnoreCase))
	{
		FString JsonTxt;
		bool bLoad = FFileHelper::LoadFileToString(JsonTxt, *Path);
		if (!bLoad)
		{
			UE_LOG(LogSSworld, Log, TEXT("file: %s not exist"), *Path);
			return false;
		}
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonTxt);
		TSharedPtr<FJsonObject> RootObject;
		bool bSuccessful = FJsonSerializer::Deserialize(JsonReader, RootObject);
		if (!bSuccessful)
		{
			FString ErrMsg = JsonReader->GetErrorMessage();
			UE_LOG(LogSSworld, Log, TEXT("Json parse failed! \n %s : %s"), *Path, *ErrMsg);
			return false;
		}
		auto& ActorArray = RootObject->Values["Actors"]->AsArray();
		for (auto& ActorJson : ActorArray)
		{
			TRefCountPtr<SActor> NewActor = SObjectManager::ConstructInstance<SActor>();
			if (NewActor->LoadFromJson(ActorJson->AsObject()))
			{
				Actors.Add(NewActor);
			}
		}
	}
	return true;
}

bool SWorld::PostLoadOp()
{
	bool bSuccess = true;
	for (TRefCountPtr<SActor>& Actor : Actors)
	{
		bSuccess &= Actor->PostLoadOp();
	}
	return bSuccess;
}

void SWorld::UpdateToScene()
{
	TArray<SStaticMeshComponent*> StaticMeshComponents;
	for (TRefCountPtr<SActor> &Actor : Actors)
	{
		Actor->RecurisveGetTypeComponent(SComponent::StaticMeshComponent, StaticMeshComponents);
	}
	for (SStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		CurrentScene->RegisterToScene(StaticMeshComponent);
	}
}
