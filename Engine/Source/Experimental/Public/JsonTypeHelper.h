#pragma once
#include "Core.h"
#include "Json.h"

bool ConverJsonValueToFVector(const TSharedPtr<FJsonValue>& RootObject, FVector& InOutVector);
bool ConverJsonValueToFRotator(const TSharedPtr<FJsonValue>& RootObject, FRotator& InOutVector);
