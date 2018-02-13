#pragma once
#include "CoreTypes.h"
#include "HAL\Runnable.h"
#include "Misc\IQueuedWork.h"
#include "Containers\Array.h"
#include "Hal\CriticalSection.h"
#include <mutex>
#include <condition_variable>

class RunableJobTest: public FRunnable
{
	virtual bool Init();
	virtual uint32 Run() override;
	virtual void Stop() override;
};

int32 IsPrim(int32 i);
void TestThread();
class CaculatePrim : public IQueuedWork
{
public:
	TArray<int32>		Result;
	int32				StartNum;
	int32				EndNum;
	virtual void DoThreadedWork();
	virtual void Abandon();
	CaculatePrim(int32 Start, int End,int ID);
	virtual ~CaculatePrim();

	FCriticalSection*         pMutex;
	FEvent* pConditional;
	volatile int32*               nIsSuccess;
	int32				id;
};