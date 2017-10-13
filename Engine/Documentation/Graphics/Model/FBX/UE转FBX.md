# UE convert FBX
1. /source/Editor/UnrealEd/Fbx 下

## 坐标系
![Add](Pic/Coordinates.png)

## importer

1. 首先调用GetImportType,来获取fbx里的基本信息（StaticMesh,SkeletonMesh,Morph,Bones)等等，然后提供给UI让用户选择导入
	FFbxImporter::GetImportType   //第一遍解析场景，
		|- FFbxImporter::OpenFile // 创建FbxImpoter
		|- FFbxImporter::GetSceneInfo 
			|- FFbxImporter::ImportFile 
				|- 创建FbxScene,import scene
				|- FixMaterialClashName() //重复的材质名后面加上_后缀  		
			|- 解析FbxScene 填入 FbxSceneInfo（材质数量，纹理数量，图元数量，Skin数量）
			|- FbxScene::GetGeometryCount()来获取图元数量，遍历场景中的图元
				|- 如果图元的属性是Mesh
					|- 通过Mesh->GetDeformerCount(FbxDeformer::eSkin)来判断是不是Skin
						|- 如果是SKin判断Lod
						|- 填充FbxMeshInfo(名子，是否三角化，材质数量，面数，顶点数,LOD,LOD级别，是否是SkinMesh,根骨骼名子，骨骼数量，Morph数量)，并存入FbxSceneInfo
			|- 判断场景中是否有动画（3ds max导出后有translation/rotation两个固定的动画曲线，要从第三条曲线开始判断有没有动画）
			|- 获取场景帧率
			|- 获取场景动画时间
			|- 获取节点间的变换关系
				|- 获取RootNode的变换信息(节点名，UniuqeID, Transform)并填入FbxNodeInfo,并将FbxNodeInfo节点放入FbxSceneInfo::HierachyInfo.
				|- 深度优先遍历整个Scene，将节点信息存入FbxSceneInfo::HierachyInfo.

2. 根据类型选择具体导入的类型
	1. UnFbx::FBXImportOptions* ImportOptions = FbxImporter->GetImportOptions();
	根据需求设置导入选项。

	2. 导入SkeletonMesh 
	
			|- FFbxImport::ImportFromFile
				|- FFbxImport::OpenFile  同上
				|- FFbxImport::ImportFile 同上（Scene重建了一次）
				|- ConvertScene() 坐标系与单位变换
				|- ValidateAllMeshesAreReferenceByNodeAttribute() 保证每个Mesh都保存在了NodeAttribute中
		  	|- 如果导入的是SkeletonMesh
				|- FillFbxSkelMeshArrayInScene 
					|- RecursiveFindFbxSkelMesh[outSkelMeshArray: 每级lod的skinmeshes，只有一级LOD的话，outSkelMeshArray有一个元素，该元素有所有的SKinMesh; SkeletonArray:每级LOD的骨骼根节点，只有一级LOD的话，SkeletonArray中只保存骨骼根节点
			
			|- 如果outSkelMeshArray不为空，说明有可以导入的模型
				|- 如果是SkeletalMesh
					|- 对于outSkelMeshArray中组skelMesh中的每层LOD
						|- YSkeletalMesh* NewMesh = ImportSkeletalMesh(nullptr, SkelMeshNodeArray, OutputName, &SkeletalMeshImportData, LODIndex, &bOperationCanceled);
							|- CheckSmoothingInfo()
							|- FillLastImportMaterialNames() // 目前不知道干什么
							|- FillSkeletalMeshImportData() // 最后一遍检查导入数据是否合格，如果不合格就不导入；因为之后会销毁同名的skeletonMesh,如果导入失败，之前导入的也不能用了
								|- ImportBone()
									|- RetrievePoseFromBindPose()
										|- 检查Fbx自带的BindPose()是否正确,通过调用FbxPose::IsValidBindPoseVerbose()
									|- 如果BindBose不正确
										|- 销毁当前BindPose()
										|- 通过SDK重建BindPose()
										|- BuildSkeletonSystem() //遍历所有的mesh对应的cluster,收集所有cluster的link，放到SortedLinks中
											|- RecursiveBuildSkeleton()
										|- 查找有没有同名骨骼
										|- 创建UnrealBone的层次结构（将FbxSkeleton用自己的VBone结构来表示，保存层级结构，保存骨骼位姿信息（相对于父骨骼来说，Fbx的每根骨骼都是相对于模型空间）
										|- 把UI中的变换应用于根骨骼
								|- ApplyTransformSettingsToFbxNode(),把UI中指定的变换应用到场景根节点
								|- 添加材质
								|- 对**每个**SkeletonMesh调用FillSkelMeshImporterFromFbx()
									|- 去掉BadPolygons
									|- 获取UVLayers
									|- 根据UV名（UVChannel_1)来重新对UV集排序
									|- 获取材质
									|- 建立当前mesh材质到导出的所有材质的索引；
									|- 检查光滑组，如果没有创建
									|- Trianglation Mesh
									|- 获取UV
									|- 获取光滑组
									|- 获取材质
									|- 获取顶点色
									|- 获取法线
									|- 获取切线
									
								|- 如果使用T0的姿势作为Pose
									|- SkinControlPointsToPose()
								|- 去掉没有使用过的材质:CleanUpUnsuedMaterials
								|- 对材质重排序
								|- 调用DoUnSmoothVerts() : 只在NormalGenerationMethod!= MikkTSpace下有用
									|- 记录每个ControlPoint到包含该ControlPoint的Wedge的映射 
									|- 记录每个ControlPoint到包含该ControlPoint的Weight的映射
										这些是用来分裂顶点时用 
									|- 记录每个ControlPoint到包含该ControlPoint的面的映射 
										用来深度遍历整个图，整个问题变成一个联通图问题，即如果两个三角形共用一条边（Wedge的ControlPoint相同）并且光滑组一致，则这两个三角形位于同一光滑组。
										通过对引用每个顶点的三角形面片用上述方法划分光滑组，如果有两个以上的光滑组，则分裂ControlPoint. 
										注意：如果使用MikkTSpace的话，分裂了顶点但是没有分裂wedge,基本上相当于没有操作。