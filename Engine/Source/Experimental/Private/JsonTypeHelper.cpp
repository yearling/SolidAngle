#include "JsonTypeHelper.h"

bool ConverJsonValueToFVector(const TSharedPtr<FJsonValue>& RootObject, FVector& InOutVector)
{
	check(RootObject->Type == EJson::Array);
	if (RootObject.IsValid() && RootObject->Type == EJson::Array)
	{
		auto& Array = RootObject->AsArray();
		if (Array.Num() < 3)
			return false;
		InOutVector.X = Array[0]->AsNumber();
		InOutVector.Y = Array[1]->AsNumber();
		InOutVector.Z = Array[2]->AsNumber();
		return true;
	}
	else
	{
		return false;
	}
}

bool ConverJsonValueToFRotator(const TSharedPtr<FJsonValue>& RootObject, FRotator& InOutVector)
{
	check(RootObject->Type == EJson::Array);
	if (RootObject.IsValid() && RootObject->Type == EJson::Array)
	{
		auto& Array = RootObject->AsArray();
		if (Array.Num() < 3)
			return false;
		InOutVector.Pitch = Array[0]->AsNumber();
		InOutVector.Yaw = Array[1]->AsNumber();
		InOutVector.Roll = Array[2]->AsNumber();
		return true;
	}
	else
	{
		return false;
	}
}

