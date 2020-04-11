#pragma once
#include "Core.h"
#include "SObject.h"
class SActor:public SObject
{
public:
	explicit SActor(int a);
	virtual ~SActor();
	static constexpr  bool IsInstance() { return true; };
	friend class SActorManager;
private:
	volatile int32 Count;
};
