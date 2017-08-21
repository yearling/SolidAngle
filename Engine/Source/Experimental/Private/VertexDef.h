#pragma once
#include "YYUT.h"
struct LocalVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV0;
	XMFLOAT4 Color;
};

struct PerMeshCBuffer
{
	XMMATRIX m_matWrold;
};

struct PerFMeshCBuffer
{
	FMatrix m_matWrold;
};