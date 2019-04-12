# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.14)
add_subdirectory(../../ThirdParty/IntelTBB/tbb-2017_U5 ${CMAKE_CURRENT_BINARY_DIR}/IntelTBB)
# include
include_directories("../../../Source/"
                    "Public"
                    "Private"
                    "../InputDevice/Public"
                    "../../ThirdParty/IntelTBB/tbb-2017_U5/include"
                    "Public/Templates"
                    "Public/Windows"
                    "Public/HAL"
                    "Public/Misc"
                    "Public/Async"
                    "Public/Modules"
                    "Public/GenericPlatform"
                    "Public/Math"
                    "Private/Internationalization"
                    "Public/Internationalization"
                    "Private/Serialization"
                    "Private/Misc"
                    "../SynthBenchmark/Public"
)

# collect head files and source files
file(GLOB_RECURSE SOLIDANGLE_INCLUDES "Public/*.h"
                                      "Private/*.h"
                                      "Public/*.inl"
                                      "Public/*.hpp"
                                      "Private/*.inl"
                                      "Private/*.hpp"
                                      "Resources/*.h"
                                      "Resources/*.inl"
                                      "../InputDevice/*.h"
                                      "../Launch/Resources/*.h"
                                      "../SynthBenchmark/Public/*.h"
                                     )

file(GLOB_RECURSE SOLIDANGLE_SRCS 	"Private/*.cpp"
                            "../SynthBenchmark/Private/*.cpp"
                            )
# cpp compile add_compile
#set(DLLEXPORT "__declspec(dllexport)")
add_compile_definitions(
_UNICODE
UNICODE
UE_EDITOR=1
WITH_EDITOR=1
WITH_ENGINE=1
WITH_UNREAL_DEVELOPER_TOOLS=1
WITH_PLUGIN_SUPPORT=1
WITH_PERFCOUNTERS=1
HACK_HEADER_GENERATOR=0
UE_BUILD_MINIMAL=0
IS_MONOLITHIC=0
IS_PROGRAM=0
USE_LOGGING_IN_SHIPPING=0
USE_CHECKS_IN_SHIPPING=0
WITH_SERVER_CODE=1
WIN32=1
_WIN32_WINNT=0x0601
WINVER=0x0601
PLATFORM_WINDOWS=1
UE_ENABLE_ICU=0
WITH_DEV_AUTOMATION_TESTS=1
PLATFORM_WINDOWS
_CONSOLE
UE_IS_ENGINE_MODULE=1
#CORE_API=${DLLEXPORT}
CORE_API=""
MALLOC_LEAKDETECTION=1
)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>
$<$<CONFIG:Debug>:UE_BUILD_DEBUG=1> )


set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set (CMAKE_LINK_DEF_FILE_FLAG "")

add_library(SolidAngle SHARED ${SOLIDANGLE_INCLUDES} ${SOLIDANGLE_SRCS})

target_link_libraries(SolidAngle $<$<CONFIG:Debug>:${CMAKE_CURRENT_BINARY_DIR}/IntelTBB/Debug/tbbmalloc_debug.lib> $<$<CONFIG:Release>:${CMAKE_CURRENT_BINARY_DIR}/IntelTBB/Release/tbbmalloc_release.lib>)
target_link_libraries(SolidAngle Netapi32.lib
setupapi.lib
Dwmapi.lib
Dbghelp.lib
Imm32.lib
Iphlpapi.lib
Xinput.lib
Winmm.lib

)
target_link_libraries(SolidAngle $<$<CONFIG:Debug>:${CMAKE_CURRENT_BINARY_DIR}/../../../ThirdParty/zlib/zlib-1.2.5/Lib/Win64/zlibd_64.lib> $<$<CONFIG:Release>:${CMAKE_CURRENT_BINARY_DIR}/../../../ThirdParty/zlib/zlib-1.2.5/Lib/Win64/zlib_64.lib>)

set_target_properties(SolidAngle PROPERTIES OUTPUT_NAME "SolidAngle$<$<CONFIG:Debug>:_debug>$<$<CONFIG:Release>:_release>")
add_custom_command(
	TARGET SolidAngle POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:SolidAngle> "../../../Binaries/Win64/")
