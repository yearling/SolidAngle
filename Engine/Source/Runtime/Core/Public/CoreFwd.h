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
struct YResourceSizeEx;
class YConsoleVariable;
class YRunnableThread;
class YEvent;
class YPlatformFile;
class YMalloc;
class YFileHandle;
class YAutomationTestBase;
struct YGenericMemoryStats;
class YSHAHash;
class YScriptArray;
class YThreadSafeCounter;
enum class EModuleChangeReason;
struct YManifestContext;
class IConsoleObject;
class YConfigFile;
class YConfigSection;

// Text
class YText;
class YTextFilterString;
enum class ETextFilterTextComparisonMode : uint8;
enum class ETextFilterComparisonOperation : uint8;

class SObject;