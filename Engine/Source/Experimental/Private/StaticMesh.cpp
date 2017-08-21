#include "StaticMesh.h"
#include "Canvas.h"
#include "FbxReader.h"
#include "FbxUtility.h"


// Scale all the elements of a matrix.
void MatrixScale(FbxAMatrix& pMatrix, double pValue)
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pMatrix[i][j] *= pValue;
		}
	}
}


// Add a value to all the elements in the diagonal of the matrix.
void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
{
	pMatrix[0][0] += pValue;
	pMatrix[1][1] += pValue;
	pMatrix[2][2] += pValue;
	pMatrix[3][3] += pValue;
}


// Sum two matrices element by element.
void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			pDstMatrix[i][j] += pSrcMatrix[i][j];
		}
	}
}



StaticMesh::StaticMesh()
{

}

StaticMesh::~StaticMesh()
{

}

bool StaticMesh::AllocResource(TComPtr<ID3D11Device> device, TComPtr<ID3D11DeviceContext> dc)
{
	if (FaceNum != 0)
	{
		CreateVertexBufferDynamic(device, (UINT)VertexArray.Num() * sizeof(LocalVertex), &VertexArray[0], m_VB);
		CreateIndexBuffer(device, (UINT)IndexArray.Num() * sizeof(int), &IndexArray[0], m_IB);
	}
	return true;
}

void StaticMesh::Clear()
{
	VertexArray.Empty();
	MapTriangleIndexToMaterialIndex.Empty();
	FaceNum = -1;
}


void StaticMesh::Render(TComPtr<ID3D11DeviceContext> dc, TComPtr<ID3D11Buffer> cb)
{
	if (FaceNum == 0)
		return;

	D3D11_MAPPED_SUBRESOURCE MapResource;
	auto hr = dc->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);

	PerFMeshCBuffer &cbPerMesh = (*(PerFMeshCBuffer*)MapResource.pData);
	cbPerMesh.m_matWrold = MatWorld.GetTransposed();
	dc->Unmap(cb, 0);
	//m_VS->BindResource("g_world", XMMatrixTranspose(MatWorld));

	D3D11_MAPPED_SUBRESOURCE VBMapResource;
	hr = dc->Map(m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBMapResource);
	LocalVertex *pLocalVertexArray = (LocalVertex *)VBMapResource.pData;
	memcpy(pLocalVertexArray, &VertexArray[0], sizeof(LocalVertex)*VertexArray.Num());
	dc->Unmap(m_VB,0);

	UINT strid = sizeof(LocalVertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &(m_VB), &strid, &offset);
	dc->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->VSSetConstantBuffers(1, 1, &cb);
	dc->PSSetConstantBuffers(1, 1, &cb);
	//m_VS->Update();
	//m_PS->Update();
	dc->DrawIndexed(IndexArray.Num(), 0, 0);
}

void StaticMesh::UpdateVertexPosition(FbxMesh* pMesh, FbxVector4* pVertexArray)
{
	for (int TriangleIndex = 0; TriangleIndex < FaceNum; ++TriangleIndex)
	{
		for (int i = 0; i < 3; ++i)
		{
			int CtrlPointIndex = pMesh->GetPolygonVertex(TriangleIndex, i);
			VertexArray[TriangleIndex*3+i].Position.x = static_cast<float>(pVertexArray[CtrlPointIndex][0]);
			VertexArray[TriangleIndex*3+i].Position.y = static_cast<float>(pVertexArray[CtrlPointIndex][1]);
			VertexArray[TriangleIndex*3+i].Position.z = static_cast<float>(pVertexArray[CtrlPointIndex][2]);
		}
	}
}

MeshModel::MeshModel()
{
	m_cbPerMesh = nullptr;
}

void MeshModel::Init(TComPtr<ID3D11Device> device, TComPtr<ID3D11DeviceContext> dc)
{
	m_VS = MakeUnique<YVSShader>();
	if (!m_VS->CreateShader("..\\..\\Source\\Experimental\\Private\\RenderMesh.hlsl", "VSMain"))
	{
		assert(0);
	}
	m_PS = MakeUnique<YPSShader>();
	if (!m_PS->CreateShader("..\\..\\Source\\Experimental\\Private\\RenderMesh.hlsl", "PSMain"))
	{
		assert(0);
	}
	CreateConstantBufferCPUWrite(device, sizeof(PerMeshCBuffer), m_cbPerMesh, "cbPerframe");
	mFrameTime.SetTime(0, 0, 0, 1, 0, Scene->GetGlobalSettings().GetTimeMode());
	for (auto& pStaticMesh : MeshArrays)
	{
		pStaticMesh->AllocResource(device, dc);
	}
	
}

