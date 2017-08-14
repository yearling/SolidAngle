# TBitArray
特化的用来存储bit的动态Array。最小的c++内存分配粒度为一个uint8,也就是8个bit，如果用uint8来表示一个bit的话会极大的浪费。这里使用特化的Bit操作，一个uint8可以表示8个bit.

## 实现

	template<typename Allocator = FDefaultBitArrayAllocator>
	class TBitArray;

	class FDefaultBitArrayAllocator : public TInlineAllocator<4> { public: typedef TInlineAllocator<4>     Typedef; };

因为TArray的每个元素为uint32，故默认支持128位的bit操作（小于128位没有new操作）。 见代码`typedef typename Allocator::template ForElementType<uint32> AllocatorType;`

## 主要操作
1. 增加
	1. Add()
2. 遍历
   通过TBitArray<>::FIterator来实现，`for (TBitArray<>::FIterator It(TestAllTrue); (bool)It; ++It)`  
3. 删除
	1. Empty(),清空清内存
	2. Reset(),清空不清内存
	3. RemoveAt(),移后面的数据
	4. RemoveAtSwap(),快速移动，不保证顺序
4. 修改
    1. Init()
    2. SetRange(),注意要在有效范围内设
5. 查
	1. Find()，查第一个true/false所在的位置，不存在返回INDEX_NONE(-1)
	2. Contains()
	3. FindAndSetFirstZeroBit(),返回位置，不存在返回0
	4. IsValidIndex()
	5. Num()
	6. operator[],由FBitReference来实现。首先将一个索引拆成[index/32,index%32],再进行位操作来实现。
	7. AccessCorrespondingBit,与Iterator配合使用
6. 内存大小
	1. GetAllocatedSize()
	2. CountBytes()