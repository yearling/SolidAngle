#include "YYUT.h"
#include "YYUTFrustum.h"



void ComputeFrustumFromProjection(Frustum *pOut, CXMMATRIX projection)
{
	assert(pOut);
	//cvv的六个点
	static XMVECTOR CVVPoints[6] =
	{
		{   1.0f,	0.0f,	1.0f,	1.0f}, //far right
		{  -1.0f,	0.0f,	1.0f,	1.0f}, //far left
		{   0.0f,	1.0f,	1.0f,	1.0f}, //far Top
		{   0.0f,  -1.0f,	1.0f,	1.0f}, //far Bottom
		{   0.0f,	0.0f,   0.0f,   1.0f}, //near
		{   0.0f,   0.0f,   1.0f,   1.0f}, //far
	};
	XMVECTOR Determinant;
	XMMATRIX matInverse = XMMatrixInverse(&Determinant, projection); //投影矩阵的逆
	XMVECTOR Points[6];

	for (INT i = 0; i < 6; i++)
	{
		// 把CVV的6个点变换到相机空间里
		Points[i] = XMVector4Transform(CVVPoints[i], matInverse);
	}
	pOut->Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pOut->Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0])); //Points[0].x /= Points[0].z; Points[0].y /= Points[0].z 。。。
	Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
	Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
	Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

	pOut->RightSlope = XMVectorGetX(Points[0]);
	pOut->LeftSlope = XMVectorGetX(Points[1]);
	pOut->TopSlope = XMVectorGetY(Points[2]);
	pOut->BottomSlope = XMVectorGetY(Points[3]);

	Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
	Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

	pOut->Near = XMVectorGetZ(Points[4]);
	pOut->Far = XMVectorGetZ(Points[5]);
	return;
}
