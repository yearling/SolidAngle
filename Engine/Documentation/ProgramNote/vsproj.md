# VCXPROJ

## 文件作用
1. .vcxproj : project file（文件组织，编译）
2. .vcxproj.user Files ： debugging and deployment settings
3. .vcxproj.filters File ：  Solution Explorer tree view the file 

## XML MSBuild Project File
对应的就是.vcproj,相关官方文档如下：  
https://msdn.microsoft.com/en-us/library/dd293607.aspx  

1. 该XML的root是<Project>,
2. 子元素<ItemGroup> : project configuration and platform, source file name, and header file name.  
3. 子元素<Import> :   the location of Microsoft Visual C++ settings.  
4. 子元素<PropertyGroup>：specifies project settings.

### 如何创建一个.vcProj
1. 创建xml头

		<Project DefaultTargets="Build" ToolsVersion="12.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">  
		</Project>    

2. 添加ProjectConfiguration 到 ItemGroup子元素下

		<ItemGroup>  
		  <ProjectConfiguration Include="Debug|Win32">  
		    <Configuration>Debug</Configuration>  
		    <Platform>Win32</Platform>  
		  </ProjectConfiguration>  
		  <ProjectConfiguration Include="Release|Win32">  
		    <Configuration>Release</Configuration>  
		    <Platform>Win32</Platform>  
		  </ProjectConfiguration>  
		</ItemGroup>  

3. 添加Default C++ settings 在<Import>元素下 
 
		<Import Project="$(VCTargetsPath)\Microsoft.Cpp.default.props" />  

4. 添加工程属性(project properies)在<PropertyGroup>元素下,相当于工程属性上的[常规]设置 

		<PropertyGroup>  
		  <ConfigurationType>Application</ConfigurationType>  
		  <PlatformToolset>v120</PlatformToolset>  
		</PropertyGroup>   

5. 添加当前c++的设置在<Import>下

		<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />   

6. 添加CPP到<ItemGroup> 

		<ItemGroup>  
		  <ClCompile Include="main.cpp" />  
		</ItemGroup>  

7. 添加头文件到<ItemGroup>

		<ItemGroup>  
		  <ClInclude Include="main.h" />  
		</ItemGroup>  
8.  添加 the path of the file that defines the target for this project 

		<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Targets" />  