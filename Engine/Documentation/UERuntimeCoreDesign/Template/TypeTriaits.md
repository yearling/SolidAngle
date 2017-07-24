## TAreTypesEqual
用来判断两个类型是否一致。用宏来包裹 `#define ARE_TYPES_EQUAL(A,B) TAreTypesEqual<A,B>::Value`  

## TIsDerivedFrom<typename DerivedType, typename BaseType>
用来判断是不是继承关系  

## TIsCharType
用来判断是不是Char类型

## TFormatSpecifier<>::GetFormatSpecifier
用来获取数值类型的输出符，比如int是 "%d"

## TIsReferenceType 
判断是不是引用类型

## TIsLValueReferenceType
判断是不是左值引用 

## TIsRValueReferenceType
判断是不是右值引用  

## TIsVoidType
判断是不是void类型  

## TIsFundamentalType 
判断是不是基本类型（数值+void)  

## TIsFunction
判断是不是函数类型 

## TIsZeroConstructType
判断是不是没有来构造函数 

## TIsWeakPointerType
判断是不是weakptr 

## TNameOf<>::GetName()
获取类型名  



## TRemoveConst
去掉const 修饰符号


## TCallTraitsParamTypeHelper
传参形参优化
1. 如果是小类型： typdef const T ParamType;
2. 如果是小类型指针，typdef T* ParamType;
3. 其它： typedef const T& ParamType


## TCallTraitsBase
根据输入参数的类型来判断是引用还是值传
PassByValue = （sizeof(T)<=sizeof(void*) && ISPOD<T>) || 基本数学类型 || IsPointer

## TCallTraits:public TCallTraitsBase
1. & 特化 
2. T[N] 特化
3. const T[N]特化

## TTypeTraitsBase  
给Array使用，
enum IsBytewiseComparable= (IsEnum || IsArithmetic || IsPointer) 

## TTypeTraits
继承于TTypeTraitsBase

## TContainerTraitsBase
enum MoveWillEmptyContainer 

## TMoveSupportTraits 

	template <typename T>
	void Func(typename TMoveSupportTraits<T>::Copy Obj)
	{
	    // Copy Obj here
	}
	template <typename T>
	void Func(typename TMoveSupportTraits<T>::Move Obj)
	{
	   // Move from Obj here as if it was passed as T&&
	} 

## TIsBitwiseConstructible
是否可以通过MemCpy来创建一个实例（不是通过constructor)  





