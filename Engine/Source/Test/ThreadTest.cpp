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
void TestPool(int ThreadNum, int NumCacluePerThread)
{
	double elapseTime = FPlatformTime::Seconds();
	FQueuedThreadPool*pThreadPool = FQueuedThreadPool::Allocate();
	pThreadPool->Create(ThreadNum);
	FEvent* pEventDone = FPlatformProcess::GetSynchEventFromPool(false);
	TArray<TUniquePtr<CaculatePrim>> TaskResults;
	for (int i = 0; i < ThreadNum; ++i)
	{
		TaskResults.Emplace(MakeUnique<CaculatePrim>(i * NumCacluePerThread, (i + 1) * NumCacluePerThread));
		TaskResults[i]->EventDone = pEventDone;
		pThreadPool->AddQueuedWork(TaskResults[i].Get());
	}

	for (int i = 0; i < ThreadNum; ++i)
	{
		pEventDone->Wait();
	}

	std::cout << "use " << ThreadNum << " thread pool caculate " << NumCacluePerThread * ThreadNum << " prime numbers cost: " << FPlatformTime::Seconds() - elapseTime << std::endl;
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
void TestThread()
{
	FCommandLine::Set(TEXT(""));
	RunableJobTest job;
	FRunnableThread* pThread = FRunnableThread::Create(&job, TEXT("JOBTHREAD"));
	delete pThread;
	const int N = 5000;
	TestPool(1, N * 24);
	TestPool(2, N * 12);
	TestPool(3, N * 8);
	TestPool(4, N * 6);
	TestPool(5, N * 24/5);
	TestPool(6, N * 4);
	TestPool(8, N * 3);
	TestPool(12, N * 2);
	TestPool(24, N );
	TestPool(48, N/2 );
	TestPool(96, N/4 );


	TestRawThread(1, N * 24);
	TestRawThread(2, N * 12);
	TestRawThread(3, N * 8);
	TestRawThread(4, N * 6);
	TestRawThread(5, N * 24 / 5);
	TestRawThread(6, N * 4);
	TestRawThread(8, N * 3);
	TestRawThread(12, N * 2);
	TestRawThread(24, N);
	TestRawThread(48, N / 2);
	TestRawThread(96, N/4 );

	
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
	FCriticalSection SCLock;
	FScopeLock Lock(&SCLock);
	EventDone->Trigger();
}

void CaculatePrim::Abandon()
{

}

CaculatePrim::CaculatePrim(int32 Start, int End)
	:StartNum(Start)
	, EndNum(End)
{
	
	Result.Empty(End - Start);
}
