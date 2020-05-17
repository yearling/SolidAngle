#include "FullScreenQuad.h"

YFullScreenQuad::YFullScreenQuad()
{

}

YFullScreenQuad::~YFullScreenQuad()
{

}

void YFullScreenQuad::Init()
{
	TArray<YStaticMeshBuildVertex> Vertexes;
	YStaticMeshBuildVertex Vertex0;
	Vertex0.Position = FVector(-1, 3, 0);
	Vertex0.UVs[0] = FVector2D(0, -1);

	YStaticMeshBuildVertex Vertex1;
	Vertex1.Position = FVector(-1, -1, 0);
	Vertex1.UVs[0] = FVector2D(0, 1);

	YStaticMeshBuildVertex Vertex2;
	Vertex2.Position = FVector(3, -1, 0);
	Vertex2.UVs[0] = FVector2D(2, 1);
	Vertexes.Add(Vertex0);
	Vertexes.Add(Vertex1);
	Vertexes.Add(Vertex2);

	VertexPoistionBuffer.Init(Vertexes, false);

	VertexPoistionBuffer.InitRHI();

	UVBuffer.Init(Vertexes, false);
	UVBuffer.InitRHI();

	IndexBuffer.SetIndices({ 0,1,2 }, EYIndexBufferStride::AutoDetect);
	
	IndexBuffer.InitRHI();
}
