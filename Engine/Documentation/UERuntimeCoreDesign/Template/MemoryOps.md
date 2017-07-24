## TCanBitwiseRelocate 
TCanBitwiseRelocate<TDes,TSrc>::Value = (TypesEqual<TDes,TSrc> || (TBitwiseConstructible<TDes,TSrc> && TIsTrivallyDestructible<SourceElementType> ))  

## DefaultConstructItems
在内存中默认构造一些items 
通过SNIFT手法， TEnableIf<TIsZeroConstructType<ElementType>::Value>::Type来判断， 
1. 如果不是ZeroConstructType(Enum,Arithmetic,IsPointer)，就place new，调用构造 
2. 如果是，就对整个内存块MemZero  
__POD__类型为什么不属于ZeroConstructType?

## DestructItem
销毁一个Item 
TEnableIf<!TIsTriviallyDestructible<ElementType>::Value>::Type来判断  
1. 如果是TriviallyDestructible,调用析构； 
2. 如果不是TriviallyDestructible，不操作。  

## ConstructItems
用一段内存来去构建一些Items.  Constructs a range of items into memory from a set of arguments.  The arguments come from an another array  
TEnableIf<!TIsBitwiseConstructible<DestinationElementType, SourceElementType>::Value>::Type  
1. 如果是BitwiseConstructible，直接Memcpy; 
2. 如果不是BitwiseConstructible，一个一个的调用构造函数创建。  

## CopyAssignItems
Copy assigns a range of items  
通过TIsTriviallyCopyAssignable<>来判断；

## RelocateConstructItems
把一段内存里的内容移到新的内存中去，原来内存中的内容销毁  
通过TCanBitwiseRelocate来判断
1. TCanBitwiseRelocate<TDest,TSrc>::Value是false，刚先拷贝复制再调用析构；
2. TCanBitwiseRelocate<TDest,TSrc>::Value是true,直接Memmove

## MoveConstructItems
调用Move构造函数来创建Items，判断方法TIsTriviallyCopyConstructible，在StaticArray中被使用  

## MoveAssignItems
调用Move赋值语义，判断方法TIsTriviallyCopyConstructible  

## CompareItems
通过TEnableIf<TTypeTraits<ElementType>::IsBytewiseComparable, bool>::Type来判断  
1. 如果为true, 调用Memcmp; 
2. 如果不为true，调用每个Item进行比较。

