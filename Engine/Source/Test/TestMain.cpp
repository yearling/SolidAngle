#include "Core.h"
#include <iostream>
#include <string>
#include <vector>
//#include "Containers/SolidAngleString.h"
#include <vector>
#include <memory>
#include "Containers\StringConv.h"
#include "Modules\ModuleManager.h"
#include "HAL\MallocLeakDetection.h"
#include "Templates\AlignmentTemplates.h"
#include "Templates\AlignOf.h"
#include "Templates\Decay.h"
#include "Templates\AreTypesEqual.h"
#include "Containers\BitArray.h"

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
//	return FMemory::SystemMalloc(MallocByte);
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

void foo(TestContainerMoveCopy&& RRef)
{
	RRef.foo();
}

void foo(TestContainerMoveCopy RRef)
{
	RRef.foo();
}



void TestMemory()
{
	std::cout << "\n---------------PlatformMemoryTest----------" << std::endl;
	YPlatformMemory::Init();
	const YPlatformMemoryStats& StateReport = YPlatformMemory::GetStats();
	std::cout << "MemoryTotalPhysical: " << StateReport.TotalPhysicalGB << " GB"<<std::endl;
	std::cout << "MemoryTotalVirtual: " << (StateReport.TotalVirtual/1024/1024/1024)<<" GB" << std::endl;
	std::cout << "MemoryPage: " << StateReport.PageSize / 1024 << " KB" << std::endl;
	//while (1)
	std::cout << "Do memory testing..." << std::endl;
	{
		FMemory::TestMemory();
	}
	FMallocLeakDetection::Get().SetAllocationCollection(true);
	//int *pTestAllocMemory = (int*)FMemory::Malloc(sizeof(int));
	int* pIntLeak = new int(5);
	FMallocLeakDetection::Get().SetAllocationCollection(false);
	FMallocLeakDetection::Get().DumpPotentialLeakers();
	FMallocLeakDetection::Get().DumpOpenCallstacks();
}
void TestAlign()
{
	std::cout << "\n---------------AlignmentTest----------" << std::endl;
	std::cout << "Align 7 of 8: " << Align(7, 8) << std::endl;
	std::cout << "Align 17 of 16: " << Align(17, 16) << std::endl;
	std::cout << "AlignDown 7 of 8: " << AlignDown(7, 8) << std::endl;
	std::cout << "AlignDown 17 of 16: " << AlignDown(17, 16) << std::endl;
	std::cout << "IsAligned(14,8): " << std::boolalpha <<  IsAligned((void*)14, 8) << std::endl;
	std::cout << "IsAligned(16,8): " <<  IsAligned((void*)16, 8) << std::endl;
	std::cout << "AlignArbitary(13,7): " << AlignArbitrary(13,7) << std::endl;
	std::cout << "AlignArbitary(-5,7): " << AlignArbitrary(-5,7) << std::endl;
}

MS_ALIGN(256) struct PODAlign256
{
	int32 a;
	char b;
};
struct PODAlign256Add :PODAlign256
{
	uint8 a;
};
struct PODAlign256Compose
{
	uint8 MisalignmentPadding;
	PODAlign256 a;
};
void TestAlignOf()
{
	std::cout << "\n---------------AlignmentOfTest----------" << std::endl;
	std::cout << "Align of int32 :" << ALIGNOF(int32) << std::endl;
	std::cout << "Align of int64 :" << ALIGNOF(int64) << std::endl;
	std::cout << "Align of PODAlign256 :" << ALIGNOF(PODAlign256) << std::endl;
	std::cout << "sizeo of PODAlign256 :" << sizeof(PODAlign256) << "   sizeof(PODAlign356Add) :" << sizeof(PODAlign256Add) << std::endl;
	std::cout << "sizeo of PODAlign256 :" << sizeof(PODAlign256) << "   sizeof(PODAlign356Composize) :" << sizeof(PODAlign256Compose) << std::endl;
}

void TestDecay()
{
	std::cout << "\n---------------DecayTest----------" << std::endl;
	using INT10 = int32[10];
	typedef void(*pFunc)();
	using FuncType = void();
	static_assert(TAreTypesEqual<pFunc, FuncType*>::Value,"Types are not equal");
	static_assert(TAreTypesEqual<int*, TDecay<INT10>::Type>::Value,"Types are not equal");
	static_assert(TAreTypesEqual<pFunc, TDecay<void()>::Type>::Value,"Types are not equal");
}

