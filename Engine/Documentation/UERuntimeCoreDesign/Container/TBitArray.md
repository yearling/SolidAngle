# TBitArray
特化的用来存储bit的动态Array。最小的c++内存分配粒度为一个uint8,也就是8个bit，如果用uint8来表示一个bit的话会极大的浪费。这里使用特化的Bit操作，一个uint8可以表示8个bit.

## 实现

	template<typename Allocator = FDefaultBitArrayAllocator>
	class TBitArray;

	class FDefaultBitArrayAllocator : public TInlineAllocator<4> { public: typedef TInlineAllocator<4>     Typedef; };

因为TArray的每个元素为uint32，故默认支持128位的bit操作（小于128位没有new操作）。 见代码`typedef typename Allocator::template ForElementType<uint32> AllocatorType;`