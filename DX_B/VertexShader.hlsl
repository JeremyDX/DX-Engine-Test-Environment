cbuffer HLSLBuffer
{
	float4x4 view_matrix;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 normal : COLOR;
    float2 texcoord: TEXCOORD;        // texture coordinates
};

VOut main(float4 position : POSITION, float4 normal : COLOR, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.position = mul(view_matrix, position);
	output.texcoord = texcoord;    // set the texture coordinates, unmodified
	output.normal = normal;
	return output;
}