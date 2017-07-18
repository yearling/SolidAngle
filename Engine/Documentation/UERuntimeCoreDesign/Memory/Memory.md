# UE Runtime Core 结构设计

## 类图如下：
![Add](Picture/MemoryClassDiagram.png)
## PlatformMemory
1.	PlatformMemory.h为主要头文件，其中包含GenericPlatformMemory和WindowsPlatformMemory(WIN平台）。
2.	GenericPlatformMemory实现基础Memory的相关功能：SyetemMalloc,Memcopy等
3.	WindowPlatformMemory实现平台特化，最终通过typedef为UPlatformMemory.  
## 头文件包含关系 

	|- PlatformMemroy.h
			|- CoreTypes.h  见\Documentation\UERuntimeCoreDesign\HeadInclude  
			|- GenericPlatform/GenericPlatformMemory.h 
					|- 定义struct :FGenericPlatformMemoryConstants  
					|- 定义struct：FGenericPlatformMemoryStats
					|- 定义并曝露FGenericPlatformMemory类，用来管理以上两个结构体，以及MemoryCpy等操作 
			|- Windows/WindowsPlatformMemory.h  
					|- struct FWindowsPlatformMemory->FGenericPlatformMemory
					|- typedef FWindowsPlatformMemory FPlatformMemory;最终曝露在外面的是FPlatformMemory
### 相关细节
1.	FGenericPlatformMemoryConstants  
    1.TotalPhysical  
	2.TotalVirtual  
	3.PageSize  
 	4.etc.
2.  FGenericPlatformMemoryStats : 继承于FGenericPlatformMemoryConstants  
    1.	AvailablePhysical  
    2.	AvailableVirtual  
    3.	UsedPhysical  
    4.	etc.
3.  FPlatformMemoryStats : FGenericPlatformMemoryStats  
	添加平台相关的数据
4.  FGenericMemoryStats
    里面有个map,保存各种内存数据，对外接口
>也就是说MemoryConstants是用来统计固件内存的，MemoryStats又添加了当前实时的内存状态

## FGnericPlatformMemory
1.	FGenericPlatformMemory   
    该类只有static函数，
	1.	统计内存使用，返回上述内存统计信息： GetStats();
	2.	内存分配初始化策略 `init()`
		1.	`SetupMemoryPools`: 设置内存池
		2.	`OnOutOfMemory`:　　预分配32mb的内存防止在OO(out of memory)crash的时候，没有空间打log;
	3.	SharedMemory
	4.	内存操作　　
		1.	`BinnedAllocFromOS` :使用VirtualAlloc系统调用
		2.	`BinnedFreeToOS`
		3.	`GPUMalloc` ：UMA架构有效
		4.	`Memmove/cmp/cpy/zero/set` `BigBlockMemoryCpy`
		5.	`StreamingMemcpy`
	5.	Dump
2.	FWindowsPlatformMemory :继承于FGenericPlatformMemory
	在Windows平台下typedef为FPlatformMemory


## MemoryBase
### FMalloc  （HAL/MemoryBase.h）
   1.	提供内存分配的基类，实现有MallocAnsic,MallocBinned,MallocBinned2,MallocStomp,MallocTBB等。
   2.	全局内存分配器GMalloc
   3.	FMalloc 主要有
  		1.	Malloc, Realloc,Free,QuantizeSize, Trim    
  		2.	SetupTLSCachesOnCurrentThread  	  
  		3.	ClearTLSCachesOnCurrentThread  
  		4.	mallc/realloc/free的记数器   
  		
**GAlloc**是一种分配器（全局），在程序初始化时设定，可以在develop下通过命令行更改，可配置，在进入Main函数之前就被调用，在第一次调用时设置
## FMemory （HAL/UnrealMemory.h)
   1.   使用代理的方式，将GMalloc根据配置设为指定的Malloc,`YMemory::GCreateMalloc()`,然后在FMemory::Malloc时调用GMalloc->Malloc来实现；  
   2.   使用代理的方式，对外有MemCmp/copy/move等函数：在FMemory::MemCopy时调用FPlatformMemory::Memcopy来实现；
   3.   SystemMallc/Free等函数；


## 使用方法
1.	一般对外接口只使用YMemory的函数就行  
	1.	**Malloc** **Free** **Realloc**
	2.	**注意**：在Model编译的时候有`REPLACEMENT_OPERATOR_NEW_AND_DELETE`宏，将operator new 替换成了FMemory::Malloc,也就是说，在每个次new的时候，分配空间调用的是FMemory::Malloc，然后再调用构造函数。
	
				#define REPLACEMENT_OPERATOR_NEW_AND_DELETE \
				OPERATOR_NEW_MSVC_PRAGMA void* operator new  ( size_t Size                        ) OPERATOR_NEW_THROW_SPEC      { return YMemory::Malloc( Size ); } \
				OPERATOR_NEW_MSVC_PRAGMA void* operator new[]( size_t Size                        ) OPERATOR_NEW_THROW_SPEC      { return YMemory::Malloc( Size ); } \
				OPERATOR_NEW_MSVC_PRAGMA void* operator new  ( size_t Size, const std::nothrow_t& ) OPERATOR_NEW_NOTHROW_SPEC    { return YMemory::Malloc( Size ); } \
				OPERATOR_NEW_MSVC_PRAGMA void* operator new[]( size_t Size, const std::nothrow_t& ) OPERATOR_NEW_NOTHROW_SPEC    { return YMemory::Malloc( Size ); } \
				void operator delete  ( void* Ptr )                                                 OPERATOR_DELETE_THROW_SPEC   { YMemory::Free( Ptr ); } \
				void operator delete[]( void* Ptr )                                                 OPERATOR_DELETE_THROW_SPEC   { YMemory::Free( Ptr ); } \
				void operator delete  ( void* Ptr, const std::nothrow_t& )                          OPERATOR_DELETE_NOTHROW_SPEC { YMemory::Free( Ptr ); } \
				void operator delete[]( void* Ptr, const std::nothrow_t& )                          OPERATOR_DELETE_NOTHROW_SPEC { YMemory::Free( Ptr ); }

	2.	Memmvoe/cpy/cmp/set/zero/swap  
	3.	SystemMalloc/Free
2.	FPlatformMemory
	1.	获取统计信息:`GetStatsForMallocProfiler`
	2.	NamedShareMemory:`MapNamedSharedMemoryRegion`
## 设计原理
### FGenericPlatformMemory
1.	FGenericPlatformMemory定义基本操作，相当于接口，带默认实现
2.	FWindowPlatformMemory实现平台特定的操作，相当于实现
3.	**注意**：实现用的只是简单的继承，没有使用虚函数使用重载来减少开销。最后typedef FWindowsPlatformMemory FPlatformMemory，对外只使用FPlatformMemory。对于ISO平台来说，使用typdef FIosPlatformMemory FPlatformMemory来实现,对调用者来说是透明的。**目的**是编译期的跨平台。

### YMalloc
1.	YMalloc继承于YUseSystemMallocForNew,其定义了类的operator new

		class CORE_API YUseSystemMallocForNew
		{
		public:
			/**
			* Overloaded new operator using the system allocator.
			*/
			void* operator new(size_t Size);
			void operator delete(void* Ptr);
			void* operator new[](size_t Size);
			void operator delete[](void* Ptr);
		};
	这样子做的原因是，需要根据参数动态创建YMalloc，开始时全局GAlloc为null, 在第一次创建FMalloc时，如`GAlloc = new FTBBMalloc`，这时会调YMemory::Malloc，因为GMalloc还没有初始化（这时候还没有到Main函数，肯定是单线程），继续调`GAlloc = new FTBBMalloc`，导致无限递归，所以要自定义operator new来打破这种递归。  
![Add](Picture/Recursive.png)

## Malloc(0) / Realloc(0)
### [Malloc(0)](https://msdn.microsoft.com/en-us/library/6ewkz86d.aspx)  
>If size is 0, malloc allocates a __zero-length__ item in the heap and returns a valid pointer to that item.   
### [Realloc(0)](https://msdn.microsoft.com/en-us/library/xbebcx7d.aspx)  
>If size is zero, then the block pointed to by memblock is __freed__; the return value is NULL, and memblock is left pointing at a freed block.
# TODO
- [ ] 统计信息
- [ ] 不同的YMemory类的功能
- [ ] 为什么默认用TBBMalloc