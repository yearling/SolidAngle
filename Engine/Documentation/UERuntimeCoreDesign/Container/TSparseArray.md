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
1. TArray用来存元素

		typedef TArray<FElementOrFreeListLink, typename Allocator::ElementAllocator> DataType; 
 
		template<typename ElementType>
		union TSparseArrayElementOrFreeListLink
		{
			/** If the element is allocated, its value is stored here. */
			ElementType ElementData;
		
			struct
			{
				/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
				int32 PrevFreeIndex;
		
				/** If the element isn't allocated, this is a link to the next element in the array's free list. */
				int32 NextFreeIndex;
			};
		};

		/**
		* The element type stored is only indirectly related to the element type requested, to avoid instantiating TArray redundantly for
		* compatible types.
		*/
		typedef TSparseArrayElementOrFreeListLink<TAlignedBytes<sizeof(ElementType), ALIGNOF(ElementType)>> FElementOrFreeListLink;
2. TBitArray来存当前索引是否有元素
	
		typedef TBitArray<typename Allocator::BitArrayAllocator> AllocationBitArrayType;
	AllocationBitArrayType AllocationFlags;
3. 第一个空闲元素的索引`FirstFreeIndex`
4. 空闲元素的数量 `NumFreeIndex`
5. 空闲元素组成一个双向链表，`FirstFreeIndex`保存着第一个空间元素的索引。**为什么是双向链表**,方便在空闲链表中间某个位置取元素。
## 使用方法

### 构造函数
标准的6种构造

### 增加
1. AddUninitialized()，返回的是一个FSparseArrayAllocationInfo，包含{索引，指针}
	1. 如果空闲链表有元素，从空闲取一个；
	2. 如果空间链表没有元素，
	3. 在AllocationBitArrayType将索引对应的位置置true
2. Add()，返回的是索引。 调用AddUninitialized()来分配空间，调用place new在分配的空间上。
3. InsertUninitialized(int32 Index), Index可以超过SparseArray的大小。
	1. 分配DataType到相应的大小；
	2. 如果分配的空间大于当前存储的空间，中间的元素放到空闲链表中，
	3. 取出当前Index对应的元素，置BitArray的标记
4. Insert()

### 删除 
1. RemoveAtUninitialized();删除元素，不调用析构，放进空闲链表；
2. RemoveAt() 
3. Empty()，置空，不释放内存
4. Reset（），置空，释放内存

###　内存操作
1. Reserve()
2. Shrink()
3. Compact(),返回true: 有元素被relocated; 
4. CompactStable()，返回true: 有元素被relocated; 
5. GetAllocatedSize()

### Sort 
1. Sort()

### 查
1. IsAllocated()
2. GetMaxIndex()
3. Num()
4. CheckAddress()
