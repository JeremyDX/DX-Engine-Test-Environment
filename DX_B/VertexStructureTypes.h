#pragma once

struct Vector3S
{
	signed int x, y, z;
};

struct Vector3U
{
	unsigned int x, y, z;
};

struct Vertex3F
{
	float X, Y, Z;		// vertex position
};

struct VertexPositionColor
{
	float X, Y, Z;		// vertex position
	float R, G, B;		// vertex color;
};

struct VertexPositionTexture
{
	float X, Y, Z;		// vertex position
	float U, V;		// vertex color;
};

struct VertexPositionNormal
{
	float X, Y, Z;		// vertex position
	float nX, nY, nZ;	// normal direction
};

struct VertexPositionNormalTexture
{
	float X, Y, Z;		// vertex position
	float NX, NY, NZ;	// normal direction
	float U, V;			// texture uv position
};

class VertexStructureTypes
{
	public:

		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::Position[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionColor[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionNormal[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionTexture[];
		static D3D11_INPUT_ELEMENT_DESC VertexStructureTypes::PositionNormalTexture[];
};