void MeshModel::Tick()
{
	if (mStop > mStart )
	{
		mCurrentTime += mFrameTime;
		if (mCurrentTime > mStop)
		{
			mCurrentTime = mStart;
		}
	}
}



void MeshModel::DrawNodeRecursive(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode);

	if (pNode->GetNodeAttribute())
	{
		// Geometry offset.
		// it is not inherited by the children.
		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
		FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

		DrawNode(pNode, pParentGlobalPosition, lGlobalOffPosition);
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		DrawNodeRecursive(pNode->GetChild(lChildIndex), lGlobalPosition);
	}
}

// Draw the node following the content of it's node attribute.
void MeshModel::DrawNode(FbxNode* pNode,
	FbxAMatrix& pParentGlobalPosition,
	FbxAMatrix& pGlobalPosition)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		// All lights has been processed before the whole scene because they influence every geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
		{
			//DrawMarker(pGlobalPosition);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			//DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
		}
		// NURBS and patch have been converted into triangluation meshes.
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			DrawMesh(pNode, pGlobalPosition);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
		{
			//DrawCamera(pNode, pTime, pAnimLayer, pGlobalPosition);
		}
		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
		{
			//DrawNull(pGlobalPosition);
		}
	}
	else
	{
		// Draw a Null for nodes without attribute.
		//DrawNull(pGlobalPosition);
	}
}
void MeshModel::ComputeShapeDeformation(FbxMesh* pMesh, FbxVector4* pVertexArray)
{
	int lVertexCount = pMesh->GetControlPointsCount();

	FbxVector4* lSrcVertexArray = pVertexArray;
	FbxVector4* lDstVertexArray = new FbxVector4[lVertexCount];
	memcpy(lDstVertexArray, pVertexArray, lVertexCount * sizeof(FbxVector4));

	int lBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)pMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
			if (lChannel)
			{
				// Get the percentage of influence on this channel.
				FbxAnimCurve* lFCurve = pMesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, mCurrentAnimLayer);
				if (!lFCurve) continue;
				double lWeight = lFCurve->Evaluate(mCurrentTime);

				/*
				If there is only one targetShape on this channel, the influence is easy to calculate:
				influence = (targetShape - baseGeometry) * weight * 0.01
				dstGeometry = baseGeometry + influence

				But if there are more than one targetShapes on this channel, this is an in-between
				blendshape, also called progressive morph. The calculation of influence is different.

				For example, given two in-between targets, the full weight percentage of first target
				is 50, and the full weight percentage of the second target is 100.
				When the weight percentage reach 50, the base geometry is already be fully morphed
				to the first target shape. When the weight go over 50, it begin to morph from the
				first target shape to the second target shape.

				To calculate influence when the weight percentage is 25:
				1. 25 falls in the scope of 0 and 50, the morphing is from base geometry to the first target.
				2. And since 25 is already half way between 0 and 50, so the real weight percentage change to
				the first target is 50.
				influence = (firstTargetShape - baseGeometry) * (25-0)/(50-0) * 100
				dstGeometry = baseGeometry + influence

				To calculate influence when the weight percentage is 75:
				1. 75 falls in the scope of 50 and 100, the morphing is from the first target to the second.
				2. And since 75 is already half way between 50 and 100, so the real weight percentage change
				to the second target is 50.
				influence = (secondTargetShape - firstTargetShape) * (75-50)/(100-50) * 100
				dstGeometry = firstTargetShape + influence
				*/

				// Find the two shape indices for influence calculation according to the weight.
				// Consider index of base geometry as -1.

				int lShapeCount = lChannel->GetTargetShapeCount();
				double* lFullWeights = lChannel->GetTargetShapeFullWeights();

				// Find out which scope the lWeight falls in.
				int lStartIndex = -1;
				int lEndIndex = -1;
				for (int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex)
				{
					if (lWeight > 0 && lWeight <= lFullWeights[0])
					{
						lEndIndex = 0;
						break;
					}
					if (lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex + 1])
					{
						lStartIndex = lShapeIndex;
						lEndIndex = lShapeIndex + 1;
						break;
					}
				}

				FbxShape* lStartShape = NULL;
				FbxShape* lEndShape = NULL;
				if (lStartIndex > -1)
				{
					lStartShape = lChannel->GetTargetShape(lStartIndex);
				}
				if (lEndIndex > -1)
				{
					lEndShape = lChannel->GetTargetShape(lEndIndex);
				}

				//The weight percentage falls between base geometry and the first target shape.
				if (lStartIndex == -1 && lEndShape)
				{
					double lEndWeight = lFullWeights[0];
					// Calculate the real weight.
					lWeight = (lWeight / lEndWeight) * 100;
					// Initialize the lDstVertexArray with vertex of base geometry.
					memcpy(lDstVertexArray, lSrcVertexArray, lVertexCount * sizeof(FbxVector4));
					for (int j = 0; j < lVertexCount; j++)
					{
						// Add the influence of the shape vertex to the mesh vertex.
						FbxVector4 lInfluence = (lEndShape->GetControlPoints()[j] - lSrcVertexArray[j]) * lWeight * 0.01;
						lDstVertexArray[j] += lInfluence;
					}
				}
				//The weight percentage falls between two target shapes.
				else if (lStartShape && lEndShape)
				{
					double lStartWeight = lFullWeights[lStartIndex];
					double lEndWeight = lFullWeights[lEndIndex];
					// Calculate the real weight.
					lWeight = ((lWeight - lStartWeight) / (lEndWeight - lStartWeight)) * 100;
					// Initialize the lDstVertexArray with vertex of the previous target shape geometry.
					memcpy(lDstVertexArray, lStartShape->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
					for (int j = 0; j < lVertexCount; j++)
					{
						// Add the influence of the shape vertex to the previous shape vertex.
						FbxVector4 lInfluence = (lEndShape->GetControlPoints()[j] - lStartShape->GetControlPoints()[j]) * lWeight * 0.01;
						lDstVertexArray[j] += lInfluence;
					}
				}
			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	memcpy(pVertexArray, lDstVertexArray, lVertexCount * sizeof(FbxVector4));

	delete[] lDstVertexArray;
}

