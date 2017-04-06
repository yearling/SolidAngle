#include "Core.h"
#include <iostream>
#include <string>
#include <vector>
//#include "Containers/SolidAngleString.h"
#include <vector>
#include <memory>
#include "Containers\StringConv.h"
struct TRUEValue
{
	enum 
	{
		Value = true
	};
};

struct FALSEValue
{
	enum 
	{
		Value = false
	};
};

template< int64 N>
struct Fib
{
	enum { Value = Fib<N-2>::Value +Fib<N-1>::Value};
};

template<>
struct Fib<0>
{
	enum { Value = 0 };
};

template<>
struct Fib<1>
{
	enum { Value = 1 };
};

template<int M, int N>
struct Mul
{
enum { Value = Mul<M, N - 1>::Value + M };
};

template<int M>
struct Mul<M, 1>
{
	enum {
		Value = M
	};
};

struct TrueValue
{
	enum { Value = true };
};
struct FalseValue
{
	enum { Value = true };
};
class BaseTest
{
public:
	BaseTest() {};
	virtual ~BaseTest() {};
	virtual bool NonTrivial() { return true; };
};

struct PODType
{
	int a;
	int b;
	char c[0x7fffffff];
};
struct PODAlign8
{
	int64 a;
};

MS_ALIGN(128) struct PODAlign16
{
	int64 a;
	__m128 b;
};
struct PODTypeWithStdString
{
	int a;
	std::string name;
};

struct PODTypeWithStdVector
{
	int a;
	std::vector<int> name;
};

class DirivedTest :public BaseTest
{
public:
	DirivedTest() {};
	virtual ~DirivedTest() {}
	virtual bool NonTrivial() override { return true; }
};

enum TestEnum { eFirst,eSecond };

enum class TestClassEnum : uint32
{
	eFirst,eSecond
};

template<typename T>
typename TEnableIf<TIsArithmetic<T>::Value, T>::Type GetEnableValue(T a)
{
	return a;
}

template<typename T>
typename TEnableIf<!TIsArithmetic<T>::Value, int>::Type GetEnableValue(T a)
{
	return -1;
}

//void* operator new(SIZE_T MallocByte)
//{
//	return YMemory::SystemMalloc(MallocByte);
//}
class TestContainerMoveCopy
{
public:
	TestContainerMoveCopy()
	{
		std::cout << "call default construct!!" << std::endl;
		pData = new ANSICHAR(50);
		Vec0 = { 1,2,4,5 };
	}
	virtual ~TestContainerMoveCopy()
	{
		delete pData;
	}
	TestContainerMoveCopy(const TestContainerMoveCopy& RHS)
	{
		std::cout << "call copy construct!!" << std::endl;
	}
	TestContainerMoveCopy& operator= (const TestContainerMoveCopy& RHS)
	{
		if (this != &RHS)
		{
			std::cout << "call operator construct!!" << std::endl;
		}
	}
	TestContainerMoveCopy(TestContainerMoveCopy&& RHS)
	{
		std::cout << "call right value construct!!" << std::endl;
		pData = RHS.pData;
		delete RHS.pData;
		RHS.pData = nullptr;
	}
	TestContainerMoveCopy& operator= (TestContainerMoveCopy&& RHS)
	{
		std::cout << "call right value opeartor constructor!!" << std::endl;
		pData = RHS.pData;
		delete RHS.pData;
		RHS.pData = nullptr;
	}
	void foo()
	{
		std::cout << pData << std::endl;
	}
private:
	ANSICHAR* pData=nullptr;
	std::vector<int>  Vec0;
};

