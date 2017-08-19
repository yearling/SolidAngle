#pragma once

#include "YYUT.h"

struct Frustum
{
	XMFLOAT3 Origin;				//原点
	XMFLOAT4 Orientation;			//朝向
	float RightSlope;				//右平面角度，就是FOV的一半，不过是正的
	float LeftSlope;				//左平面的角度，FOV的一半，正的
	float TopSlope;					//顶平面的角度，
	float BottomSlope;				//低平面的角度
	float Near, Far;					//近平面，远平面
};
//用来从透视投影矩阵算视锥，矩阵必须为标准的对称透视投影矩阵
void ComputeFrustumFromProjection(Frustum *pOut, CXMMATRIX projection);