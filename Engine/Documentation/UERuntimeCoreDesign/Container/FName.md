# FName
*	FName要解决的问题
*   FName有两种初始化方式
    *   内置类型
	*   自定义类型

## FName要解决的问题
1.	唯一名子
2.	名子一般做为ID用，最常用的是比较
3.	内存使用优化
4.	比较优化
### FName有两种初始化方式
## 内置类型



## 自定义类型  
		FName::FName(const WIDECHAR* Name, EFindName FindType)
		FName::FName(const ANSICHAR* Name, EFindName FindType)  

1.	FName构造函数一般传入的是TCHAR,这样子会使用WIDECHAR* 的重载版本，为了节约内存使用，会检查是不是该WIDECHAR* 都可以用ASCII表示 
    1.	 `YName::Init(const WIDECHAR* InName, int32 InNumber, EFindName FindType, bool bSplitName, int32 HardcodeIndex)`,__注意__: HardcodeIndex(默认为-1):如果<0，说明是自定义类型，如果>=0,说明是内置类型; 
    2.	 如果可以用ASCII表示，转化为ASCII使用ASCII的特化版本`InitInternal_HashSplit<ANSICHAR>(StringCast<ANSICHAR>(InName).Get(), InNumber, FindType, bSplitName, HardcodeIndex);`; 
	     1.    根据传入的名子和bSplitName来决定是否启用名子末位是数字需要切名子；
	     2.    获取uint16 NonCasePerservingHash用来对比与查询；
	     3.    获取uint16 CasePerservingHase用来编辑器使用；
	     4.    调用`YName::InitInternal(const TCharType* InName, int32 InNumber, const EFindName FindType, const int32 HardcodeIndex, const uint16 NonCasePreservingHash, const uint16 CasePreservingHash)`; 
		     	1.   判断FName整个类是否初始化，
		     		 1.    使用GetIsInitialized()来获取bInitialized的引用。
		     		       __注意__: 这里不使用`bool FName::bIsInitialized`,而是使用 
 
						  static bool& YName::GetIsInitialized()
									{
									static bool bIsInitialized = false;
									return bIsInitialized;
									} 	

						   
					 2.    其原因如下，为了防止静态类成员在Main函数执行前的初始化顺序问题，在第一次使用时保证其初始化。比较像是Singleton的手法。
					 3.    执行`FName::StaticInit()`来初始化FName类
					       1.   `FCrc::Init()`，其实在计算NonCasePerservingHash时已经使用过FCrc，这时FCrc的初始化只是检测下FCrc的Table是不是正确。发布版不检测，这是个空函数；
					       2.   设置GetIsInitialized() = true;  
					       3.   FName::NameHashHead[65536]为空; FName::NameHashTail[65536]为空
					       4.   之后进入CriticalSection
					            1.     