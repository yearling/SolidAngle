# HAL
HAL(Hardware Abstruct Interface),用来统一操作系统的抽象层

## 分为4大块
1. 内存
2. IO
	1. 文件
3. 线程
	1. 线程创建管理
	2. 同步原语
	3. TLS
4. Misc

## 内存
相关的文件有 14个

	|- MallocAnsi.h
	|- MallocBinned.h
	|- MallocBinned2.h 
	|- MallocDebug.h 
	|- MallocJemalloc.h 
	|- MallocLeakDetection.h 
	|- MallocPoisonProxy.h
	|- MallocStomp.h 
	|- MallocTBB.h 
	|- MallocThreadSafeProxy.h 
	|- MallocVerify.h 
	|- MemoryBase.h 
	|- MemoryMisc.h 
	|- PlatformMallocCrash.h
	|- PlatformMemory.h
	|- UnrealMemory.h

## IO
### 文件相相
相关文件有 9 个  

	|- FileManager.h  
	|- FileManagerGeneric.h 
	|- IOBase.h 
	|- IPlatformFileCachedWrapper.h 
	|- IPlatformFileLogWrapper.h 
	|- IPlatformFileModule.h 
	|- IPlatformFileOpenLogWrapper.h 
	|- IPlatformFileProfilerWrapper.h 
	|- PlatformFile.h
	|- PlatformFilemanager.h
### IO相关

	|- IOBase.h 
    |- OutputDevice.h  
	|- PlatformOutputDevices.h

## 线程
### 线程管理

	|- PlatformAffinity.h 
	|- PlatformAtomics.h 
	|- PlatformProcess.h
	|- Runnable.h
	|- RunnableThread.h
	|- ThreadHeartBeat.h
	|- ThreadingBase.h 
	|- ThreadManager.h 

### 同步原语

	|- CriticalSection.h 
	|- Event.h
	|- PThreadCriticalSection.h
	|- PThreadEvent.h
	|- ThreadSafeBool.h  
	|- ThreadSafeCounter.h 
	|- ThreadSafeCounter64.h
	|- ThreadSingleton.h
### TLS
	
	|- PlatformTLS.h
	|- TlsAutoCleanup.h


## Misc
	
	|- PlatformMisc.h

### 字符串

	|- PlatformString.h

### 数学库

	|- PlatformMath.h

### 时间

	|- PlatformTime.h

### 控制台

	|- ConsoleManager.h 
	|- IConsoleManager.h

### 异常处理

	|- ExceptionHanding.h

### 反馈

	|- FeedbackContextAnsi.h 

### 代码分析 

	|- PlatformCodeAnalysis.h

### 不知道干什么的

	|- PlatformFramePacer.h
	|- PlatformProperties.h
	|- PlatformStackWalk.h

## Platform.h 

通过宏来控制使用哪些平台依赖的实现，比如windows使用PLATFORM_WINDOWS宏。