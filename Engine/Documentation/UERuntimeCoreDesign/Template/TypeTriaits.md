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


## TCallTraitsParamTypeHelper

	template <typename T, bool TypeIsSmall>
	struct TCallTraitsParamTypeHelper
	{
		typedef const T& ParamType;
		typedef const T& ConstParamType;
	};
	// 可直接复制的类型优化
	template <typename T>
	struct TCallTraitsParamTypeHelper<T, true>
	{
		typedef const T ParamType;
		typedef const T ConstParamType;
	};
    // 指针类型优化
	template <typename T>
	struct TCallTraitsParamTypeHelper<T*, true>
	{
		typedef T* ParamType;
		typedef const T* ConstParamType;
	}

主要由TCallTraitsBase来实现  

	template <typename T>
	struct TCallTraitsBase
	{
	private:
		enum { PassByValue = TOr<TAndValue<(sizeof(T) <= sizeof(void*)), TIsPODType<T>>, TIsArithmetic<T>, TIsPointer<T>>::Value };
	public:
		typedef T ValueType;
		typedef T& Reference;
		typedef const T& ConstReference;
		typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ParamType ParamType;
		typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ConstParamType ConstPointerType;
	};
之后有T&,T[N],const T[N]类型的特化

TTypeTraits的实现  

	template<typename T>
	struct TTypeTraitsBase
	{
		typedef typename TCallTraits<T>::ParamType ConstInitType;
		typedef typename TCallTraits<T>::ConstPointerType ConstPointerType;
	
		// There's no good way of detecting this so we'll just assume it to be true for certain known types and expect
		// users to customize it for their custom types.
		enum { IsBytewiseComparable = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T>>::Value };
	};

也就是说
1. 如果是复杂的值类型（非指针，非引用，非基本类型，比一个指针大）T，
TCallTraits<T>::ValueType       T
TCallTraits<T>::Referece        T&
TCallTraits<T>::ConstReference  const T &
TCallTraits<T>::ParamType       const T &
TCallTraits<T>::ConstPointerType   const T &

2. 如果是简单类型，非指针
TCallTraits<T>::ValueType       T
TCallTraits<T>::Referece        T&
TCallTraits<T>::ConstReference  const T &
TCallTraits<T>::ParamType       const T 
TCallTraits<T>::ConstPointerType   const T 

3. 指针 T*
TCallTraits<T>::ValueType       T
TCallTraits<T>::Referece        T&
TCallTraits<T>::ConstReference  const T &
TCallTraits<T>::ParamType       T* 
TCallTraits<T>::ConstPointerType   const T* 

4. 引用 T&
TCallTraits<T>::ValueType       T&
TCallTraits<T>::Referece        T&
TCallTraits<T>::ConstReference  const T &
TCallTraits<T>::ParamType       T& 
TCallTraits<T>::ConstPointerType   T& 

5. 数组T<N>
typedef T ArrayType[N];
TCallTraits<T>::ValueType       const T*
TCallTraits<T>::Referece        ArrayType&
TCallTraits<T>::ConstReference  const ArrayType&
TCallTraits<T>::ParamType       const T* const
TCallTraits<T>::ConstPointerType   const T* const

6. const 数组 const T<N>
typedef T ArrayType[N];
TCallTraits<T>::ValueType       T*
TCallTraits<T>::Referece        ArrayType&
TCallTraits<T>::ConstReference  const ArrayType&
TCallTraits<T>::ParamType       const T* const
TCallTraits<T>::ConstPointerType   const T* const

## TTypeTraitsBase

	struct TTypeTraitsBase
	{
		typedef typename TCallTraits<T>::ParamType ConstInitType;
		typedef typename TCallTraits<T>::ConstPointerType ConstPointerType;
	
		// There's no good way of detecting this so we'll just assume it to be true for certain known types and expect
		// users to customize it for their custom types.
		enum { IsBytewiseComparable = TOr<TIsEnum<T>, TIsArithmetic<T>, TIsPointer<T>>::Value };
	};
