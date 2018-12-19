Texture2D Texture : register(t0);
Texture2D TransparencyMap : register(t1);
SamplerState ss;

// Pixel Shader
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	float4 color = Texture.Sample(ss, texcoord);
	float4 alpha = TransparencyMap.Sample(ss, texcoord);
	color.b = texcoord.x;
	color.g = texcoord.y;
	/*if (alpha.r == alpha.g && alpha.g == alpha.b)
	{
		color.a = texcoord.y + 0.1F;
	}*/
	return color;
}
