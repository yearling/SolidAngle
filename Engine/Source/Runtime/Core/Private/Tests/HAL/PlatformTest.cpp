// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/AlignOf.h"
#include "Templates/UnrealTemplate.h"
#include "Containers/SolidAngleString.h"
#include "UObject/NameTypes.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

float TheCompilerDoesntKnowThisIsAlwaysZero = 0.0f;

struct TestA
{
	virtual ~TestA() {}
	virtual void TestAA() 
	{
		Space[0] = 1;
	}
	uint8 Space[64];
};


struct TestB
{
	virtual ~TestB() {}
	virtual void TestBB() 
	{
		Space[5] = 1;
	}
	uint8 Space[96];
};


struct TestC : public TestA, TestB
{
	int i;
};


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlatformVerificationTest, "System.Core.HAL.Platform Verification", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::SmokeFilter)

bool FPlatformVerificationTest::RunTest (const YString& Parameters)
{
	PTRINT Offset1 = VTABLE_OFFSET(TestC, TestB);
	PTRINT Offset2 = VTABLE_OFFSET(TestC, TestA);
	check(Offset1 == 64 + sizeof(void*));
	check(Offset2 == 0);
	int32 Test = 0x12345678;
#if PLATFORM_LITTLE_ENDIAN
	check(*(uint8*)&Test == 0x78);
#else
	check(*(uint8*)&Test == 0x12);
#endif
	check(YMath::IsNaN(sqrtf(-1.0f)));
	check(!YMath::IsFinite(sqrtf(-1.0f)));
	check(!YMath::IsFinite(-1.0f/TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!YMath::IsFinite(1.0f/TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!YMath::IsNaN(-1.0f/TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!YMath::IsNaN(1.0f/TheCompilerDoesntKnowThisIsAlwaysZero));
	check(!YMath::IsNaN(MAX_FLT));
	check(YMath::IsFinite(MAX_FLT));
	check(!YMath::IsNaN(0.0f));
	check(YMath::IsFinite(0.0f));
	check(!YMath::IsNaN(1.0f));
	check(YMath::IsFinite(1.0f));
	check(!YMath::IsNaN(-1.e37f));
	check(YMath::IsFinite(-1.e37f));
	check(YMath::FloorLog2(0) == 0);
	check(YMath::FloorLog2(1) == 0);
	check(YMath::FloorLog2(2) == 1);
	check(YMath::FloorLog2(12) == 3);
	check(YMath::FloorLog2(16) == 4);

	YGenericPlatformMath::AutoTest();

#if WITH_EDITORONLY_DATA
	check(FPlatformProperties::HasEditorOnlyData());
#else
	check(!FPlatformProperties::HasEditorOnlyData());
#endif

	check(FPlatformProperties::HasEditorOnlyData() != FPlatformProperties::RequiresCookedData());

#if PLATFORM_LITTLE_ENDIAN
	check(FPlatformProperties::IsLittleEndian());
#else
	check(!FPlatformProperties::IsLittleEndian());
#endif
	check(FPlatformProperties::PlatformName());

	check(YString(FPlatformProperties::PlatformName()).Len() > 0); 

	static_assert(ALIGNOF(int32) == 4, "Align of int32 is not 4."); //Hmmm, this would be very strange, ok maybe, but strange

	MS_ALIGN(16) struct FTestAlign
	{
		uint8 Test;
	} GCC_ALIGN(16);

	static_assert(ALIGNOF(FTestAlign) == 16, "Align of FTestAlign is not 16.");

	FName::AutoTest();

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
