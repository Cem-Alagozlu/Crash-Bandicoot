#pragma once

struct VertexPosCol
{
	public:

	VertexPosCol(){};
	VertexPosCol(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 col):
		Position(pos), Color(col){}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct VertexPosNormCol
{
public:

	VertexPosNormCol(){};
	VertexPosNormCol(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 norm, DirectX::XMFLOAT4 col):
		Position(pos), Normal(norm), Color(col){}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
};

struct VertexPosNormTex
{
public:

	VertexPosNormTex() {};
	VertexPosNormTex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 norm, DirectX::XMFLOAT2 texCoord):
		Position(pos), Normal(norm), TexCoord(texCoord){}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};

struct QuadPosNormCol
{
public:

	QuadPosNormCol(){};
	QuadPosNormCol(VertexPosNormCol vert1, VertexPosNormCol vert2, VertexPosNormCol vert3, VertexPosNormCol vert4):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3), Vertex4(vert4){}

	VertexPosNormCol Vertex1;
	VertexPosNormCol Vertex2;
	VertexPosNormCol Vertex3;
	VertexPosNormCol Vertex4;
};

struct TrianglePosNormCol
{
public:

	TrianglePosNormCol(){};
	TrianglePosNormCol(VertexPosNormCol vert1, VertexPosNormCol vert2, VertexPosNormCol vert3):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3){}

	VertexPosNormCol Vertex1;
	VertexPosNormCol Vertex2;
	VertexPosNormCol Vertex3;
};

// UPDATE PP
struct VertexPosTex
{
	public:

	VertexPosTex(){};
	VertexPosTex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 uv):
		Position(pos), UV(uv){}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
};

struct QuadPosTex
{
public:

	QuadPosTex(){};
	QuadPosTex(VertexPosTex vert1, VertexPosTex vert2, VertexPosTex vert3, VertexPosTex vert4):
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3), Vertex4(vert4){}

	VertexPosTex Vertex1;
	VertexPosTex Vertex2;
	VertexPosTex Vertex3;
	VertexPosTex Vertex4;
};

struct SkinnedVertex
{
	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color,float blendWeight01,float blendWeight02)
		:TransformedVertex(position, normal, color)
		, OriginalVertex(position, normal, color)
		, BlendWeight01{ blendWeight01 }
		, BlendWeight02{ blendWeight02 }
	{
		
	}
	VertexPosNormCol TransformedVertex;
	VertexPosNormCol OriginalVertex;

	float BlendWeight01;
	float BlendWeight02;
};