// Deform the vertex array in classic linear way.
void MeshModel::ComputeLinearDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxVector4* pVertexArray)
{
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);

		int lClusterCount = lSkinDeformer->GetClusterCount();
		for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				FbxAMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == FbxCluster::eAdditive)
				{
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++)
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0)
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		}
	}

	delete[] lClusterDeformation;
	delete[] lClusterWeight;

}


//Compute the transform matrix that the cluster will transform the vertex.
void MeshModel::ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxCluster* pCluster,
	FbxAMatrix& pVertexTransformMatrix)
{
	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix MatBoneSpaceToLocalSpace;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix MatControlPointFromClusterSpaceToBoneSpace;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel());

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(MatBoneSpaceToLocalSpace);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		MatBoneSpaceToLocalSpace *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink());

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * MatBoneSpaceToLocalSpace.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		FbxAMatrix MatControlPointsFromClusterSpaceToPivotSpace;
		pCluster->GetTransformMatrix(MatControlPointsFromClusterSpaceToPivotSpace);
		FbxAMatrix matPivotToLocal = GetGeometry(pMesh->GetNode());
		FbxAMatrix MatControlPointFromClusterSpaceLocalSpace = MatControlPointsFromClusterSpaceToPivotSpace*matPivotToLocal;
		pCluster->GetTransformLinkMatrix(MatBoneSpaceToLocalSpace);
		MatControlPointFromClusterSpaceToBoneSpace = MatBoneSpaceToLocalSpace.Inverse() * MatControlPointFromClusterSpaceLocalSpace;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink());
		FbxAMatrix BoneBindPos;
		//pVertexTransformMatrix = lClusterGlobalCurrentPosition*(BoneBindPos.Inverse()*BoneBindPos)*MatControlPointFromClusterSpaceToBoneSpace;
		//pVertexTransformMatrix = lClusterGlobalCurrentPosition*MatControlPointFromClusterSpaceToBoneSpace;
		pVertexTransformMatrix = lClusterGlobalCurrentPosition*MatControlPointFromClusterSpaceToBoneSpace;
		FbxAMatrix localGloblaPosition = pGlobalPosition;
		//这一步的作用是将插值好的skin的定点从local space 转到 node space，因为在渲染的时候会乘节点的local offset
		pVertexTransformMatrix = localGloblaPosition.Inverse()*pVertexTransformMatrix;
		//pVertexTransformMatrix = pVertexTransformMatrix*pGlobalPosition.Inverse();
		//pVertexTransformMatrix = pVertexTransformMatrix*pGlobalPosition.Inverse();
	}
}


