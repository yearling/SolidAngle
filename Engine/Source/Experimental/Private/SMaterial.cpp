#include "SMaterial.h"
#include "Json.h"
#include "SObjectManager.h"
#include "Misc\FileHelper.h"
DEFINE_LOG_CATEGORY(LogSMaterial);

SMaterial::SMaterial()
{

}

SMaterial::~SMaterial()
{

}

FArchive& SMaterial::Serialize(FArchive& Ar)
{
	return Ar;
}

bool SMaterial::LoadFromPackage(const FString & Path)
{
	if (FPaths::GetExtension(Path).Equals(TEXT("json"), ESearchCase::IgnoreCase))
	{
		FString JsonTxt;
		bool bLoad = FFileHelper::LoadFileToString(JsonTxt, *Path);
		if (!bLoad)
		{
			UE_LOG(LogSMaterial, Log, TEXT("file: %s not exist"), *Path);
			return false;
		}
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonTxt);
		TSharedPtr<FJsonObject> RootObject;
		bool bSuccessful = FJsonSerializer::Deserialize(JsonReader, RootObject);
		if (!bSuccessful)
		{
			FString ErrMsg = JsonReader->GetErrorMessage();
			UE_LOG(LogSMaterial, Log, TEXT("Json parse failed! \n %s : %s"), *Path, *ErrMsg);
			return false;
		}
		RootObject->Values.Find(TEXT("RenderState"));
		auto* Parameters = RootObject->Values.Find(TEXT("Parameters"));
		if (Parameters && (*Parameters)->Type == EJson::Array)
		{
			auto& InnerArray = (*Parameters)->AsArray();
			for (int32 i = 0; i < InnerArray.Num(); ++i)
			{
				auto& ParameterItem = InnerArray[i]->AsObject();
				{
					if (ParameterItem->Values["Type"]->AsString() == TEXT("Textrue2D"))
					{
						TRefCountPtr< STexture> Texture = SObjectManager::ConstructInstance<STexture>();
						FString TexturePath = ParameterItem->Values["Value"]->AsString();
						Texture->TexturePackagePath = *TexturePath;
						FName ParameterName = *ParameterItem->Values["Name"]->AsString();
						Texture->TextureType = ETextureType::ETT_2D;
						TextureParameters.Add(ParameterName, Texture);
					}
				}
			}
		}
	}
	return true;
}

void SMaterial::SaveToPackage(const FString & Path)
{
}

void SMaterial::AddMaterialTextureParameter(FName ParameterName, TRefCountPtr<STexture> Texture)
{
	TextureParameters.FindOrAdd(ParameterName) = Texture;
}

SMaterialInstance::SMaterialInstance()
{

}

SMaterialInstance::~SMaterialInstance()
{

}
