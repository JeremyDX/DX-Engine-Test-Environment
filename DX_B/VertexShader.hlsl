cbuffer HLSLBuffer
{
	float4x4 view_matrix;
};

struct VOut
{
	float4 position : SV_POSITION;		//Vertex Position.
	float2 texcoord: TEXCOORD;			//Texture coordinates
};

VOut main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.position = mul(view_matrix, position);
	output.texcoord = texcoord;    // set the texture coordinates, unmodified

	return output;
}