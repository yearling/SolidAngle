#pragma once
#include "CoreTypes.h"
#include "Containers/ContainersFwd.h"
// Basic types
class FName;
class FExec;
class FArchive;
class FOutputDevice;
class FFeedbackContext;
struct FDateTime;
struct FGuid;

// Math
class FSphere;
struct FBox;
struct FBox2D;
struct FColor;
struct YMath;
struct FMatrix;
struct FPlane;
struct FQuat;
struct FRotator;
struct FTransform;
struct FVector;
struct YVector2D;
struct FVector4;
struct YBoxSphereBounds;
struct YIntPoint;
struct YIntRect;

// Misc
struct FResourceSizeEx;
class IConsoleVariable;
class FRunnableThread;
class FEvent;
class FPlatformFile;
class FMalloc;
class FFileHandle;
class FAutomationTestBase;
struct FGenericMemoryStats;
class FSHAHash;
class FScriptArray;
class FThreadSafeCounter;
enum class EModuleChangeReason;
struct FManifestContext;
class IConsoleObject;
class FConfigFile;
class FConfigSection;

// Text
class FText;
class FTextFilterString;
enum class ETextFilterTextComparisonMode : uint8;
enum class ETextFilterComparisonOperation : uint8;

class UObject;