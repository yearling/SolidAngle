# UE Runtime Core结构设计 Allocator
## Allocator种类
1.	TAlignedHeapAllocator
2.	FHeapAllocator
3.	TInlineAllocator
4.	TFixedAllocator
5.	TSparseArrayAllocator
6.	TInlineSetAllocator

## 结构分析
主要代码见Conatiners/ContainerAllocationPolicies.h
### 默认的内存增加/收缩函数
1.	DefaultCalculateSlackShrink
2.	DefaultCalculateSlackGrow
3.	DefaultCalculateSlackReserve

### Traits
TAllocatorTraits： 里面只有SupportsMove 和 IsZeroConstruct两个标记

### Allocator基本接口
FContainerAllocatorInterface 
1.	MoveToEmpty： 对应Allocator的move语义
2.	GetAllocation
3.	ResizeAllocation
4.	CalculateSlack
5.	CalculateSlackShrink
6.	CalculateSlackGrow
7.	GetAllocatedSize

### FHeapAllocator
1. 实现了上述Allocator的基本接口，
2. TAllocatorTraits<FHeapAllocator>中定义SupportsMove= true, IsZeroConstruct= true  


FHeapAllocator为FDefualtAllocator,其成员变量只有FScriptContainerElement* Data，也就是void* Data，对应的大小在64bit的平台下为8字节。
FHeapAllocator只保存连续分配空间的地址，不保存有多少个元素等。保存多少个元素等信息存放在TArray等容器中，这个设计的原因有可能是为了减少冗余的数据。如果Allocator自己保存了有多少个元素，最大元素有多少，这样在container中也要存放这些信息，会造成数据冗余及一致性的问题。故FHeapAllocator只存指针。


### TInlineAllocator
1. 其模版声明为 
		template <uint32 NumInlineElements, typename SecondaryAllocator = FDefaultAllocator>  
		class TInlineAllocator 
   其NumInlineElements是指固定的内存分配大小，SecondaryAllocator当内存分配量大于固定内存时，动态分配内存；
2. __注意__:在Resize时，有可能要把在内存上分配的对象拷贝到堆上（或者反过来），这个时候牵涉到一个优化：
	1. 如果是可以直接位拷贝的`TCanBitwiseRelocate`,就有FMemory::Memmove;  
		TCanBitwiseRelocate<SourceType,DesType>::value为true:
	        TOr<
				TAreTypesEqual<DestinationElementType, SourceElementType>,
				TAnd<
				TIsBitwiseConstructible<DestinationElementType, SourceElementType>,
				TIsTriviallyDestructible<SourceElementType>  
		这其中有个`TIsBitwiseConstructible<T,U>`，用来判断U这个类型能不能通过memcpy产生一个新的T，
	2. 如果不可以直接位拷贝，要执行place new 

### TFixedAllocator
固定大小的Allocator