void MeshModel::ComputeSkinDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxVector4* pVertexArray)
{
	FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();

	if (lSkinningType == FbxSkin::eLinear || lSkinningType == FbxSkin::eRigid)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pVertexArray);
	}
	else if (lSkinningType == FbxSkin::eDualQuaternion)
	{
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pVertexArray);
	}
	else if (lSkinningType == FbxSkin::eBlend)
	{
		int lVertexCount = pMesh->GetControlPointsCount();

		FbxVector4* lVertexArrayLinear = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, pMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

		FbxVector4* lVertexArrayDQ = new FbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, pMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

		ComputeLinearDeformation(pGlobalPosition, pMesh, lVertexArrayLinear);
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, lVertexArrayDQ);

		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for (int lBWIndex = 0; lBWIndex < lBlendWeightsCount; ++lBWIndex)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
	}
}
void MeshModel::DrawMesh(FbxNode* pNode,
	FbxAMatrix& pGlobalPosition)
{
	auto FindResult = mapFbxNodeToStaticMesh.Find(pNode);
	if (!FindResult)
		return;
	int StaticMeshIndex = *FindResult;
	TUniquePtr<StaticMesh> &pMesh = MeshArrays[StaticMeshIndex];
	pMesh->m_VS = m_VS.Get();
	pMesh->m_PS = m_PS.Get();
	FbxAMatrix MeshOffsetInNode = GetGeometry(pNode);
	// do deformer
	FbxMesh* lMesh = pNode->GetMesh();
	const bool lHasVertexCache = lMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
		(static_cast<FbxVertexCacheDeformer*>(lMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
	const bool lHasShape = lMesh->GetShapeCount() > 0;
	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;
	const int lVertexCount = lMesh->GetControlPointsCount();

	// No vertex to draw.
	if (lVertexCount == 0)
	{
		return;
	}
	FbxVector4* lVertexArray = NULL;
	if (!pMesh || lHasDeformation)
	{
		lVertexArray = new FbxVector4[lVertexCount];
		memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
	}

	if (lHasDeformation)
	{
		// Active vertex cache deformer will overwrite any other deformer
		if (lHasVertexCache)
		{
			//ReadVertexCacheData(lMesh, pTime, lVertexArray);
			assert(0);
		}
		else
		{
			if (lHasShape)
			{
				// Deform the vertex array with the shapes.
				ComputeShapeDeformation(lMesh, lVertexArray);
			}

			//we need to get the number of clusters
			const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
			int lClusterCount = 0;
			for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
			{
				lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
			}
			if (lClusterCount)
			{
				// Deform the vertex array with the skin deformer.
				ComputeSkinDeformation(pGlobalPosition, lMesh, lVertexArray);
			}
		}

		if (pMesh)
			pMesh->UpdateVertexPosition(lMesh,lVertexArray);
	}
	// to do 
	// mutiply the node's trasformation
	FbxAMatrix GlobalTrans = GetGlobalPosition(pNode);
	pMesh->MatWorld = FbxMatrixToFMATRIX(GlobalTrans);
	pMesh->Render(m_dc,m_cbPerMesh);
}
void MeshModel::Render(TComPtr<ID3D11DeviceContext> dc)
{
	m_dc = dc;
	FbxAMatrix matIdentity;
	DrawNodeRecursive(RootNode, matIdentity);
	Bone::BoneIDType RootBoneID = MainSkeleton.RootBone;
	if (RootBoneID != Bone::InvalidBone)
	{
		Bone* pRootBone = &MainSkeleton.GetBone(RootBoneID);
		DrawSkeleton2(pRootBone, XMMatrixIdentity());
	}
	/*for (int i = 0; i < MeshArrays.size(); ++i)
	{
		MeshArrays[i]->Render(dc, m_cb);
	}*/
}

bool MeshModel::SetCurrentAnimStack(int pIndex)
{
	const int lAnimStackCount = mAnimStackNameArray.GetCount();
	if (!lAnimStackCount || pIndex >= lAnimStackCount)
	{
		return false;
	}

	// select the base layer from the animation stack
	FbxAnimStack * lCurrentAnimationStack = Scene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
	if (lCurrentAnimationStack == NULL)
	{
		// this is a problem. The anim stack should be found in the scene!
		return false;
	}

	// we assume that the first animation layer connected to the animation stack is the base layer
	// (this is the assumption made in the FBXSDK)
	mCurrentAnimLayer = lCurrentAnimationStack->GetMember<FbxAnimLayer>();
	Scene->SetCurrentAnimationStack(lCurrentAnimationStack);

	FbxTakeInfo* lCurrentTakeInfo = Scene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
	if (lCurrentTakeInfo)
	{
		mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
		mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
	}
	else
	{
		// Take the time line value
		FbxTimeSpan lTimeLineTimeSpan;
		Scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

		mStart = lTimeLineTimeSpan.GetStart();
		mStop = lTimeLineTimeSpan.GetStop();
	}

	// check for smallest start with cache start
	if (FBXSDK_TIME_INFINITE < mStart)
		mStart = FBXSDK_TIME_INFINITE;

	// check for biggest stop with cache stop
	if (FBXSDK_TIME_MINUS_INFINITE > mStop)
		mStop = FBXSDK_TIME_MINUS_INFINITE;

	// move to beginning
	mCurrentTime = mStart;
	return true;
}

FbxAMatrix MeshModel::GetGlobalPosition(FbxNode* pNode)
{
	FbxAMatrix lGlobalPosition;

		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(mCurrentTime);

	return lGlobalPosition;
}

void MeshModel::DrawSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition)
{
	FbxSkeleton* lSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();
	auto SkeletonType = lSkeleton->GetSkeletonType();
	// Only draw the skeleton if it's a limb node and if 
	// the parent also has an attribute of type skeleton.
	if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode &&
		pNode->GetParent() &&
		pNode->GetParent()->GetNodeAttribute() &&
		pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxVector4 BasePose= pParentGlobalPosition.GetT();
		FbxVector4 RefPose = pGlobalPosition.GetT();
		XMFLOAT3 fBasePose;
		fBasePose.x = BasePose[0];
		fBasePose.y = BasePose[1];
		fBasePose.z = BasePose[2];
		XMFLOAT3 fRefPose;
		fRefPose.x = RefPose[0];
		fRefPose.y = RefPose[1];
		fRefPose.z = RefPose[2];
		GCanvas->DrawLine(fBasePose, fRefPose, XMFLOAT4(1.0, 1.0, 0, 1.0));
	}
}

