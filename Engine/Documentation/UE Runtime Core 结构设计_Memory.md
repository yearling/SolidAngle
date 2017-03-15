#UE Runtime Core 结构设计
##Memory


###头文件组织关系
1.	PlatformMemory为主要头文件，其中包含GenericPlatformMemory和WindowsPlatformMemory(WIN平台）。
2.	GenericPlatformMemory实现基础功能
3.	WindowPlatformMemory实现平台特化，最终通过typedef为UPlatformMemory.