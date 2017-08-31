# PDB
PDB 用来放调试信息，一个工程有两个pdb
1. vcx0.pdb: 用来放每个Obj的的调试信息
2. project.pdb:用来放整个exe的所有调试信息。该pdb的绝对路径放到exe的二进制文件中。

## 官网解释
[PDB微软官网解释](https://msdn.microsoft.com/en-us/library/yd4f8bd1(v=vs.100).aspx)
> A program database (PDB) file holds debugging and project state information that allows incremental linking of a Debug configuration of your program. A PDB file is created when you build with /ZI or /Zi (for C/C++).
In Visual C++, the /Fd option names the PDB file created by the compiler. When you create a project in Visual Studio using wizards, the /Fd option is set to create a PDB named project.PDB.
If you build your C/C++ application using a makefile, and you specify /ZI or /Zi without /Fd, you end up with two PDB files:
VCx0.PDB, where x represents the version of Visual C++, for example VC100.PDB. This file stores all debugging information for the individual OBJ files and resides in the same directory as the project makefile.
project.PDB   This file stores all debug information for the.exe file. For C/C++, it resides in the \debug subdirectory.
Each time it creates an OBJ file, the C/C++ compiler merges debug information into VCx0.PDB. The inserted information includes type information but does not include symbol information such as function definitions. So, even if every source file includes common header files such as <windows.h>, the typedefs from those headers are stored only once, rather than being in every OBJ file.
The linker creates project.PDB, which contains debug information for the project's EXE file. The project.PDB file contains full debug information, including function prototypes, not just the type information found in VCx0.PDB. Both PDB files allow incremental updates. The linker also embeds the path to the .pdb file in the .exe or .dll file that it creates.
The Visual Studio debugger uses the path to the PDB in the EXE or DLL file to find the project.PDB file. If the debugger cannot find the PDB file at that location or if the path is invalid (for example, if the project was moved to another computer), the debugger searches the path containing the EXE, the symbol paths specified in the Options dialog box (Debugging folder, Symbols node). If the debugger cannot find a .PDB file, a Find Symbols dialog box appears, which allows you to search for symbols or to add additional locations to the search path.