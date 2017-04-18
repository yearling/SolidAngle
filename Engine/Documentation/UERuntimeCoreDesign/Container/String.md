# String

关于编码的知识与bugs，可以见[UE](https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/StringHandling/CharacterEncoding/index.html)的官方文档，有详细介绍。 

## PlatformString
* 按照UE的跨平台的结构，首先有GenericPlatformString,然后有WindowsPlatformString来特化，最后通过typdef FWindowsPlatformString 为 FPlatformString
* 这里有点区别，继承链如下： FWindowsPlatformString -> FMicrosoftPlatformString -> FGenericPlaftformString,中间多了一层FMicrosoftPlatformString。
* UE中字符的类型  

		typedef YPlatformTypes::ANSICHAR	ANSICHAR;	///< An ANSI character. Normally a signed type.
		typedef YPlatformTypes::WIDECHAR	WIDECHAR;	///< A wide character. Normally a signed type.
		typedef YPlatformTypes::TCHAR		TCHAR;		///< Either ANSICHAR or WIDECHAR, depending on whether the platform supports wide characters or the requirements of the licensee.
		typedef YPlatformTypes::CHAR8		UTF8CHAR;	///< An 8-bit character containing a UTF8 (Unicode, 8-bit, variable-width) code unit.
		typedef YPlatformTypes::CHAR16	UCS2CHAR;	///< A 16-bit character containing a UCS2 (Unicode, 16-bit, fixed-width) code unit, used for compatibility with 'Windows TCHAR' across multiple platforms.
		typedef YPlatformTypes::CHAR16	UTF16CHAR;	///< A 16-bit character containing a UTF16 (Unicode, 16-bit, variable-width) code unit.
		typedef YPlatformTypes::CHAR32	UTF32CHAR;	///< A 32-bit character containing a UTF32 (Unicode, 32-bit, fixed-width) code unit.  

* 定义了一些元函数
   1.	`IsValidChar(Encoding Ch)`：
   			1.	ANSICHAR: `Ch >= 0x00 && Ch <= 0x7F;`
   			2.	别的都是true
   2.	`CanConvertChar`: A是有效的，并且AB之间可以互相转,转完后B也是有效的
   3.	`GetEncodingTypeName`  
   4.	`TIsFixedWidthEncoding`: ANSICHAR,WIDECHAR,UCS2CHAR为true
   5.	`TAreEncodingsCompatible`: A，B都要是FixedWidthEncodeing并且AB的size一样
   6.	`Convert`：将A转为B，如果B的大小太小，返回null，转化失败；如果转化成功，返回的是B的缓存的end的后一位

# MicrosoftPlatformString
1. 定义了一些C风格String的基本操作，如strcpy等，为ASCICHAR ,WIDECHAR有各自的特化版本
2. 定义了atoi等类型转换的函数，支持ASCICHAR ,WIDECHAR
3. __注意__ Stricmp()是支持ASCIChar与WIDEChar字符串相互比较

# Char 
位于Misc/Char

		typedef TChar<TCHAR>    FChar;
		typedef TChar<WIDECHAR> FCharWide;
		typedef TChar<ANSICHAR> FCharAnsi;
# TCString 
同时支持ASCIChar与WIDEChar,和各自的特化版本
		
		typedef TCString<TCHAR>    FCString;
		typedef TCString<ANSICHAR> FCStringAnsi;
		typedef TCString<WIDECHAR> FCStringWide;

TCString就是对FPlatformString的一个封装，使用时只用使用FCstring::*类型函数就行 


# FString
主要成员是TArray<TCHAR> DataType.
在Windows平台下TCHAR是WIDECHAR

## 构造函数
1. 基本的5种构造函数
2. 带ExtraSlack的拷贝/转移构造函数
3. 参数为C风格字符串的构造函数，注意：UE默认的如果类型为ASCII的字符串中有非ASCII的字符，会赋值为？？
4. 带长度的C风格字符串
5. TChar的拷贝构造


## Get
opeartor[]  
opeartor*()  
GetCharArray()  

## Iterator
CreateIterator  
CreateConsterator  

