#pragma once
#include "CoreTypes.h"
#include "Containers/ContainersFwd.h"
// Basic types
class YName;
class YExec;
class YArchive;
class YOutputDevice;
class YFeedbackContext;
struct YDateTime;
struct YGuid;

// Math
class YSphere;
struct YBox;
struct YBox2D;
struct YColor;
struct YLinearColor;
struct YMatrix;
struct YPlane;
struct YQuat;
struct YRotator;
struct YTransform;
struct YVector;
struct YVector2D;
struct YVector4;
struct YBoxSphereBounds;
struct YIntPoint;
struct YIntRect;

// Misc
struct FResourceSizeEx;
class FConsoleVariable;
class FRunnableThread;
class FEvent;
class FPlatformFile;
class YMalloc;
class FFileHandle;
class FAutomationTestBase;
struct YGenericMemoryStats;
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

class SObject;