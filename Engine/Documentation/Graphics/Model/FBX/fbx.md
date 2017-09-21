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
## fbx类的作用关系
1. FbxScene
	1. 管理整个场景
		1. 场景层次关系组织
		2. 场景的内存分配
		3. RootNode和所有的Node
		4. scene information,global settings,global evaluator
	2. Evaluator 动画
2. FbxMesh 
	1. 从FbxGeometry集成而来，是FbxNodeAttribute
	2. 可以是多边形，也可以是三角形
	3. 

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

### FbxLayerElement
没有从FbxObject上继承而来，保存在FbxMesh中，用来储存各元素的映射信息。 
>fbxlyaer.h A layer element type is identified by EType. 
A FbxLayerElement describes how the layer element is mapped to a geometry surface
and how the mapping information is arranged in memory.
A FbxLayerElement contains Normals, UVs or other kind of information.

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

## FBX创建静态Mesh流程
1. 创建ControlPoints
	1. FbxMesh::InitControlPoints
	2. FbxVector4* lControlPoints = lMesh->GetControlPoints();
	3. lControlPoints[0] = FbxVector4(1,1,1);
	
2. 创建IndexBuffer
	1. int lPolygonVertices[] = { 0, 1, 2, 3, 4, 5, 6};
	
3. 创建三角形列表
	1. 对于每个三角形
	2. FbxMesh->BeginPolygon(-1, -1, -1, false);
	3. 添加顶点 
		1. FbxMesh->AddPolygon(ControlPointIndex);
	4. FbxMesh->EndPolygon();

4. 创建法线
	1. 先考虑分裂顶点后，定点与法线一一对应的情况
		1. 创建映射
				FbxGeometryElementNormal* lGeometryElementNormal = FbxMesh->CreateElementNormal(); 
		2. 设置映射模式为ebyControlPoints 
				lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
		3. 设置引用模式为eDirect
				lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);
		4. 设置对应关系
				lGeometryElementNormal->GetDirectArray().Add(FbxVector(1,1,1));

5. 创建UV
	1. 考虑索引方式
		1. 创建映射
				FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV("DiffuseUV"); //可以有多套UV
		2. 设置映射模式为eByPolygonVertex
				lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
		3. 设置引用模式为eIndexToDirect
				lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		4. 给DirectArray()就是存的是float2的数组赋值
				lUVDiffuseElement->GetDirectArray().Add(lVectors0); 
		5. 给IndexArray() 赋值
				1. lUVDiffuseElement->GetIndexArray().SetCount(24);
				2. FbxMesh->BeginPolygon(-1, -1, -1, false);
				3. lUVDiffuseElement->GetIndexArray().SetAt(顶点索引, UV索引);
				4. FbxMesh->EndPolygon()；
				5. 
6. 创建FbxMesh与FbxNode间的关系
		FbxNode* lNode = FbxNode::Create(pScene,pName);
	    // set the node attribute
	    lNode->SetNodeAttribute(lMesh);

7. 创建材质
	__注意__:材质一定要在mesh的拓扑关系确定后再添加
	1. 材质基类：FbxSurfaceMaterial
	2. 创建材质 
			FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());
	3. 添加材质
		1. 创建映射
				FbxGeometryElementMaterial* lMaterialElement = pMesh->CreateElementMaterial(); 
		2. 设置映射模式为eByPolygon
				lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
		3. 设置引用模式为eIndexToDirect
				lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
		4. 建立FbxMesh与FbxSurfaceMaterial的关系,下面会用到。因为Material与FNode是多对多的关系，一个FNode可以有多个Material,靠索引来查找
				  FbxNode* lNode = pMesh->GetNode();
	    		  lNode->AddMaterial(通过FbxSurfacePhone::Create()创建的Mtl));

			__注意__：这里不用创建DirectArray,FbxGeometryElementMaterial::GetDirectArray是Private的，要用上述方法来添加。原因是为了实现材质的复用。
		5. 创建IndexArray
			 	lMaterialElement->GetIndexArray().SetCount(6);
		6. 填充IndexArray
				lMaterialElement->GetIndexArray().SetAt(三角形索引,材质索引);
		
