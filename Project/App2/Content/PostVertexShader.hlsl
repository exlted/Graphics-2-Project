struct AppData
{
	float3 pos : POSITION;
	float2 uv : UV;
};

struct ToPixel
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

ToPixel main( AppData IN)
{
	ToPixel OUT;
	OUT.pos = float4(IN.pos, 1);
	OUT.uv = IN.uv;
	return OUT;
}