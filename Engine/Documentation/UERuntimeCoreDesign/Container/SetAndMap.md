# TSet TMap

TMap是哈希Map，此容器是一个关联式容器，意味着每个键均拥有一个关联值，可通过键高效地查找值对象。
存在两种映射类型：TMap 和 TMultiMap。TMap 的键为唯一。键已存在时插入一个新的键值对将导致现有的对被替代。TMultiMap 的键不为唯一，因此新添加的对不会替代现有的对。

TSet 类似于 TMap 和 TMultiMap，但具有一个重要区别：TSet 不会以单独提供的键来关联元素，而是将元素本身作为键使用。TSets 添加、查找和移除元素非常快速（恒时）。默认情况下，TSets 不允许重复的键，但如果需要，也可以支持这一行为。

本文主要介绍
1. TSet,TMap的引擎需求
2. TSet,TMap的使用方法
3. TSet,TMap的实现原理及细节

##　TSet, TMap的引擎需求
1. 实现O(1)时间的查找，添加，删除；
2. 对元素没有偏序的要求，不像std::set,std::map那样要提供operator<
3. 可以迅速的遍历

## TSet的使用方法

## GetTypeHash
元素要有`inline uint32 GetTypeHash()`的hash函数
### 添加
	TSet<FString> FruitSet;
	FruitSet.Add(TEXT("Banana"));
	FruitSet.Emplace(TEXT("Mango"));

### 遍历
	for(auto & Elem :FruitSet)
	{ 
	  do(*Elem);
	}

### 查询
	int32 Count = FruitSet.Num();
	FSetElementId BananaIndex = FruitSet.Index(Text("Banana"));
	if(BananaIndex.IsValidId())
	{ 
	  do(FruitSet[BananaIndex]);
	}

	bool bHasBanana = FruitSet.Contains(TEXT("Banana"));
	FString* PtrBanana = FruitSet.Find(TEXT("Banana"));
	if(PtrBanana)
	{
	  do(*PtrBanana);
	}

### ToArray
	TArray<FString> FruitArray = FruitSet.Array();

### 删除 
	FruitSet.Remove(0); //索引
	int32 RemovedAmountPineapple = FruitSet.Remove(TEXT("Pineapple")); //返回的是删除的数量
	FruitSet.Empty(); //清空，清内存
	FruitSet.Reset(); //清空，不清内存

### 排序
	FruitSet.Sort([](const FString& A, const FString& B) {
	    return A > B; // sort by reverse-alphabetical order
	});

### Reserve Shrink
	FruitSet.Reserve(10);
	FruitSet.Shrink(); // 只删最后面的洞，中间的洞不删

## TMap的使用方法

###　添加
	TMap<int32, FString> FruitMap;
	FruitMap.Add(5, TEXT("Banana"));
	FruitMap.Add(2, TEXT("Grapefruit"));
	FruitMap.Add(7, TEXT("Pineapple"));
	FruitMap.Add(9);
	TMap<int32, FString> FruitMap2;
	FruitMap2.Emplace(3, TEXT("Orange"));
    FruitMap.Append(FruitMap2);

### 遍历
	for(auto& Elem :FruitMap)
	{
	   do(Elem.Key , *Elem.Value);
	}
	for(auto It = FruitMap.CreateConstIterator();It;++It)
	{
	    do(It.Key(),*It.Value());
	}

### 查询

	int32 Count = FruitMap.Num();
	FString Val7 = FruitMap[7];
	FString Val8 = FruitMap[8]; // assert!
	FString* Ptr7 = FruitMap.Find(7);
    bool bHas7 = FruitMap.Contains(7);
	FString& Ref7 = FruitMap.FindOrAdd(7);
	FString Val7 = FruitMap.FindRef(7);
    const int32* KeyMangoPtr   = FruitMap.FindKey(TEXT("Mango"));//反向查找，查value

###　ToArray
	TArray<int32>   FruitKeys;
	TArray<FString> FruitValues;
	FruitMap.GenerateKeyArray  (FruitKeys);
	FruitMap.GenerateValueArray(FruitValues);

### 删除
	FruitMap.Remove(8);
	FString Removed8 = FruitMap.FindAndRemoveChecked(8); // assert! 不存在的话会assert
	bool bFound2 = FruitMap.RemoveAndCopyValue(2, Removed);

