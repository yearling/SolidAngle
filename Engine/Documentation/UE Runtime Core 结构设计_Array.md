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
