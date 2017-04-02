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

### TInlineAllocator