### 排序
	FruitMap.KeySort([](int32 A, int32 B) {
	    return A > B; // sort keys in reverse
	});
	
	FruitMap.ValueSort([](const FString& A, const FString& B) {
	    return A.Len() < B.Len(); // sort strings by length
	});

### Reserve Shrink
	FruitMap.Empty();
	FruitMap.Reset();
	FruitMap.Reserve(10);
	FruitMap.Shrink(); //只删除最后面的洞
	FruitMap.Compact();//所有的洞都删



# MapSet实现细节
## 比较函数
### BaseKeyFuncs
	template<typename ElementType, typename InKeyType, bool bInAllowDuplicateKeys = false>
	struct BaseKeyFuncs 

## Set实现细节
### Set的声明

		template<
		typename InElementType,
		typename KeyFuncs = DefaultKeyFuncs<InElementType>,
		typename Allocator = FDefaultSetAllocator>
		class TSet;

		class FDefaultSetAllocator : public TSetAllocator<> { public: typedef TSetAllocator<>         Typedef; };

		template<
		typename InSparseArrayAllocator = TSparseArrayAllocator<>,
		typename InHashAllocator = TInlineAllocator<1, FDefaultAllocator>,
		uint32   AverageNumberOfElementsPerHashBucket = DEFAULT_NUMBER_OF_ELEMENTS_PER_HASH_BUCKET（2）,
		uint32   BaseNumberOfHashBuckets = DEFAULT_BASE_NUMBER_OF_HASH_BUCKETS（8）,
		uint32   MinNumberOfHashedElements = DEFAULT_MIN_NUMBER_OF_HASHED_ELEMENTS （4）
		>
		class TSetAllocator
		
可见，TSet保存元素使用TSparseArray, 平均hash每个bucket中有2个元素，默认有8个bucket，最小有4个元素；
### 主要成员变量
1. Elements

		ElementArrayType   Elements; 
		
		typedef TSparseArray<SetElementType, typename Allocator::SparseArrayAllocator>     ElementArrayType;
		
		typedef TSetElement<InElementType> SetElementType;
		
		template <typename InElementType>
		class TSetElement
		{
			typedef InElementType ElementType;
	
			/** The element's value. */
			ElementType Value;
	
			/** The id of the next element in the same hash bucket. */
			mutable FSetElementId HashNextId;
	
			/** The hash bucket that the element is currently linked to. */
			mutable int32 HashIndex;
		}	
也就是说TSparseArray中放的是{当前元素值，下一个bucket的ID（当前这个TSparseArray的索引），当前元素的hashID（在hashArray中的索引）}；

2. Hash
	
		mutable HashType   Hash;
		mutable int32      HashSize;
	
		typedef typename Allocator::HashAllocator::template ForElementType<FSetElementId> HashType;
HashType中存的是TSParseArray中的索引。

### 实现方式
简单来说，TSparseArray中存元素和自己的hash值及冲突哈希值；
HashArray中保存每个bucket的第一个元素的在TSparseArray中的索引。
__注意__：

		KeyFuncs::GetSetKey(): 通过元素的值找到key的值，对于set来说，key,value一致；对于Map来说，就不一样了
		
		KeyFuncs::GetKeyHash()：得到Hash值 
		
		GetTypedHash():从HashArray中读取值,值就是SparseArray的索引 
1. 添加
	1. 在Elements中分配一个SetElementType，把SetElementType.Value构造为元素的值；
	2. hash该元素，赋值为SetElementType.HashIndex;
	3. 以SetElementType.HashIndex值为地址在Hash中查找是否存在
	   1. 存在，hash冲突，链式hash,把SetElementType.HashIndex设为头，之前的设为SetElementType.HashNextID;
	   2. 不存在，设为头，SetElementType.HashNextID设为NONE_INDEX;
2. 查找
	1. 根据元素值获取hash值，再通过hash值获取在TSparseArray中的SetElementType,
	2. 对比SetElementType中的元素值与被查元素值是否一样，
	   1. 一样，查找到；
	   2. 不一样，取SetElementType.NextHashID来继续查找。
	