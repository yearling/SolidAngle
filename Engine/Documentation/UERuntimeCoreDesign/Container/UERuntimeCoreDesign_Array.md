# TArray
A Dynamically sized array of typed elements.like std::vector

## 定义
	template<typename InElementType, typename InAllocator>
	class TArray  
InAllocator默认为FDefaultAllocator,就是FHeapAllocator

## 成员
	   typedef InElementType ElementType;
	   typedef InAllocator   Allocator;
	   typedef typename TChooseClass<
		Allocator::NeedsElementType,
		typename Allocator::template ForElementType<ElementType>,
		typename Allocator::ForAnyElementType
	>::Result ElementAllocatorType;

	ElementAllocatorType AllocatorInstance;
	int32	  ArrayNum;
	int32	  ArrayMax;
    
对于FDefaultAllocator来说其NeedsElementType为false,故一般为Allocator::ForAnyElementType,也就是Allcator返回的都是void*.

## 类大小为16字节 64bit平台下
ArrayNum,4字节，ArrayMax，4字节， FDefalutAllocator::ForAnyElemntType中保存了当前分配的指针，8字节，共16字节。

## 构造函数
1.	默认构造
2.	std::initializer_list<>构造
3.	由别的allocator的TArray来构造
4.	拷贝构造
5.	拷贝构造并且预分配多余的内存 `TArray(const TArray& Other, int32 ExtraSlack)`  
6.	由std::initializer的赋值构造
7.	由别的allocator的TArray来赋值构造
8.	由相同类型的Allocator的TArray来赋值构造
### 构造函数实现细节
1. `void CopyToEmpty(const OtherElementType* OtherData, int32 OtherNum, int32 PrevMax, int32 ExtraSlack)`  
    1. ResizeForCopy() 分配内存
    2. ConstructItmes<ElemntType>(GetData(),OtherData,OtherNum);
       ConstructItems通过`TIsBitwiseConstructible<DestinationElementType>`判断：
       1. 如果是POD类型，或者指针，或者基础数字类型的，直接Memcpy
       2. 否则，一个一个的调用place new来构造
 
## Move相关构造
1. 右值构造
2. 由既ElementType不同，Allocator也不同的TArray来右值构造
3. 由ElementType相同，Allocator不的的TArray来右值构造
3. 右值赋值构造
### 实现细节
调用`MoveOrCopy`函数
    1. 通过`TCanMoveTArrayPointersBetweenArrayTypes`来实现是否可以直接内存级的move语义  
        1. 两个Allocator类型一致
       	2. Allocator::MoveWillEmptyContainer为true
       	3. 元素类型一致，或者TIsBitwiseConstructible
    2. 否则调用CopyToEmpty()

## 析构
调用`DestructItems`来实现   
    1. `TIsTriviallyDestructible`为true，无操作
    2. `TIsTriviallyDestructible`为false,调用元素的析构

## 获取
`GetData`,获取首指针    
`GetTypeSize`, 获取元素sizeof    
`GetAllocatedSize`,获取分配了多少内存  
`GetSlack`,获取空闲内存  
`CheckInvariants`,检查(ArrayNum >= 0) & (ArrayMax >= ArrayNum  
`RangeCheck(int32 Index)`,检查Index是否非法。不知道在Build下是不是有损耗  
`IsValidIndex`:检查Index是否非法  
`Num`:容器元素数量  
`Max`:最大数量  
`operator[]`:
`Pop`:  
`Push`:  
`Top`: 返回最后一个元素
`Last`:
`CheckAddress`：
## 内存处理
`Shrink`  
`Reserve` 
## 查
`Find`  
`FindLast`  
`FindLastByPredicate` 
`IndexOfByKey` 
`IndexOfByPredicate`  
`FindByKey`  
`FindByPredicate`  
`FilterByPredicate`  
`Contains`  
`ContainsByPredicate`  
`operator==`  

## Add
`Add` 
`AddZero`
`AddDefault`
`Emplace`
`EmplaceAt`
`AddUnititialized`  
`InsertUninitialized`  
`InsertZeroed`
`InsertDefaulted`  
`Insert`
`Append`  
`AddUnique` 

## Delete
`RemoveAt` ：删除，保证原顺序
`RemoveAtSwap`:快速删除，不保证原顺序
`Reset(int32 NewSize)`:与Empty()一样，但**不释放内存**；除非NewSize > ArrayMax，会开辟新内存  
`Empty(int32 Slack)` : 清空Array并调用元素析构,**释放内存**
`RemoveSingle(const ElementType& Item)`:除去第一个与Item一致的元素  
`Remove`: 只要是与Item一致的都除去
`RemoveAll`:与Remove相比带Predicate  
`RemoveAllSwap`:与RemoveAll一致但不保证顺序  
`RemoveSingleSwap`: 
`RemoveSwap`: 与Remove一致，但不保证顺序
## SetNum
`SetNum`： 比当前数量多，会默认创建，比当前数量少，会调用RemoveAt()  
`SetNumZeroed`  
`SetNumUninitialized`  
`SetNumUnsafeInternal`: **只改变ArrayNum，不涉及内存变化及析构等** 
`Init`:创建多个  

## Swap
`SwapMemory` :
`Swap`:

## UObject
`FindItemByClass`:

## Iterator
`CreateIterator`:
`CreateConstIterator`:
UE中Iterator的使用方法如下：
`for(auto Iter = TArray.CreateIterator;Iter;++Iter)`  

## Sort
`Sort`
`StableSort`

## Heap 

## Traits
TIsContiguousContainer : true
TIsArray: true

## operator new 
用法与Emplace一致，原因有可能是当时没有c++11，一种取巧的方法  

		TArray<Foo> VecFoos;
		new （VecFoos)Foo(1,2);