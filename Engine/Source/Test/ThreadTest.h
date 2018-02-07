#pragma once
#include "CoreTypes.h"
#include "HAL\Runnable.h"


class RunableJobTest: public FRunnable
{
	virtual bool Init();
	virtual uint32 Run() override;
	virtual void Stop() override;
};


void TestThread();