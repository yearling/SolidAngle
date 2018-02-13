#include "ThreadTest.h"
#include "HAL\RunnableThread.h"
#include <iostream>
#include "Misc\CommandLine.h"
#include "GenericPlatform\GenericPlatformMath.h"
#include "Misc\QueuedThreadPool.h"
#include "Templates\UniquePtr.h"
#include "HAL\Event.h"
#include "Windows\WindowsPlatformTime.h"
#include "GenericPlatform\GenericPlatformProcess.h"
#include "Windows\WindowsPlatformProcess.h"
#include "HAL\Runnable.h"
#include "Containers\Array.h"
#include "Misc\ScopeLock.h"
#include "Windows\WindowsCriticalSection.h"
#include "Async\TaskGraphInterfaces.h"

int IsPrim(int n)
{
	if (n == 2)
		return true;
	for (int i = 2; i <= FMath::RoundToInt(FMath::Sqrt(n)); ++i)
	{
		if (n%i == 0)
		{
			return false;
		}
	}
	return true;
}
void TestPool(int ThreadNum, int JobNum, int PrimNum)
{
	FQueuedThreadPool*pThreadPool = FQueuedThreadPool::Allocate();
	pThreadPool->Create(ThreadNum);
	FEvent* CV = FPlatformProcess::GetSynchEventFromPool(false);
	FCriticalSection Mutex;
	TArray<TUniquePtr<CaculatePrim>> TaskResults;
	FCriticalSection CriticalSection;
	
	double elapseTime = FPlatformTime::Seconds();
	const int NumJobs= JobNum;
	volatile int nSuccess = NumJobs;
	for (int i = 0; i < NumJobs; ++i)
	{
		const int32 PerJobCount = PrimNum / NumJobs;
		TaskResults.Emplace(MakeUnique<CaculatePrim>(i * PerJobCount, (i + 1) * PerJobCount, i));
		TaskResults[i]->pMutex = &Mutex;
		TaskResults[i]->pConditional = CV;
		TaskResults[i]->nIsSuccess = &nSuccess;
		pThreadPool->AddQueuedWork(TaskResults[i].Get());
	}

		//FScopeLock Lock(&CriticalSection);
	std::cout << "before wait" << std::endl;
	while (1)
	{
		CV->Wait();
		FScopeLock lock(&Mutex);
		if (nSuccess == 0)
			break;
	}

	std::cout << "use " << ThreadNum << " thread pool caculate " << JobNum << " prime numbers cost: " << FPlatformTime::Seconds() - elapseTime << std::endl;
	delete pThreadPool;
}
class FPrimRunable final : public FRunnable
{
public:
	FPrimRunable(int32 Start, int32 End)
		:StartNum(Start),
		EndNum(End)
	{

	}
	virtual uint32 Run() override
	{
		for (int32 i = StartNum; i <= EndNum; ++i)
		{
			if (IsPrim(i))
			{
				Result.Add(i);
			}
		}
		return 1;
	}
	TArray<int32>		Result;
	int32				StartNum;
	int32				EndNum;
};
void TestRawThread(int ThreadNum, int NumCacluePerThread)
{

	double elapseTime = FPlatformTime::Seconds();

	TArray<TUniquePtr<FPrimRunable>> Result;
	TArray<FRunnableThread*> pThread;
	for (int i = 0; i < ThreadNum; ++i)
	{
		Result.Emplace(MakeUnique<FPrimRunable>(i * NumCacluePerThread, (i + 1) * NumCacluePerThread));
		pThread.Add( FRunnableThread::Create(Result[i].Get(), TEXT("THR0")));
	}
	for (int i = 0; i < ThreadNum; ++i)
	{
		pThread[i]->WaitForCompletion();
	}
	std::cout << "use " << ThreadNum << " RAW thread caculate " << NumCacluePerThread * ThreadNum << " prime numbers cost: " << FPlatformTime::Seconds() - elapseTime << std::endl;
}

bool RunableJobTest::Init()
{
	//FRunnableThread* pCurrentThread = FRunnableThread::GetRunnableThread();
	//check(pCurrentThread);
	//std::cout << "Init thread is is" << pCurrentThread->GetThreadID() << std::endl;
	return true;
}

uint32 RunableJobTest::Run()
{
	int i = 0;
	while (i<1000000)
	{
		++i;
	}
	return 0;
}

void RunableJobTest::Stop()
{
	std::cout << "call stop" << std::endl;
}

void CaculatePrim::DoThreadedWork()
{
	for (int32 i = StartNum; i <= EndNum; ++i)
	{
		if (IsPrim(i))
		{
			Result.Add(i);
		}
	}
	{
		FScopeLock lock(pMutex);
		(*nIsSuccess)--;
	}
	pConditional->Trigger();
}

void CaculatePrim::Abandon()
{

}

