# Fbx
通用的模型交换格式

## 关注点
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

## 编译注意
1. 链接库
	1. 动态 Libfbxsdk.lib, fbxsdk-<version>.dll ，__注意__:编译时需要带`FBXSDK_SHARED`宏
	2. 静态
		1.  `/MT`: LIBCMT.lib静态链接
		2.  `/MD`: LIBCMT.lib动态链接

## 映射方式
决定每个元素怎么映射到一个平面上  

	enum EMappingMode
	{
		eNone,
		eByControlPoint,
		eByPolygonVertex,
		eByPolygon,
		eByEdge,
		eAllSame
	};
1. eNone：The mapping is undetermined.
2. __eByControlPoint__: There will be one mapping coordinate for each surface control point/vertex.
3. __eByPolygonVertex__:There will be one mapping coordinate for each vertex, for every polygon of which it is a part.This means that a vertex will have as many mapping coordinates as polygons of which it is a part. 
4. eByPolygon: There can be only one mapping coordinate for the whole polygon.
5. eByEdge:   There will be one mapping coordinate for each unique edge in the mesh.This is meant to be used with **smoothing layer elements**.
6. eAllSame:  There can be only one mapping coordinate for the whole surface.

## 索引方式
Determines how the mapping information is stored in the array of coordinates. 

	enum EReferenceMode
	{
		eDirect,
		eIndex,
		eIndexToDirect
	};

1. **eDirect**: This indicates that the mapping information for the n'th element is found in the n'th place of `FbxLayerElementTemplate::mDirectArray`.
2. eIdex: This symbol is kept for backward compatibility with FBX v5.0 files. In FBX v6.0 and higher,this symbol is replaced with eIndexToDirect.
3. **eIndexToDirect**:This indicates that the `FbxLayerElementTemplate::mIndexArray` contains, for the n'th element, an **index** in the `FbxLayerElementTemplate::mDirectArray` array of mapping elements. eIndexToDirect is usually useful for storing **eByPolygonVertex mapping mode** elements coordinates. Since the same coordinates are usually repeated many times, this **saves spaces** by storing the coordinate only one time and then referring to them with an index. Materials and Textures are also referenced with this mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray.

## Geometry Element

	/**  Defines geometry element classes.
	  *  A geometry element describes how the geometry element is mapped to a geometry surface
	  *  and how the mapping information is arranged in memory.
	  *  \remarks Geometry elements are independent of layer elements and hide the complexity of layers.
	  *  \code
	  *  FbxGeometryElementUV* lUVs = lMesh->GetElementUV("map1");
	  *  FbxGeometryElementUV::DirectArrayType lDirectArray = lUVs->GetDirectArray();
	  *  int lDirectUVCount = lDirectArray.GetCount();
	  *  FbxVector2 lFirstUV = lDirectArray[0];
	  *  \endcode
	  *  \see FbxGeometryBase
	  */

typedef FbxLayerElement FbxGeometryElement;
typedef FbxLayerElementNormal FbxGeometryElementNormal;
typedef FbxLayerElementBinormal FbxGeometryElementBinormal;
typedef FbxLayerElementTangent FbxGeometryElementTangent;
typedef FbxLayerElementMaterial FbxGeometryElementMaterial;
typedef FbxLayerElementPolygonGroup FbxGeometryElementPolygonGroup;
typedef FbxLayerElementUV FbxGeometryElementUV;
typedef FbxLayerElementVertexColor FbxGeometryElementVertexColor;
typedef FbxLayerElementUserData FbxGeometryElementUserData;
typedef FbxLayerElementSmoothing FbxGeometryElementSmoothing;
typedef FbxLayerElementCrease FbxGeometryElementCrease;
typedef FbxLayerElementHole FbxGeometryElementHole;
typedef FbxLayerElementVisibility FbxGeometryElementVisibility

