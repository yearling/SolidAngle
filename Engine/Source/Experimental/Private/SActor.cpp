#include "SActor.h"
#include "SComponent.h"
DEFINE_LOG_CATEGORY(LogSActor);

SActor::SActor()
{
}

SActor::~SActor()
{

}

bool SActor::LoadFromJson(const TSharedPtr<FJsonObject>&RootJson)
{
	auto& JsonComponnets = RootJson->Values["Components"]->AsObject();
	for (auto& KeyPair : JsonComponnets->Values)
	{
		TRefCountPtr<SComponent> NewComponent = SComponent::LoadFromNamedJson(KeyPair.Key, KeyPair.Value->AsObject());
		Components.Add(NewComponent);
	}
	return true;
}


bool SActor::PostLoadOp()
{
	bool bSuccess = true;
	for (TRefCountPtr<SComponent>& Component : Components)
	{
		bSuccess &= Component->PostLoadOp();
	}
	return bSuccess;
}
