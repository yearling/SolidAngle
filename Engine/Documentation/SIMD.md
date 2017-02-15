#SSE SIMD

##DirectXMath
[Introducing DirectXMath](https://blogs.msdn.microsoft.com/chuckw/2012/03/26/introducing-directxmath/) 主要介绍了DirectXMath数学库不再依赖<windows.h>头文件。

[DirectXMath注意事项](https://msdn.microsoft.com/en-us/library/windows/desktop/ee418732(v=vs.85).aspx#Properly_Align_Alloc) 介绍了

1.	SSE指令对内存16bit对齐的速度较快，x64和x86在栈上的变量和和全局变量都是16位对齐的内存分配，但malloc时，对x64来说是16位对齐的，x86来说是8位对齐的，故对于x86使用SSE时，要注意分配器的16位对齐。
2.	For Windows x86 targets, enable /arch:SSE2. For all Windows targets, enable /fp:fast.