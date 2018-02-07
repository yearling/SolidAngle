#include "ThreadTest.h"
#include "HAL\RunnableThread.h"
#include <iostream>
#include "Misc\CommandLine.h"
void TestThread()
{
	FCommandLine::Set(TEXT(""));
	RunableJobTest job;
	FRunnableThread* pThread = FRunnableThread::Create(&job, TEXT("JOBTHREAD"));
	delete pThread;
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