class InspectorCopyMoveConstructor
{
public:
	InspectorCopyMoveConstructor()
	{
		std::cout << "[InspectorCopyMoveConstructor]:Default Constructor" << std::endl;
	}

	InspectorCopyMoveConstructor(const InspectorCopyMoveConstructor&)
	{
		std::cout << "[InspectorCopyMoveConstructor]:Default Copy Constructor" << std::endl;
	}

	InspectorCopyMoveConstructor& operator=(const InspectorCopyMoveConstructor&)
	{
		std::cout << "[InspectorCopyMoveConstructor]:Default Assignment" << std::endl;
	}

	InspectorCopyMoveConstructor( InspectorCopyMoveConstructor&&)
	{
		std::cout << "[InspectorCopyMoveConstructor]:Default Move Constructor" << std::endl;
	}
	InspectorCopyMoveConstructor& operator=(const InspectorCopyMoveConstructor&&)
	{
		std::cout << "[InspectorCopyMoveConstructor]:Default Move Assignment" << std::endl;
	}
	void operator()(int *p)
	{
		delete p;
	}
}; 

void TestUniquePtr()
{
	std::cout << "\n---------------UniqueTest----------" << std::endl;
	FMallocLeakDetection::Get().SetAllocationCollection(true);
	{
		TUniquePtr<int> TestIntUniquePtr = MakeUnique<int>(5);
		std::cout << "*(TUniquePtr<int>()): " << *TestIntUniquePtr << std::endl;

		TUniquePtr<int> TestIntUniqueMove = MoveTemp(TestIntUniquePtr);
		std::cout << "size of TUniquePtr<int>: " << sizeof(TestIntUniqueMove) << std::endl;
		check(TestIntUniquePtr.Get() == nullptr);
		check(*TestIntUniqueMove == 5);
		check(!TestIntUniquePtr.IsValid());
		check(!TestIntUniquePtr);
		int *pLeakInt = TestIntUniqueMove.Release();
		check(!TestIntUniqueMove);
		delete pLeakInt;
		TUniquePtr<int[]> TestIntArrayPtr = MakeUnique<int[]>(5);
		TUniquePtr<int, InspectorCopyMoveConstructor> UniquePtrWithDeleter = TUniquePtr<int, InspectorCopyMoveConstructor>(new int(5), InspectorCopyMoveConstructor());
		TUniquePtr<int, InspectorCopyMoveConstructor> UniquePtrWithDeleterCpy = MoveTemp(UniquePtrWithDeleter);
	}
	FMallocLeakDetection::Get().SetAllocationCollection(false);
	FMallocLeakDetection::Get().DumpPotentialLeakers();
	FMallocLeakDetection::Get().DumpOpenCallstacks();
}

void TestSharedPtr()
{
	std::cout << "\n---------------SharedPtrTest----------" << std::endl;
	SharedPointerTesting::TestSharedPointer<ESPMode::Fast>();
}
class YTestModel : public FDefaultModuleImpl
{
public:
	virtual bool SupportsDynamicReloading() override
	{
		// Core cannot be unloaded or reloaded
		return false;
	}
};

void TestBitArray()
{
	std::cout << "\n---------------TBitArrayTest----------" << std::endl;
	TBitArray<> TestBitArray;
	TBitArray<> TestAllTrue(true, 31);
	//TestAllTrue.Add(false);
	TestAllTrue.Add(true);
	TestAllTrue.Add(true);
	check(!TestAllTrue.Contains(false));
	check(TestAllTrue.Find(false) == INDEX_NONE);
	check(TestAllTrue.FindAndSetFirstZeroBit() == INDEX_NONE);
	FBitReference BooleanBit = TestAllTrue[0];
	BooleanBit = TestAllTrue[32];
	for (TBitArray<>::FIterator It(TestAllTrue); (bool)It; ++It)
	{
		//std::cout << "TBitArray[" << It.GetIndex() << "]'s Value is " << It.GetValue() << std::endl;
	}
	std::cout << "1<<31:" << (1 << 1) << std::endl;
}

