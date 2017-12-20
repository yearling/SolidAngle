# SkeletaMeshRender

## Relative files
1. GPUSkinVertexFactory.h/cpp
2. SkeletalRenderGPUSkin.h/cpp  
	`FSkeletalMeshObjectGPUSkin::UpdateDynamicData_RenderThread` 
	`FSkeletalMeshObjectGPUSkin::ProcessUpdatedDynamicData`
3. SkeletalRenderCPUSkin.h/cpp
4. SkinWeightVertexBuffer.h/cpp
4. SkeletalRenderPublic.h


## Resource Createion

在启动编辑器时下断点，可见 

 	|- UActorComponent::ExecuteRegisterEvents()
		|-  USkinnedMeshComponent::CreateRenderState_Concurrent() 
			|- 通过一系统条件判断是GPUSkin还是CPUSkin(骨骼数量等） SkelMeshResource->RequiresCPUSkinning()
				|- MeshObject = ::new FSkeletalMeshObjectGPUSkin(this, SkelMeshResource, SceneFeatureLevel);

## Rendering
	|- UWorld::SendAllEndOfFrameUpdates()
			|- UActorComponent::DoDeferredRenderUpdates_Concurrent()
				|- USkinnedMeshComponent::SendRenderDynamicData_Concurrent() 
					|- FSkeletalMeshObjectGPUSkin::Update	|- FDynamicSkelMeshObjectDataGPUSkin中存的是要渲染的数据，从game thread ==> Render thread. 
						|- 
		
			注意：RenderThread
						|- MeshObject->UpdateDynamicData_RenderThread(RHICmdList, NewDynamicData, FrameNumberToPrepare);
							|-FSkeletalMeshObjectGPUSkin::UpdateDynamicData_RenderThread()
								|- FSkeletalMeshObjectGPUSkin::ProcessUpdatedDynamicData()
									|- 设置vbFGPUBaseSkinVertexFactory::FShaderDataType::UpdateBoneData()
									   更新骨骼矩阵
