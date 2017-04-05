# TArrayView
Template  fixed-size view of another array.主要目的是用来兼容原始C语言风格的数组，或者是TArray作为参数(没有添加和删除，相当于const）传进函数。
__对原来C语言风格的数组提供TArray的接口__

## 构造函数
1. 元素为非当前类型（可转化为当前类型）的TArrayView
2. 元素为非当前类型（可转化为当前类型）的原始C类型数组
3. 元素为非当前类型（可转化为当前类型）的TArray
4. 元素为非当前类型（可转化为当前类型）的std::initializer_list
5. 元素为非当前类型（可转化为当前类型）的指针和元素数量

## MakeArrayView
支持上数类型，推荐使用MakeArrayView

## Get
`GetData`  
`GetTypeSize`  
`Num`  
`operator[]`  
`Last`  
`Slice`
## Check
`CheckInvariants`
`RangeCheck` 
`IsValidIndex`

## Find
`Find`
`FindLast`
`FindLastByPredicate`  
`IndexOfByKey`  
`FindByKey`  
`FindByPredicate`  
`FilterByPredicate`  
`Contain`  

##Sort 
__Sort__会改变原来数据
`Sort`
`StableSort`