void MeshModel::DrawSkeleton2(Bone* pBone, const XMMATRIX& MatParent)
{
	if (!pBone)
		return;
	for (int ChildBoneID : pBone->ChildBones)
	{
		Bone & ChildBone = MainSkeleton.GetBone(ChildBoneID);
		XMFLOAT4X4 matParentBase;
		XMFLOAT4X4 matChildBase;
		XMMATRIX matParentTransform = XMMatrixMultiply(MatParent,pBone->MatParentToBone);
		XMStoreFloat4x4(&matParentBase, matParentTransform);
		XMStoreFloat4x4(&matChildBase, XMMatrixMultiply(matParentTransform,ChildBone.MatParentToBone));
		XMFLOAT3 fBasePose(matParentBase.m[3]);
		XMFLOAT3 fRefPos(matChildBase.m[3]);
		GCanvas->DrawLine(fBasePose, fRefPos, XMFLOAT4(1.0, 1.0, 1.0, 1.0));
		DrawSkeleton2(&ChildBone, matParentTransform);
	}
}

void MeshModel::ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
	FbxMesh* pMesh,
	FbxVector4* pVertexArray)
{
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	FbxDualQuaternion* lDQClusterDeformation = new FbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(FbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for (int lClusterIndex = 0; lClusterIndex < lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix);

			FbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			FbxVector4 lT = lVertexTransformMatrix.GetT();
			FbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				FbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == FbxCluster::eAdditive)
				{
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					if (lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if (lSign >= 0.0)
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++)
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0)
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		}
	}

	delete[] lDQClusterDeformation;
	delete[] lClusterWeight;
}
