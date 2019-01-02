Texture2D Texture;
SamplerState ss;

// Pixel Shader
float4 main(float4 position : SV_POSITION, float4 normal : COLOR0, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return normal * Texture.Sample(ss, texcoord);
}