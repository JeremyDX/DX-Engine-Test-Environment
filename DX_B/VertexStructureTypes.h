#pragma once

struct Vector3S
{
	signed int x, y, z;
};

struct Vector3U
{
	unsigned int x, y, z;
};

struct Vertex2F
{
	float U, V;			// vertex position
};

struct Vertex3F
{
	float X, Y, Z;		// vertex position
};

struct Vertex4F
{
	float R, G, B, A;	// vertex position
};

struct Vertex12Byte
{
	float _X, _Y, _Z;
};

struct Vertex20Byte
{
	float _X, _Y, _Z;
	float _U, _V;
};

struct Vertex24Byte
{
	float _X, _Y, _Z;
	float _1, _2, _3;
};

struct Vertex32Byte
{
	float _X, _Y, _Z;
	float _1, _2, _3;
	float _U, _V;
};

struct Vertex44Byte
{
	float _X, _Y, _Z;
	float _1, _2, _3;
	float _4, _5, _6;
	float _U, _V;
};

struct VertexPositionColorx
{
	float X, Y, Z;		// vertex position
	float R, G, B;		// vertex color;
};

struct VertexPositionTexturex
{
	float X, Y, Z;		// vertex position
	float U, V;		// vertex color;
};

struct VertexPositionNormalx
{
	float X, Y, Z;		// vertex position
	float nX, nY, nZ;	// normal direction
};

struct VertexPositionNormalTexturex
{
	float X, Y, Z;		// vertex position
	float NX, NY, NZ;	// normal direction
	float U, V;			// texture uv position
};

struct VertexPositionColorTexture
{
	float X, Y, Z;		// vertex position
	float R, G, B;	// normal direction
	float U, V;			// texture uv position
};


class VertexStructureTypes
{
	public:

		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::Position[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionColor[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionNormal[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionTexture[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionColorTexture[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionNormalxTexture[];
};