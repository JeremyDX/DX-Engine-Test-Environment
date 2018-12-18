float4 main(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	float4 newcolor = color;
	newcolor.a = 0.1f;
	return newcolor;
}