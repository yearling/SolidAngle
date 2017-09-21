# FbxSkeletonAnimation
主要由类FbxSkeleton来实现，FbxSkeleton继承于FbxNodeAttribute,类型为`FbxNodeAttribute::EType::eSkeleton`

## Bone type
	  enum EType
	  {
	    eRoot,			/*!< First element of a chain. */
	    eLimb,			/*!< Chain element.a bone defined by a transform and a length */
	    eLimbNode,		/*!< Chain element. a bone defined uniquely by a transform and a size value while*/
	    eEffector		/*!< Last element of a chain. */
	  };    

## LimbLength
>FbxSkeleton is a node attribute and it will be attached to a FbxNode which represents the transform.
Given a chain of skeleton nodes the parent and child skeletons will be attached to a parent node and a child node.
The orientation of the limb is computed from the vector between the parent and child position (from parent to child). 
The **LimbLength** represents the proportion 
of the parent node's position to the child node's position which is used to compute the actual limb length.
The default value of 1.0 means the LimbLength is equal to the length between the parent and child node's position.
So if the value is 0.5, it means the LimbLength will be half of the length between the parent and child node's position.
LimbLength是父子node节点实际距离的一个比值

size:指的是在maya或者max中显示骨骼的大小，ui层面的

## FbxCluster
>A cluster, or link, is an entity acting on a geometry (FbxGeometry).More precisely, the cluster acts on a subset of the geometry's control points.For each control point that the cluster acts on, the intensity of the cluster's
action is modulated by a weight. The link mode (ELinkMode) specifies how the weights are taken into account.

>The cluster's link node specifies the node (FbxNode) that influences the control points of the cluster. If the node is animated, the control points will move accordingly.
>Each node influences some control
points of the mesh. A node has a high influence on some of the points (high weight)
and lower influence on some other points (low weight). Some points of the mesh
are not affected at all by the bone, so they would not be part of the corresponding
cluster.

FbxCluster对应的是maya skin中的cluster，一个Cluster对应的是一个Mesh的部分顶点，也就是说，一个mesh可以包含多个Cluster，但一个Cluster只能对应一个Mesh.

Cluster::AddControlPoints建立
### Link modes.
The link mode sets how the link influences the position of a control point and the relationship between the weights assigned to a control point. The weights assigned to a control point are distributed among the set of links associated with an instance of class FbxGeometry.

	enum ELinkMode
	    {
			eNormalize,       /*!<	  In mode eNormalize, the sum of the weights assigned to a control point
			                          is normalized to 1.0. Setting the associate model in this mode is not
			                          relevant. The influence of the link is a function of the displacement of the
			                          link node relative to the node containing the control points.*/
	        eAdditive,
			                  /*!<    In mode eAdditive, the sum of the weights assigned to a control point
			                          is kept as is. It is the only mode where setting the associate model is
			                          relevant. The influence of the link is a function of the displacement of
			                          the link node relative to the node containing the control points or,
			                          if set, the associate model. The weight gives the proportional displacement
			                          of a control point. For example, if the weight of a link over a control
			                          point is set to 2.0, a displacement of the link node of 1 unit in the X
			                          direction relative to the node containing the control points or, if set,
			                          the associate model, triggers a displacement of the control point of 2
			                          units in the same direction.*/
	        eTotalOne   
			                  /*!<    Mode eTotalOne is identical to mode eNormalize except that the sum of the
			                          weights assigned to a control point is not normalized and must equal 1.0.*/
	    };
### LinkMatrix

	A link has three transformation matrices:
	     \li Transform refers to the global initial transform of the geometry node that contains the link node.
	     \li TransformLink refers to global initial transform of the link node.
	     \li TransformAssociateModel refers to the global initial transform of the associate model.
	
	For example, given a mesh binding with several bones(links), Transform is the global transform 
	of the mesh at the binding moment, TransformLink is the global transform of the bone(link)
	at the binding moment, TransformAssociateModel is the global transform of the associate model 
	at the binding moment.
1. TransformMatrix: mesh相对于模型空间的变换矩阵；
2. TransformLinkMatrix: bone相于模型空间的变换矩阵；

### fbx骨骼变换方程
1. 待解决的问题
   一些顶点以骨骼为导向，绕着骨骼进行位置变换 ==》 在骨骼空间的顶点按照骨骼变换矩阵变换，然后再还原回local空间去渲染，所以需要3个矩阵：1. MatLocalToBone;2. MatBoneTransform;3. MatMeshToLocal; 
2. 变换矩阵为(向量为行向量）：
   VLocal = VMesh *　MatMeshToLocal* MatLocalToBone * MatBoneTransform* (MatLocalToBone的逆）；
   其中 MatMeshTolocal为Cluster::TransformMatrix,MatLocalToBone是Cluster::TransformLinkMatrix的逆

### FbxSkin
FbxSkin和我们一般意义上的Skin有所不同，FbxSkin是作用于**一个**Mesh上的所有顶点变换（骨骼，morph)的集合。__注意__:是一个mesh,因为Cluster中存的是Controlpointer的索引，如果是两个mesh的话，就引起歧义了。 

>A skin deformer contains clusters (FbxCluster). Each cluster acts on a subset of the geometry's
control points, with different weights. For example, a mesh of humanoid shape
can have a skin attached, that describes the way the humanoid mesh is deformed
by bones. When the bones are animated, the clusters act on the geometry to
animate it too.

## FbxMesh,FbxCluster,FbxSkeleton,FbxSkin,FbxAnimationLayer的关系

FbxMesh里包含顶点；一个FbxMesh里包含多个FbxSkin，每个FbxSkin里包含多个FbxCluster，每个Cluster建立Mesh中顶点与FbxSkeleton中谷骨骼的关系。 FbxAnimationLayer中包含骨骼变换数据。

## FbxPose
Pose信息只有Maya中使用，从Max中导出的Fbx没有Pose的信息，在导入到Fbx时会自动创建Pose.FbxPose中包含骨骼变换矩阵为Identity的时候的标准姿态。 
### 创建方式
1. 收集所有的Mesh与骨骼
2. 创建一个FbxPose
		FbxPose* lPose = FbxPose::Create(pScene, "SelfPose");
3. 设置为BindPose
	lPose->SetIsBindPose(true);
4. 添加每个Mesh的node与bindpose时的相对于Global的变换矩阵
	
		FbxNode*  lKFbxNode   = lClusteredFbxNodes.GetAt(i);
        FbxMatrix lBindMatrix = lKFbxNode->EvaluateGlobalTransform();
        lPose->Add(lKFbxNode, lBindMatrix);
		这里没有检查返回值，要检查返回值。（同一Node如果有两个不同的BindPose会返回错误）
5. 添加当前FbxPose到Scene
		pScene->AddPose(lPose);