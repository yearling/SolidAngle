#pragma once

#include "CoreTypes.h"
#include "Templates/SharedPointer.h"

class FCulture;

typedef TSharedPtr<FCulture, ESPMode::ThreadSafe> FCulturePtr;
typedef TSharedRef<FCulture, ESPMode::ThreadSafe> FCultureRef;