int main()
{

#if 0
	std::cout << "---------------TEnableIf-----------" << std::endl;
	std::cout << GetEnableValue(12.0f) << std::endl;
	DirivedTest EnableDrived;
	std::cout<< GetEnableValue(EnableDrived)<<std::endl;
	std::cout << "---------------TAnd-----------" << std::endl;
	static_assert(TAnd<>::Value, "should be true");
	static_assert(TAnd<TAnd<TRUEValue, TRUEValue>, TRUEValue>::Value, "should be true");
	//static_assert(!YAnd<FALSEValue>::Value,"should be true");
	std::cout << Fib<46>::Value << std::endl;
	std::cout << Mul<5, 4>::Value << std::endl;
	static_assert(TAnd<TRUEValue, FalseValue>::Value, "should be true");
	static_assert(std::is_pod<BaseTest*>::value, "shoudl be true");
	static_assert(!std::is_pod<BaseTest>::value, "shoudl be true");
	static_assert(!std::is_pod<DirivedTest>::value, "shoudl be true");
	static_assert(std::is_pod<PODType>::value, "shoudl be true");
	static_assert(!std::is_pod<PODTypeWithStdString>::value, "shoudl be true");
	static_assert(!std::is_pod<PODTypeWithStdVector>::value, "shoudl be true");
	static_assert(!TIsPODType<PODTypeWithStdString>::Value, "shoudl be true");
	static_assert(TIsTriviallyCopyAssignable<PODType>::Value, "shoudl be true");
	static_assert(!TIsTriviallyCopyAssignable<PODTypeWithStdVector>::Value, "shoudl be true");
	static_assert(TPointerIsConvertibleFromTo<DirivedTest, BaseTest>::Value, "shoudl be true");
	//static_assert(!TIsPODType<PODTypeWithStdVector>::Value, "shoudl be true");
	//static_assert(TIsTriviallyCopyAssignable<PODTypeWithStdString>::Value, "shoudl be true");
	//static_assert(!TIsPODType<PODTypeWithStdVector>::Value, "shoudl be true");
	//static_assert(TAnd<>::Value, "should be true");
	std::cout << "---------------Align-----------" << std::endl;
	std::cout << YMath::RoundUpToPowerOfTwo(31) << std::endl;
	std::cout << YMath::FloorLog2(31) << std::endl;
	std::cout << YMath::CeilLogTwo(31) << std::endl;
	std::cout << Align(1023, 16) << std::endl;
	std::cout << AlignDown(1023, 16) << std::endl;

	std::cout << "\n---------------Align Of----------" << std::endl;
	std::cout << TElementAlignmentCalculator<TrueValue>::Value << std::endl;
	std::cout << TElementAlignmentCalculator<PODType>::Value << std::endl;
	std::cout << TElementAlignmentCalculator<PODAlign8>::Value << std::endl;
	std::cout << TElementAlignmentCalculator<PODAlign16>::Value << std::endl;

	PODAlign16 StackAlign;
	if (IsAligned(&StackAlign, TElementAlignmentCalculator<PODAlign16>::Value))
	{
		std::cout << "Stack Align" << std::endl;
	}
	int32 j = 0;
	for (int32 i = 0; i < 10000; ++i)
	{
		PODAlign16* pHeapAlign = new PODAlign16;
		if (!IsAligned(pHeapAlign, TElementAlignmentCalculator<PODAlign16>::Value))
		{
			//std::cout << "Heap is not Align" << std::endl;
			j++;
		}
	}
	std::cout << "Heap is not Align happens " <<j<<" Times in 10000 loop"<< std::endl;

	std::cout << "\n---------------TTypeCompatibleBytes----------" << std::endl;
	TTypeCompatibleBytes<PODAlign8> TypeCompatiblePOD8;
	TypeCompatiblePOD8.Padding.Pad[0];
	std::cout << "\n---------------TIsTriviallyDestructible----------" << std::endl;
	static_assert(TIsTriviallyDestructible<PODType>::Value, "shoudl be true");
	static_assert(!TIsTriviallyDestructible<DirivedTest>::Value, "shoudl be true");

	std::cout << "\n---------------TIsEnum----------" << std::endl;
	static_assert(TIsEnum<TestEnum>::Value, "should be true");
	static_assert(TIsEnumClass<TestClassEnum>::Value, "should be true");

	std::cout << "\n---------------TChar----------" << std::endl;
	TCHAR test_Chinese[] = TEXT("ello world\n");
	std::cout << test_Chinese << std::endl;
	std::wcout << test_Chinese << std::endl;

#endif 
	std::cout << "\n---------------PlatformMemoryTest----------" << std::endl;
	YPlatformMemory::Init();
	auto StateReport = YPlatformMemory::GetStats();
	//YPlatformMemory::OnOutOfMemory(3000, 16);
	//std::cout << StateReport.TotalPhysicalGB << std::endl;
	//while (1)
	{
		YMemory::TestMemory();
	}

	//PODTypeWithStdString* pMemLeak = new PODTypeWithStdString();


	TArray<TestContainerMoveCopy> vecMoveUniuqe;
	new (vecMoveUniuqe)TestContainerMoveCopy();
	new (vecMoveUniuqe)TestContainerMoveCopy();

	TArray<int> ArrayInt = { 1,2,3,4,5,6,7,8 };
	Algo::Reverse(ArrayInt);

	TArrayView<int> ViewTest(ArrayInt);
	ViewTest.Sort();

	int buffer[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13 };
	TArrayView<int>  ViewBuffer(buffer);
	ViewBuffer.CheckInvariants();
	YString YStringTest = TEXT("Hello中国");
	YString FirstStringConsturct(YStringTest,2 );
	FChar wc = TEXT('𪚥');
	return 0;
}
