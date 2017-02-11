###[__64是MSVC扩展的](https://msdn.microsoft.com/en-us/library/29dh1w7z.aspx#)
Microsoft C/C++ features support for sized integer types. You can declare 8-, 16-, 32-, or 64-bit integer variables by using the __intn type specifier, where n is 8, 16, 32, or 64.

The following example declares one variable for each of these types of sized integers:

__int8 nSmall;      // Declares 8-bit integer  
__int16 nMedium;    // Declares 16-bit integer  
__int32 nLarge;     // Declares 32-bit integer  
__int64 nHuge;      // Declares 64-bit integer  

The types __int8, __int16, and __int32 are synonyms for the ANSI types that have the same size, and are useful for writing portable code that behaves identically across multiple platforms. The __int8 data type is synonymous with type char, __int16 is synonymous with type short, and __int32 is synonymous with type int. The __int64 type has no ANSI equivalent.