# Template
UE为了防止引用std namespace,将C++11里引进的[type_traits](http://en.cppreference.com/w/cpp/header/type_traits)简单实现了一遍。

## TEnableIf  
对应c++11 的std::enable_if通过使用 SFINAE,  根据第一个模板参数来确定返回值Type的类型，如果为false，就没有实现，模板实例化时会失败。   
用来通过对类型的判断来执行是值传还是引传等。

		template< bool Predicate, typename ResultType = void>
		class TEnableIf;
		
		template< typename Result>
		class TEnableIf<true, Result>
		{
		public:
			typedef Result Type;
		};
		
		template<typename Result>
		class TEnableIf<false,Result>
		{}

## TAnd TAndValue
对应c++17的 std::conjunction  
TAnd实现对类型的::Value来执行And操作，有false短路操作。
TAndValue实现对<bool,Type::Value>的And操作。

## TOr TOrValue
对应c++17的std::disjunction  
TOr实现对类型的::Value来执行And操作，有true短路操作。
TOrValue实现对<bool,Type::Value>的Or操作。

## TNot
对应c++17的std::negation  
TNot实现对类型的Value的否操作

## TAreTypesEqual
对应c++11的std::is_same  
来判断两类型是否一致

## TRemoveCV
对应c++11的std::remove_cv  

## TIsArithmetic
对应c++11的std::arithmetic  
如果是数值类型（包括CV)，value为true，如果是数值类型的指针，为false
 
## TIsFloatingPoint
对应c++11的std::floating_point  
对于flaot,double,long double（包括CV）为true

## TIsIntegral
对应c++11的std::is_integral  
对于int,shot,bool,char(包括CV）为true

## TIsPointer
对应c++11的std::is_pointer

## TIsPOD
对应c++11的std::is_pod
[POD](http://en.cppreference.com/w/cpp/concept/PODType)  
POD,plain old data,标量，array，或者是不带static,private，virtual的struct,class,union  
可以用malloc分配空间，使用std::memmove，与C类型完全兼容

## TIsSigned 
对应c++11的std::is_signed

## TIsTriviallyCopyAssignable
对应c++11的is_trivially_copy_assignable
**注意** ：判断是否能在=左边

## IsTriviallyCopyConstructible
对应c++11的std::is_trivially_copy_assignable
**注意** ：包含scalar types，trivial copyable class(指的是，没有虚函数，没有拷贝构造函数）

## TPointerIsConvertibleFromTo
测试用来看pointer能不能从from转为to

## TTypeWrapper
没来防止模版递归的

## Align
头文件使用AlignmentTemplates.h，有函数Align,AlignDown,IsAligned,AlignArbitrary

## TRemoveReference
对应c++11的std::remove_reference

## TChooseClass
对应c++11的std::conditional
根据条件来判断返回哪个类 Predicate?TrueClass:falseClass

## TIntegralConstant
对应c++11的std::integral_constant
帮助类

## TIsClass
对应c++11的std::is_class
**注意** enum class不是class
	
## TElementAlignmentCalculator
对应c++11的std::alignment_of
用来计算对齐值。

## TTypeCompatibleBytes
对应c++11的std::aligned_storage
用来计算对象对应的byte数

## TIsContiguousContainer
用来判断是否是连续内存

## TIsTriviallyDestructible
对应c++11的std::is_trivially_destructible

## TIsEnum
对应c++11的std::is_enum

## TIsEnumClass
判断是不是enum class

## TLess
对应c++14的std::less

## TGreater
对应c++14的std::greater

## TIsDerivedFrom
对应c++11的std::is_base_of

## TIsReferenceType
对应c++11的std::reference

## TIsLValueReferenceType
对应c++11的std::is_lvalue_reference

## TIsRValueReferenceType
对应c++11的std::is_rvalue_reference

## TIsVoidType
对应c++11的std::is_void

## TIsFundamentalType
对应c++11的std::is_fundamental

## TIsFunction
对应c++11的std::is_function

## TIsZeroConstructType

## Call
1.	TCallTraitsParamTypeHelper
2.	TCallTraitsBase
3.	TCallTraits

## Type
1.	TTypeTraitsBase
2.	TTypeTraits
3.	TContainerTraitsBase
4.	TMoveSupportTraits
5.	GENERATE_MEMBER_FUNCTION_CHECK

## TDecay
对应std::decay，Applies lvalue-to-rvalue, array-to-pointer, and function-to-pointer implicit conversions to the type T, removes cv-qualifiers,降级的转换

