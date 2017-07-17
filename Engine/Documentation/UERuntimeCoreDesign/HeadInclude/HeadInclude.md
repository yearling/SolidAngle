#UE Include 
## Core

	core.h
		|- CoreMinimal.h
				|- CoreTypes.h
						|- Misc/Build.h //定义了编译选项：DEBUG,SHIPPING,TEST,DEVELOPMENT,ASSERT,VERIFY
						|- HAL/Platform.h
								|- Windows/WindowsPlatformCompilerPreSetup.h // 忽略一些warning
								|- GenericPlatform/GenericPlatformCompilerPreSetup.h // 定义deprecated 宏  
								|- GenericPlatform/GenericPlatform.h // 定义int32等基本类型  
								|- Windows/WindowsPlatform.h //定义FORCEINLINE，DLLEXPORT等平台相关的宏
					 			|- 通过GenericPlatform与windowsPlatform最终定义int32等类 
						|-Misc/CoreMiscDefines.h
								|-统计相关的宏	
		|- CoreFwd.h
				|- Containers/ContainersFwd.h
				|- 基本类型的前置声明
		|- Memory库，数学库，容器类的头文件
		|- 所有的头文件