## Size 
GetAllocatedSize() 获取分配的内存大小
Empty(int32 Slack) 清空并分配默认大小
IsEmpty()    看TChar数量是不是<=1
Reset()      清空内存不销毁
Shrink()
Reserve()  

## Check
CheckInvariants  
IsValidIndex
## TCHAR*
operator *  

## Add
AppendChar  
AppendChars   
operator+=   
Append  
InsertAt  
PathAppend 添加一个`/`
operator/= 调用PathAppend
opeartor/  添加一个`/`  
operator+   __注意__:右值的特殊处理  

## Remove
RemoveAt
RemoveFromStart
RemoveFromEnd

## compare
operator<= :case **IN**sensitive  
operator<  :case **IN**sensitive 
operator>= :case **IN**sensitive
operator>  :case **IN**sensitive
operator== :case **IN**sensitive
operator!_ :case **IN**sensitive 
Equals(是否大小写敏感)  
Compare(是否大小写敏感)  
## length 
Len() : 返回字符串的长度，不包含最后的'\n'

## part
Left 
LeftChop : return the left most characters form the string chopping the given number of charactors from the end.
Right:
RightChop:
Mid:
Split:  
Trim： Remove the whitespace characters from the front of this string 
TrimTrailing : Removes trailing whitespace characters  
TrimToNullTerminator:　
TrimQuotes：　　
ParseIntoArray：　　
ParseIntoArray：　使用" ","\t","\r","\n","" 为分割符  
ParseIntoArrayLines： 使用"\r","\r\n","\n" 为分割符
CullArray： 传进去一个TArray<YString>,将为空的string移去  

## find
Find(匹配字符串，大小写敏感，查找方向，开始查找位置)  
Contains(匹配字符串，大小写敏感，查找方向)  
FindChar  
FindLastChar  
FindLastCharByPredicate
StartsWith
EndsWith  
MatchesWildcard : 通配符


## 大小写  
ToUpper()：返回一个大写的copy
ToUpperInline():将本身的字符转为大写
ToLower():
ToLowerInline():  

## Pad 
LeftPad 
RightPad  

## Numeric
IsNumeric() 
FormatAsNumber: 

## format
Format  
FromInt  
AppendInt  
ToBool  
SanitizeFloat: 去掉float后的0 
FromBlob  
ToBlob  
FromHexBlob
ToHexBlob  
Join(TArray,TChar* Seperator): 
BytesToString  
StringToBytes  
NibbleToTChar  
ByteToHex  
BytesToHex
CheckTCharIsHex  
TCharToNibble
HexToBytes  

## Chr
Chr():'char'+'\n'
ChrN(): 'char'*N+ '\n'  

## 反转
Reverse(): 返回一个Reverse的copy  
ReverseString();

## 替换
Replace(): 返回替换关键词的copy
ReplaceInline():  
ReplaceQuotesWithEscapedQuotes(): 引号变为转义引号
ReplaceCharWithEscapedChar():转义字符变为转义字符串,比如\n变为\\\n,支持 { \n, \r, \t, \', \", \\ }. 
ReplaceEscapedCharWithChar： 反过来，\\\n转为\n  
ConvertTabsToSpaces: 

## Serialize
Serialize  
SerializeAsANSICharArray

## Traits 
TIsContiguousContainer<YString>::Value = true;
TIsZeroConstructType<YString>::value = true; 
TContainerTraits<YString> ::MoveWillEmptyContainer  

## 全局函数
GetData  
GetNum  
__GetTypeHash__:

## 字符类型转换
TCHAR_TO_ANSI(str)
ANSI_TO_TCHAR(str)
TCHAR_TO_UTF8(str)
UTF8_TO_TCHAR(str)
__注意__:这些对象的生命周期很短，一般用来函数参数的转化，不能在函数作用域外使用这些转化后的对象  
__注意__:只能传入指针，不能是char类型  

		SomeApi(TCHAR_TO_ANSI(SomeUnicodeString));
		
		const char* SomePointer = TCHAR_TO_ANSI(SomeUnicodeString); <--- Bad!!!
