# TSparseArray
稀疏Array
## 定义
	
	template<typename ElementType, typename Allocator = FDefaultSparseArrayAllocator >
	class TSparseArray;

	template<typename InElementAllocator = FDefaultAllocator, typename InBitArrayAllocator = FDefaultBitArrayAllocator>
	class TSparseArrayAllocator

	class FDefaultBitArrayAllocator : public TInlineAllocator<4> { public: typedef TInlineAllocator<4>     Typedef; };

可以看出，TSparseArray的Allocator是由THeapAlloctor与TInlineAlloctor组合而成