void TestArray()
{
	std::cout << "\n---------------TArrayTest----------" << std::endl;
	TArray<int32> TestDefaultConsturct;
	TArray<int32> TestInitializer{ 1,2,3,4,5 };
	TArray<int32> TestCopy(TestInitializer);
	TArray<int32> TestMove(MoveTemp(TestInitializer));
	TArray<int32> TestInitilaizerOperator = { 1,2,3,4,5 };
	check(TestMove.GetData() == &TestMove[0]);
	check(TestMove.GetTypeSize() == sizeof(int32));
	TestMove.GetAllocatedSize();
	TestMove.GetSlack();
	TestMove.RangeCheck(2);
	check(TestMove.IsValidIndex(2));
	TestMove.Num();
	TestMove.Max();
	TestMove.Pop();
	TestMove.Push(0);
	TestMove.Top();
	TestMove.Last();
	TestMove.Last(1);
	TestMove.Shrink();
	int nIndex;
	if (TestMove.Find(5, nIndex))
	{
	}
	nIndex = TestMove.Find(5);
	check(nIndex == INDEX_NONE);
	nIndex = TestMove.FindLast(3);
	nIndex = TestMove.FindLastByPredicate([](int32 n) { return n == 4; });
	nIndex = TestMove.IndexOfByKey(5);
	int32 *pFindResultPtr = TestMove.FindByPredicate([](int32 n) { return n == 4; });
	TArray<int32> nFilterResult = TestMove.FilterByPredicate([](int32 n) {return n > 2; });
	//check(TestMove.Contains(1));
	//check(TestMove.ContainsByPredicate([](int32 n) { return n == 5; }));
	TestMove.AddUninitialized(5);
	TestMove.InsertDefaulted(0, 5);
	TestMove.InsertZeroed(0, 5);
	TestMove.InsertDefaulted(0, 5);
	TestMove.Insert({ 1,2,3,4,5 }, 0);
	TestMove.Insert(TestCopy,0);
	TestMove.CheckAddress(&TestCopy[1]);
	TestMove.RemoveAt(0);
	TestMove.RemoveAt(1);
	TestMove.RemoveAtSwap(3);
	TestMove.Reset(0);
	TestMove.Empty();
	TestMove.SetNum(200);
	TestMove.SetNumZeroed(200);
	TestMove.SetNumUninitialized(200);
	TestMove.Append(TestCopy);
	TestCopy.Emplace(0);
	TestCopy.EmplaceAt(3,0);
	TestCopy.Add(0);
	TestCopy.AddZeroed(5);
	TestCopy.AddDefaulted(5);
	TestCopy.AddUnique(5);
	TestCopy.Reserve(200);
	TestCopy.Init(4, 200);
	TestCopy.RemoveSingle(4);
	TestCopy.RemoveSingleSwap(4);
	TestCopy.Remove(4);
	TestCopy.RemoveSwap(4);
	TestCopy.RemoveAll([](int n) {return n == 4; });
	TestCopy.RemoveAllSwap([](int n) {return n == 4; });
	TestCopy.SwapMemory(0, 1);
	TestCopy.Empty();
	TestCopy = { 1,2 };
	TestCopy.Swap(0, 1);
	for (auto Iter = TestCopy.CreateIterator(); Iter; ++Iter)
	{

	}
	TestCopy.Sort();
}

void TestSparseArray()
{
	std::cout << "\n---------------TSparseArrayTest----------" << std::endl;
	TSparseArray<int> TestSparseArray;
	TestSparseArray.Add(1);
	TestSparseArray.Add(2);
	TestSparseArray.Add(3);
	TestSparseArray.Add(4);
	TestSparseArray.RemoveAt(0);
	std::cout << "before Add" << std::endl;
	for (auto Iter = TestSparseArray.CreateIterator(); Iter; ++Iter)
	{
		if (Iter)
		{
			//std::cout << *Iter << std::endl;
		}
	}
	std::cout << "After Add" << std::endl;
	TestSparseArray.Add(3);
	for (auto Iter = TestSparseArray.CreateIterator(); Iter; ++Iter)
	{
		if (Iter)
		{
			//std::cout << *Iter << std::endl;
		}
	}
	TestSparseArray.Insert(100, 5);
	std::cout << "Max elements num:" << TestSparseArray.GetMaxIndex() << std::endl;
	for (auto Iter = TestSparseArray.CreateIterator(); Iter; ++Iter)
	{
		if (Iter)
		{
			//std::cout << *Iter << std::endl;
		}
	}
	TestSparseArray.Compact();
	std::cout << "After compact ,Max elements num:" << TestSparseArray.GetMaxIndex() << std::endl;

}

