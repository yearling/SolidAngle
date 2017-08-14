# TSparseArray
稀疏Array
## 定义
	
	template<typename ElementType, typename Allocator = FDefaultSparseArrayAllocator >
	class TSparseArray;

	template<typename InElementAllocator = FDefaultAllocator, typename InBitArrayAllocator = FDefaultBitArrayAllocator>
	class TSparseArrayAllocator

	class FDefaultBitArrayAllocator : public TInlineAllocator<4> { public: typedef TInlineAllocator<4>     Typedef; };

可以看出，TSparseArray的Allocator是由THeapAlloctor与TInlineAlloctor组合而成

## 特点
1. 在SparseArray中元素不是连续的；
2. SparseArray给所有的元素都分配了内存，包括中间有空洞的元素；
3. o(1)时间内删除元素，并且剩余元素的索引没有失效（vs，Array,O(n)时间的删除，索引失效）

## 简单实现 
1. TArray用来存元素；
2. TBitArray来存当前索引是否有元素

## 使用方法