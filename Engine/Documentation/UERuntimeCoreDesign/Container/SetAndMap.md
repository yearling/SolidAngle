# TSet TMap

TMap是哈希Map，此容器是一个关联式容器，意味着每个键均拥有一个关联值，可通过键高效地查找值对象。
存在两种映射类型：TMap 和 TMultiMap。TMap 的键为唯一。键已存在时插入一个新的键值对将导致现有的对被替代。TMultiMap 的键不为唯一，因此新添加的对不会替代现有的对。

TSet 类似于 TMap 和 TMultiMap，但具有一个重要区别：TSet 不会以单独提供的键来关联元素，而是通过评估元素的可覆盖函数将元素本身作为键使用。TSets 添加、查找和移除元素非常快速（恒时）。默认情况下，TSets 不允许重复的键，但如果需要，也可以支持这一行为。

本文主要介绍
1. TSet,TMap的引擎需求
2. TSet,TMap的使用方法
3. TSet,TMap的实现原理及细节

##　TSet, TMap的引擎需求
1. 实现O(1)时间的查找，添加，删除；
2. 对元素没有偏序的要求，不像std::set,std::map那样要提供operator<
3. 可以迅速的遍历

## TSet的使用方法

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