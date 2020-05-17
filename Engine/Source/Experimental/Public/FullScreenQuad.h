#pragma once
#include "Core.h"
#include "YRenderResource.h"
#include "YStaticMeshResource.h"
#include "IShader.h"
class YFullScreenQuad
{
public:
	YFullScreenQuad();
	~YFullScreenQuad();
	void Init();
	YRawStaticIndexBuffer IndexBuffer;
	YPositionVertexBuffer VertexPoistionBuffer;
	YStaticMeshTangentUVVertexBuffer UVBuffer;
};
