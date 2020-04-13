#pragma once
#include "Core.h"
#include "SObject.h"
class SActor :public SObject
{
public:
	virtual ~SActor();
	static constexpr  bool IsInstance() { return true; };
	friend class SObjectManager;
private:
	explicit SActor(int a);
	volatile int32 Count;
};
