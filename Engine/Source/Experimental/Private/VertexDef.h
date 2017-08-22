#pragma once
#include "YYUT.h"
struct LocalVertex
{
	FVector Position;
	FVector Normal;
	FVector2D UV0;
	FLinearColor Color;
};

struct PerMeshCBuffer
{
	XMMATRIX m_matWrold;
};

struct PerFMeshCBuffer
{
	FMatrix m_matWrold;
};