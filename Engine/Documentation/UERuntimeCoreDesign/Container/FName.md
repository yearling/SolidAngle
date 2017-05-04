# FName
*	FName要解决的问题
*   FName使用场景
*   FName的实现细节

## FName要解决的问题
在类中经常会使用一些内容不变的字符串，这时候可以使用FName来替代。
1.	使用字符串作为名子存在的**内存**方面的问题
	1.	每个名子作为字符串都需要内存分配一次（分配内存是时间很贵重的操作）  
	2.	重复名子造成的同名字符串的浪费  
	3.	UE默认是宽字节字符，但很多名子都是ANSIC的，造成浪费  
2.	使用字符串作为名子存在的**性能**方面的问题
	1.	名子一般做为ID用，最常用的是比较，普通字符串的比较是o(n)时间的
	2.	获取字符串时需要二次索引，对cache不友好  
## FName使用场景 
在类里，用来标识资源名子，类名子，等一些不变字符串时，可以使用FName。
__注意:__,也就是说凡是见到const FString的地方，大部分可以使用FName。
__注意:__,FName不区分大小写。
## FName如何解决
1.  对内存方面的优化
	1.	对于相同的字符串的FName,只保存该字符串的索引，实际字符串只有一份，使用Instance的设计模式。创建FName，如果字符串不存在，则创建字符串(分配内存,使用内存池分配)，并记录其索引；若字符串存在，则只保存索引；因为有内存池的存在，FName需要字符串时调用New的次数就大大减少，并且同样内容的FName只保存一份字符串；
	2.  检查到如果字符串完全是ANSCII的话，就用ANSCII保存。FName的索引最低位来判断是ASCII还是Widechar.
2.  对性能方面的优化
	1.	只有在第一次创建FName时有内存分配，之后创建同名的FName都是Hash链式查找
	2.	比较时只比较索引，速度很快

## FName如何实现

### FName我们的实现方式_1
1.	FName只保存到字符串的索引；
2.	字符串存在大的Hash_map中，key是字符串的索引；
#### 问题
1.	字符串的Hash值会碰撞，怎么解决唯一性?
2.	一般在加载一个Package的时候，会大量创建FName,如果每个Hashmap中放的是FString的话，会大量的调用New; 
3.	多线程创建时锁的问题

### FName我们的实现方式_2
1.  FName对应的数据FNameEntry（hash,index,char*)等在内存池中分配；
2.  FName只持有index; 通过一种映射方式（index-->FNameEntry*); 
3.  在创建FName时，通过字符串的Hash值，迅速找到FNameEntry*,来创建或者获取index; 
4.  创建时锁。
### FName UE的实现方式
UE 实现与实现方式_2接近，更多的考虑了page数据的对齐，cache的友好，数据的预取等，实现了效率，内存，扩展性，易用性的极致！！
1.	FName对应的数据存在FNameEntry{index,char*,FNameEntry* hashNext}; FName{ComparesionIndex}与FNameEntry{index}保持一致；
2.	FNameEntry保存在内存池（FNameEntryPoolAllocator ）中；
3.	FNameEntry*保存在TNameEntryArray中，TNameEntryArray就是个二维数组，每个维有64k个指针（正好是一个Page的大小），FName中保存的就是该数组的索引。
4.	有个无锁的Hash链表`NameHashHead`，保存了64k个链表头（一个page的大小），用来快速查找指定字符串对应的FNameEntry。即通过字符串hash来链式查找。


## FName实现细节  

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
					 3.    如果没有初始化，执行`FName::StaticInit()`来初始化FName类
					       1.   `FCrc::Init()`，其实在计算NonCasePerservingHash时已经使用过FCrc，这时FCrc的初始化只是检测下FCrc的Table是不是正确。发布版不检测，这是个空函数；
					       2.   设置GetIsInitialized() = true;  
					       3.   FName::NameHashHead[65536]为空; FName::NameHashTail[65536]为空
					       4.   之后进入CriticalSection
					            1.    获取`TNameEntryArray`  
					            2.    给Hard Code FName分配空间
					            3.    创建Hard Code 类型的FName.
			   2.    用NonCasePerservingHash在FName::NameHashHead中查找是否有与指定字符串同名的FName
			         1.  有同名的，返回对应的Index;
			         2.  没有同名的
			             1.  在TNameEntryArray分配FNameEntry*,返回Index。`OutIndex = TNameEntryArray.AddZero(1)`   
			             2.  FNameEntryPoolAllocator给FNameEntry分配空间，存入Index与字符串；
			             3.  FName::NameHashHead的尾部添上当前新创建的FNameEntry*;
			             4.  给FName::ComparisonIndex赋值Index;


### FNameEntry
FNameEntry主要保存每个FName的HashCode和内容，为变长类型。其基本成员如下  

	NAME_INDEX		Index;  
	YNameEntry*		HashNext;  
	union
	{
		ANSICHAR	AnsiName[NAME_SIZE];
		WIDECHAR	WideName[NAME_SIZE];
	};
	NAME_SIZE=1024;  

最关键的是如果保存的是ASCII，sizeof(FNameEntry)=2064,一般FName会有2M个，光名子开销就是4个G，肯定不能这样子分配。其实AnsiName/WideName就相当于一个指针，其大小是跟要分配的内容相关的。    
__为什么不用FString?__ 因为FString的开销很大，除了本身在64位下有16字节的大小，还需要一次内存分配。而FName使用内存池，保证一个FName字符串的大小与实际大小一致。
__为什么存Hash?__ 保证对比与查找的速度，比字符串比较快多了。
__为什么有宽窄两个版本?__  为了减少不必要的内存浪费，在构造函数的时候，会检查字符串能不能全部转化为ASCII，如果能就保存为ASCII。 

### FNameEntryPoolAllocator 
FNameEntry的内存分配器，非线程安全，就是FNameEntry的内存池，不过该内存池只负责创建不负责销毁，最主要的原因是没必要销毁，FNameEntry没有一个合适的销毁时间。FNameEntryPoolAllocator一次申请一个Page的大小内存，也就是64k。  
分配给一个FNameEntry的大小与FNameEntry中AnsiName的长度有关。
### TNameEntryArray
为FNameEntry*，（__注                                                                                             意__是FNameEntry的指针）的线程安全版本的一个二维数组管理器，与其名子不一样，FNameEntry的内存由FNameEntryPoolAllocator分配。                                                                                                                                                                                                              
TNameEntryArray的实现为  

		TStaticIndirectArrayThreadSafeRead<YNameEntry, 2 * 1024 * 1024 /* 2M unique YNames */, 16384 /* allocated in 64K/128K chunks */ > TNameEntryArray;
其中MaxTotalElements为2M个, ElementsPerChunk为16k个。也就是说TNameEntryArray是一个128维的，每个维有64k个元素。（为什么是64K,因为每个元素是指针，也就是说一个维度就是一个page的大小）。  
如何定位到一个元素 

	int32 ChunkIndex = Index / ElementsPerChunk;
	int32 WithinChunkIndex = Index % ElementsPerChunk;
如何分配内存  
先判断Chucks是否给当前Index分配内存，如果没有，就分配。见ExpandChuncksToIndex(),注意，这里使用了原子操作来保证多线程安全。  

如何释放内存
和FNameEntryPoolAllocator一样，不释放内存。原因如FNameEntryPoolAllocator不释放内存的原因一样。 


## FName 操作
FName::ToString ： 转化为String
FName::Compare  : 对比两个FName，用来排序