
[引用](http://en.cppreference.com/w/cpp/language/types)

>wchar_t - type for wide character representation (see wide strings). Required to be large enough to represent any supported character code point (32 bits on systems that support Unicode. A notable exception is Windows, where wchar_t is 16 bits and holds UTF-16 code units) It has the same size, signedness, and alignment as one of the integral types, but is a distinct type.

wchart_t是的大小要求可以放得下code point，一般来说是32bit，保存UniCode，但**Widnows是个例外，放是是16bit的UTF16**

