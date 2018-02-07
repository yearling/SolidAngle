# Thread

## Job
### class FRunable
1. Init() //在分配出来的新线程里做
2. Run()  //在分配出来的新线程里做
3. Stop() __//在主线程，也就是持有FRunableThread的线程中调用__
4. Exit() //在分配出来的新线程里做

## Thread
### class FRunableThread
		class FRunnableThreadWin: FRunableThread  //实现
1. static unit32 RunableTlsSlot // 所有的线程统一管理，该类的this指针存在RunableTlsSlot中。 
2. FRunable* Runable; // job
3. FEvent* ThreadInitSyncEvent; // Sync event to make sure that Init() has been completed before allowing the main thread to continue
4. TArray<FTlsAutoCleanup*> TlsInstances; // 用来保存ThreadSingleton的变量
5. uint32 ThreadID(); 
2. Create() 
		static FRunnableThread* Create(
		class FRunnable* InRunnable,
		const TCHAR* ThreadName,
		bool bAutoDeleteSelf,
		bool bAutoDeleteRunnable = false,
		uint32 InStackSize = 0,
		EThreadPriority InThreadPri = TPri_Normal,
		uint64 InThreadAffinityMask = 0);
			{
				 FRunnableThread* NewThread =  FPlatformProcess::CreateRunnableThread();
													|- new FRunnableThreadWin();
								|- NewThread->CreateInternal() 
									|- ThreadInitSyncEvent = FPlatformProcess::GetSynchEventFromPool(true);
									|- Windows API:CreateThread() 
										|- WinProc 
											|- NewThread->GuardedRun(); 
												|- try{ NewThread->Run() }
													|- Run()
														|- if(NewThread->Runable->Init())
															 { 
																ThreadInitSyncEvent->Trigger();
																SetTls(); //设置当前Thread的this指针， SlotIndex在进入Main函数前就已经获取
																Runnable->Run();
																Runnable->Exit();
																FreeTls(); //清除当前位于Tls中的this指针，以及清除ThreadSingleton变量
															}
															else
															{
																ThreadInitSyncEvent->Trigger();
															}
								
			}
									|- FThreadManager::Get().AddThread(ThreadID, this); // 添加到ThreadManger
									|- ThreadInitSyncEvent->Wait(INFINITE);
									|-ThreadName = InThreadName ? InThreadName : TEXT("Unnamed UE4");
									|-SetThreadName( ThreadID, TCHAR_TO_ANSI( *ThreadName ) );
									|-SetThreadPriority(InThreadPri);
## Utility
### FTlsAutoCleanUp 接口
#### 实现 
	1. 创建
		 `Register()`
		通过在当前线程的TLS中注册自己，所有权交由 FRunableThread::TlsInstances来管理
	2. 销毁
		当FRunableThread::FreeTls()时，被删除

### FThreadManager 
为了兼容单线程的东西

		/** List of thread objects to be ticked. */
		TMap<uint32, class FRunnableThread*> Threads;
		/** Critical section for ThreadList */
		FCriticalSection ThreadsCritical;
不管理线程的生命周期，只保存ID到FRunnableThread的指针
		