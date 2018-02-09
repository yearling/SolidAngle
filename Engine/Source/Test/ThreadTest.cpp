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
	std::mutex Mutex;
	std::condition_variable CV;
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
		TaskResults[i]->pConditional = &CV;
		TaskResults[i]->nIsSuccess = &nSuccess;
		pThreadPool->AddQueuedWork(TaskResults[i].Get());
	}

		//FScopeLock Lock(&CriticalSection);
		std::unique_lock<std::mutex> lock(Mutex);
		CV.wait(lock, [&nSuccess] {return nSuccess == 0; });

	std::cout << "use " << ThreadNum << " thread pool caculate " << JobNum << " prime numbers cost: " << FPlatformTime::Seconds() - elapseTime << std::endl;
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
	for (int ThreadNum = 1; ThreadNum <= 256; ThreadNum *= 2)
	{
		for (int JobNum = 1; JobNum <= 256; JobNum *= 2)
		{
			TestPool(ThreadNum, JobNum, N);
		}
	}

	
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
	std::unique_lock<std::mutex> lk(*pMutex);
	(*nIsSuccess)--;
	lk.unlock();
	pConditional->notify_one();
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