void TestSet()
{
	std::cout << "\n---------------TSetTest----------" << std::endl;
	TSet<int> SetTest;
	TSet<int> SetInitializer = { 1,2,3,4,5,6 };
	TSet<int> SetInitializerRepeat = { 1,2,3,4,5,6,1,2,3 };
	check(SetInitializer.Find(2));
	check(SetInitializer.Contains(2));
	FSetElementId SetID = SetInitializer.FindId(2);
	int nFindFromID = SetInitializer[SetID];
	auto SetInterset = SetInitializer.Intersect(SetInitializerRepeat);
	auto SetUnion = SetInitializer.Union(SetInitializerRepeat);
	auto SetDifference = SetInitializer.Difference(SetInitializerRepeat);
	check(SetInitializerRepeat.Includes(SetInitializer));
	TArray<int> TestArrayFromSet = SetInitializer.Array();
	for (TSet<int>::TIterator Iter(SetInitializer); Iter; ++Iter)
	{
		std::cout << *Iter << std::endl;
	}
}
IMPLEMENT_MODULE(YTestModel, TestModel);

 /*void* operator new  (size_t Size)  { return FMemory::Malloc(Size); } 
 void* operator new[](size_t Size) { return FMemory::Malloc(Size); } 
 void* operator new  (size_t Size, const std::nothrow_t&) throw() { return FMemory::Malloc(Size); } 
 void* operator new[](size_t Size, const std::nothrow_t&) throw(){ return FMemory::Malloc(Size); } 
void operator delete  (void* Ptr)                                                  { FMemory::Free(Ptr); } 
void operator delete[](void* Ptr)                                                 { FMemory::Free(Ptr); } 
void operator delete  (void* Ptr, const std::nothrow_t&)                          throw() { FMemory::Free(Ptr); } 
void operator delete[](void* Ptr, const std::nothrow_t&)                          throw() { FMemory::Free(Ptr); }*/
struct TestStructDefaultConstruct
{
	int a;
	int b;
	int c;
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
	TestMemory();
	TestAlign();
	TestAlignOf();
	TestDecay();
	typedef void (*pFUnc)();
	//using FuncPointerType = decltype(pFUnc);
	pFUnc c=nullptr;
	static_assert(TIsFunction<void()>::Value, "Is true");

	TestUniquePtr();
	TestSharedPtr();
	TestArray();
	TestBitArray();
	TestSparseArray();
	TestSet();
//PODTypeWithStdString* pMemLeak = new PODTypeWithStdString();


	//TArray<TestContainerMoveCopy> vecMoveUniuqe;
	//new (vecMoveUniuqe)TestContainerMoveCopy();
	//new (vecMoveUniuqe)TestContainerMoveCopy();

	//TArray<int> ArrayInt = { 1,2,3,4,5,6,7,8 };
	//Algo::Reverse(ArrayInt);

	//TArrayView<int> ViewTest(ArrayInt);
	//ViewTest.Sort();

	//int buffer[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13 };
	//TArrayView<int>  ViewBuffer(buffer);
	//ViewBuffer.CheckInvariants();
	//YString YStringTest = TEXT("Hello中国");
	//YString FirstStringConsturct(YStringTest,2 );

	//TSparseArray<int>  SparseArray;
	//SparseArray.Add(6);
	//SparseArray.Add(7);
	//SparseArray.Insert(5, 10);
	//SparseArray.Add(8);
	//SparseArray.RemoveAt(5);
	//SparseArray.Insert(100, 10);
	//SparseArray.Add(11);
	//SparseArray.Add(12);
	//SparseArray.Add(13);
	//SparseArray.Add(14);
	//SparseArray.Reserve(1000);
	//SparseArray.Shrink();

	//for ( auto i = SparseArray.CreateConstIterator(); i; ++i)
	//{
	//	std::cout << *i <<  "    "<< i.GetIndex()<< std::endl;
	//}

	////foo(TestContainerMoveCopy());
	////std::vector<int> vecAdd;
	////vecAdd.push_back(1);
	//TSet<int> Set;
	//Set.Add(5);
	//Set.Add(6);
	//Set.Add(7);
	//Set.Add(7);
	//Set.Add(8);
	//Set.Add(9);
	//Set.Add(10);
	//Set.Add(11);
	//Set.Contains(9);
	//Set.Relax();
	//Set.Remove(9);
	return 0;
}

