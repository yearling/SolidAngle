# Fbx
通用的模型交换格式

# 关注点
1. 手系
	1. 建模软件的手系：maya, max的手系 
	2. 引擎的手系: gl,dx
2. 长度单位
	1. 建模软件的单位
	2. 引擎的单位
3. 欧拉角
	1. 骨骼旋转的欧拉角
	2. 引擎欧拉角
4. 切线空间及法线导出
5. 光滑组导出
6. LOD
4. 材质导出

# 编译注意
1. 链接库
	1. 动态 Libfbxsdk.lib, fbxsdk-<version>.dll ，__注意__:编译时需要带`FBXSDK_SHARED`宏
	2. 静态
		1.  `/MT`: LIBCMT.lib静态链接
		2.  `/MD`: LIBCMT.lib动态链接