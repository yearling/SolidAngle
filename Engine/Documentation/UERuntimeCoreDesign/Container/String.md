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
主要成员是`TArray<TCHAR> DataType`.
在Windows平台下TCHAR是WIDECHAR

## 构造函数
1. 基本的5种构造函数
2. 带ExtraSlack的拷贝/转移构造函数
3. 参数为C风格字符串的构造函数，注意：UE默认的如果类型为ASCII的字符串中有非ASCII的字符，会赋值为？？
4. 