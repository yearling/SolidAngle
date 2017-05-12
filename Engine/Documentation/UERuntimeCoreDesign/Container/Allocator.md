# UE Runtime Core结构设计 Allocator
## Allocator要解决什么问题
对于容器来说，主要是负责基本数据结构的操作，对于C++来说对内存使用的要求是比较严格的：
1. 内存分配的位置，栈还是堆？
2. 内存分配是否连续，再分配是否之后的引用与指针会失效
3. 内存增长的策略
4. 内存释放
5. 对齐问题
这些功能放到比较TArray,Set,Map中去维护是比较困难的，能否抽象出来一种通用的内存管理模块，就是Allocator的作用。

## 为什么要作为模板参数传进容器内
Allocator基本上是使用设计模式中的组合的思想，实现容器的内存管理，对外暴露统一的接口；既然是组合的思想，完全可以这样子写  

### 写法１　

	template<typename T>
	class TArray
	{
	    FDefaultAllocator<T>  Allocator; 
	}; 

	template<typename T>
	class TArrayWithInlineAllocator
	{
	    FInlineAllocator<T>  Allocator; 
	}; 
问题：没法在编译器的指定Allocator，对于一种容器只能使用一种Allocator

### 写法2

	template<typename T>
	class IAllocator{};
	
	template<typename T>
	class FDefaultAllocator:IAllocator<T>{};
	
	template<typename T>
	class TArray
	{
		IAllocator::FDefaultAllocator* pAllocator;
	public:
		TArray( IAllocator* pAllocator);
	or : 
	    static MakeTarrayWithDefulatAllocator();
	}; 

问题： FDefaultAllocator的生命管理是个问题，并且与容器分离开了，造成逻辑和管理上的混乱。
并且，如果需要Allocator的组和就更复杂了，比如

	template<typename ElementAllocator= FDefaultAllocator,typename HashAllocator = FInlineAllocator>
	class THashArray
	{
	} 
如果用这种写法的话就问题很多。  


### 写法3 
template<typename T, typename TAllocator= FDefaultAllocator>
class TArray
{
  TAllocator Allocator;
}

和标准库与UE都采用这种实现，好处  
1. 在编译期实现Allocator的定制；
2. Allocator可以相互组合；
3. 没有运行期的开销；


##  UE中 Allocator种类
1.	TAlignedHeapAllocator
2.	FHeapAllocator
3.	TInlineAllocator
4.	TFixedAllocator
5.	TSparseArrayAllocator
6.	TInlineSetAllocator

## Allocator基本接口
FContainerAllocatorInterface 
1.	MoveToEmpty： 对应Allocator的move语义
2.	GetAllocation
3.	ResizeAllocation
4.	CalculateSlack
5.	CalculateSlackShrink
6.	CalculateSlackGrow
7.	GetAllocatedSize

## 结构分析
主要代码见Conatiners/ContainerAllocationPolicies.h
### 默认的内存增加/收缩函数
1.	DefaultCalculateSlackShrink
2.	DefaultCalculateSlackGrow
3.	DefaultCalculateSlackReserve

### Traits
TAllocatorTraits： 里面只有SupportsMove 和 IsZeroConstruct两个标记



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