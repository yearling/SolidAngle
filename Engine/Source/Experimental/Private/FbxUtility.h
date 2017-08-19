#pragma once
#include "YYUT.h"
#include <fbxsdk.h>
inline XMMATRIX FbxMatrixToXMMATRIX(const FbxAMatrix & InMatrix)
{
	XMFLOAT4X4 OutMatrix;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			OutMatrix.m[i][j] = (float)InMatrix[i][j];
		}
	}
	return XMLoadFloat4x4(&OutMatrix);
}

inline FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}