CaculatePrim::CaculatePrim(int32 Start, int End,int32 ID)
	:StartNum(Start)
	, EndNum(End)
	,  id(ID)
{
	pMutex = nullptr;
	pConditional = nullptr;
	nIsSuccess = nullptr;
}

CaculatePrim::~CaculatePrim()
{
	if (id)
	{
		pMutex = nullptr;
	}
}



TArray<uint32> PrimeNumbers;
// This is the array of thread completions to know if all threads are done yet
FGraphEventArray		VictoryMultithreadTest_CompletionEvents;
//~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~
//Are All Tasks Complete?
//~~~~~~~~~~~~~~~
bool TasksAreComplete()
{
	//Check all thread completion events
	for (int32 Index = 0; Index < VictoryMultithreadTest_CompletionEvents.Num(); Index++)
	{
		//If  ! IsComplete()
		if (!VictoryMultithreadTest_CompletionEvents[Index]->IsComplete())
		{
			return false;
		}
	}
	return true;
}
//~~~~~~~~~~~
//Actual Task Code
//~~~~~~~~~~~
int32 FindNextPrimeNumber()
{
	//Last known prime number  + 1
	int32 TestPrime = PrimeNumbers.Last();

	bool NumIsPrime = false;
	while (!NumIsPrime)
	{
		NumIsPrime = true;

		//Try Next Number
		TestPrime++;

		//Modulus from 2 to current number - 1 
		for (int32 b = 2; b < TestPrime; b++)
		{
			if (TestPrime % b == 0)
			{
				NumIsPrime = false;
				break;
				//~~~
			}
		}
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//Did another thread find this number already?
	if (PrimeNumbers.Contains(TestPrime))
	{
		return FindNextPrimeNumber(); //recursion
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Success!
	return TestPrime;
}


//~~~~~~~~~~~
//Each Task Thread
//~~~~~~~~~~~
class FVictoryTestTask
{

public:
	FVictoryTestTask() //send in property defaults here
	{
		//can add properties here
	}

	/** return the name of the task **/
	static const TCHAR* GetTaskName()
	{
		return TEXT("FVictoryTestTask");
	}
	FORCEINLINE static TStatId GetStatId()
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FVictoryTestTask, STATGROUP_TaskGraphTasks);
	}
	/** return the thread for this task **/
	static ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::AnyThread;
	}


	/*
	namespace ESubsequentsMode
	{
	enum Type
	{
	//Necessary when another task will depend on this task.
	TrackSubsequents,
	//Can be used to save task graph overhead when firing off a task that will not be a dependency of other tasks.
	FireAndForget
	};
	}
	*/
	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return ESubsequentsMode::TrackSubsequents;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~
	//Main Function: Do Task!!
	//~~~~~~~~~~~~~~~~~~~~~~~~
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		PrimeNumbers.Add(FindNextPrimeNumber());

		//***************************************
		//Show Incremental Results in Main Game Thread!

		//	Please note you should not create, destroy, or modify UObjects here.
		//	  Do those sort of things after all thread are completed.

		//	  All calcs for making stuff can be done in the threads
		//	     But the actual making/modifying of the UObjects should be done in main game thread,
		//		 which is AFTER all tasks have completed :)

		//ThePC->ClientMessage(FString("A thread completed! ~ ") + FString::FromInt(PrimeNumbers.Last()));
		//***************************************
	}
};

//~~~~~~~~~~~~~~~~~~~
//  Multi-Task Initiation Point 
//~~~~~~~~~~~~~~~~~~~
void FindPrimes(const uint32 TotalToFind)
{
	PrimeNumbers.Empty();
	PrimeNumbers.Add(2);
	PrimeNumbers.Add(3);

	//~~~~~~~~~~~~~~~~~~~~
	//Add thread / task for each of total prime numbers to find
	//~~~~~~~~~~~~~~~~~~~~

	for (uint32 b = 0; b < TotalToFind; b++)
	{
		VictoryMultithreadTest_CompletionEvents.Add(TGraphTask<FVictoryTestTask>::CreateTask(NULL, ENamedThreads::GameThread).ConstructAndDispatchWhenReady()); //add properties inside ConstructAndDispatchWhenReady()
	}
}
void TestThread()
{
	FCommandLine::Set(TEXT(""));
	/*RunableJobTest job;
	FRunnableThread* pThread = FRunnableThread::Create(&job, TEXT("JOBTHREAD"));
	delete pThread;
	const int N = 5000;
	for (int ThreadNum = 1; ThreadNum <= 256; ThreadNum *= 2)
	{
		for (int JobNum = 1; JobNum <= 256; JobNum *= 2)
		{
			TestPool(ThreadNum, JobNum, N);
		}
	}*/
	FTaskGraphInterface::Startup(FPlatformMisc::NumberOfCores());
	FTaskGraphInterface::Get().AttachToThread(ENamedThreads::GameThread);
	FindPrimes(5000);
	while (!TasksAreComplete())
	{
		FPlatformProcess::Sleep(1);
	}
}
