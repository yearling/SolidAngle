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
				|- RecursiveFixSkeleton() 修正因为ActorX导出的Mesh为bone的情况，将Mesh替换为bone
			
			|- 如果outSkelMeshArray不为空，说明有可以导入的模型
				|- 如果是SkeletalMesh,对于每组SkeletonMesh(有可能一个fbx中，有两组skeleton mesh)
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
											|- 统计Cluster对应的Link
											|- 对于每个link,获取每个link相对于Pose的位置（位于模型空间），然后乘以父亲节点变换的逆，得到相对于父节点的变换
										|- 把UI中的变换应用于根骨骼
								|- ApplyTransformSettingsToFbxNode(),把UI中指定的变换应用到场景根节点
								|- 收集材质
								|- 对**每个**SkeletonMesh调用FillSkelMeshImporterFromFbx(),填充FSkeletalMeshImportData结构体
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
							|- ProcessImportMeshMaterials()
							|- ProcessImportMeshSkeleton()
							|- ProcessImportMeshInfluences()
								|- 对FSkeletalMeshImportData::Influences中ControlPoint其索引排序；
								|- 遍历FSkeletalMeshImportData::Influences，查看是否存在受8根以上骨骼影响的顶点，给出Warning;Normalize顶点权重；
								|- 删掉8根骨骼权重以上的权重，删掉小于0.00001的权重，normalize顶点权重
							|- bCreateRenderData
								|- IMeshUtilities::BuildSkeletalMesh(ImportedResource->LODModels[0]/*要填入的数据*/）)
									|- FSkeletalMeshUtilityBuilder::PrepareSourceMesh 算切空间
										|- Skeletal_FindOverlappingCorners()
											|- 将3维的Vertex的hash到一维，然后进行排序，之后对排序的解果进行对比，得到在空间上重合的点，O(n^2)复杂度。
										|- Skeletal_ComputeTangents_MikkTSpace()
											使用Mikkt来计算Tangent空间
											|- Skeletal_ComputeTriangleTangents()
												计算面法线
											|- 对于每个面的顶点，找出与顶点相邻的面（顶点位置一样或者索引一样，位于同一光滑组），平分法线
												|- 粗选： 选择与该面相临的三角形（距离）,相临信息存在AdjacentFaces中，
												|- 细选：每个顶点一个相邻面的list，存在RelevantFacesForCorner[3]中，
												|- 再细选： 面光滑组相同 && （bBlendOverlappingNormals ||顶点的索引相同）
												|- 平均面法线并保存到顶点
                                            |- 调用Mikkt的库，计算每个顶点的Tangent.
									|- FSkeletalMeshUtilityBuilder::GenerateSkeletalRenderMesh() 优化Mesh,根据材质切分Mesh
										|- 检查每个Wedge是否有一个Influence
										|- 将读取到的原始数据，拼成TArray<FSoftSkinBuildVertex> RawVertices，FSoftSkinBuildVertex{Position,TangentXYZ,UVs,Color,InfluenceBones,InfluenceWeights,PointWedgeIdx},其实就是扩展后的Wedge，说白了就是以前的一个Wedge可以被多个三角形引用,现在相当与把引用的Wedge实例化了，每个顶点的Wedge保存数据而不是索引。RawVertices按着三角形顶点顺序一个一个排。
										|- SkeletalMeshTools::BuildSkeletalMeshChunks() 
											|- 因为材质ID存在于面上，所以对位于同一边但属于两个不同材质的三角形的顶点来说，需要拆分。每个属于同一个材质的Mesh保存到FSkinnedMeshChunk中去 
											|- 对扩展后的Wedge,也就是RawVertices的进行排序，以便加速查询，像上面算切空间一样
											|- 删掉重复的顶点（因为上一步生成RawVertices时拆分了顶点
											|- 生成Index；这时候就完成了材质的拆分；
										|- SkeletalMeshTools::ChunkSkinnedVertices() 防止骨骼太多来拆分Chunk,比如有257根骨头，引用第256根骨头的顶点就会放到新的Chunk中。
										|-  FMeshUtilities::BuildSkeletalModelFromChunks() //生成可渲染数据
											|- FSkinnedMeshChunk是MeshUtilities中的数据结构；FSkelMeshSection是Engine中用来渲染的数据结构，所以要把FSkinnedMeshChunk转化为FSkelMeshSection
												|- 对Index进行优化
										|- 	YSkeletalMesh::	CalculateInvRefMatrices(),计算每根骨骼到LocalSpace的逆
							|- 创建FSkeleton
								|- FSkeleton::MergeAllBonesToBoneTree,从FSkeletalMesh创建FSeleton
											
