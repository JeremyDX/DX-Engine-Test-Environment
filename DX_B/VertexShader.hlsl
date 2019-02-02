cbuffer HLSLBuffer
{
	float4x4 view_matrix;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float2 texcoord: TEXCOORD;        // texture coordinates
};

//target vs_4_0_level_9_3

VOut main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.position = mul(view_matrix, position);
	int base1 = (int)(color.r / 256); //0-255
	int base2 = ((int)color.r) - (base1 * 256);
	output.color.r = (base1 / 255.0f);
	output.color.g = color.g;
	output.color.b = color.b;
	output.color.a = (base2 / 255.0f);

	output.texcoord = texcoord;    // set the texture coordinates, unmodified
	return output;
}