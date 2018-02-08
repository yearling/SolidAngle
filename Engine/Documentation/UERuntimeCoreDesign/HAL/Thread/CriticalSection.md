# CriticalSection 

分为2个
1. FWindowsCriticalSection
2. FPThreadsCriticalSection:用CriticalSection来实现
3. FWindowsSystemWideCriticalSection：用Mutex来实现

		typedef FWindowsCriticalSection FCriticalSection;
		typedef FWindowsSystemWideCriticalSection FSystemWideCriticalSection;

# FEvent
用来实现wait机制，
>This interface has platform-specific implementations that are used to wait for another thread to signal that it is ready for the waiting thread to do some work. It can also
be used for telling groups of threads to exit.

## 继承
Inheritance Hierarchy

	|- FEvent
		|- FPThreadEvent
		|- FSingleThreadEvent //Fake event object used when running with only one thread.
		|- FEventWin // windows 实现
		|- FSafeRecyclableEvent // EventPool的包裹
	|- FEventPool 
## 实现
1. 成员变量
	1. uint32 EventId; //An unique id of this even
	2. uint32 EventStartCycles; // Greater than 0, if the event called wait
2. FWinEvent
	1. HANDLE Event;
	2. bool ManualReset;
3. FSingleThreadEvent
	1.  pthread_mutex_t Mutex;
    2.  pthread_cond_t Condition;
可见，UE使用类似Windows的Event的接口，而不是Pthread的，这里使用pThread的接口来模拟WindowsEvent

## 使用工厂类来创建

### PoolEvent

1. 创建：FGenericPlatformProcess::GetSynchEventFromPool(bool bIsManualRese);
2. 回收：FPlatformProcess::ReturnSynchEventToPool(ThreadInitSyncEvent);

### 普通Event 被废弃
1. 创建： static class FEvent* CreateSynchEvent(bool bIsManualReset = false); 
### 实现

	FGenericPlatformProcess::GetSynchEventFromPool(bool bIsManualRese)
	{
		Event = new FEventWin();
		Event->Create(bIsManualReset);
	}

	FGenericPlatformProcess::GetSynchEventFromPool(bool bIsManualRese)
	{
		 FEvent* Result = Pool.Pop(); //从无锁队列中取出一个Event
		 if(!Result)
		 {
			Result = FGenericPlatformProcess::GetSynchEventFromPool(bool bIsManualRese);
		 }
		 return new FSafeRecyclableEvent(Result); // 包裹一下
    }
	

## FScopedEvent
析构时等待

