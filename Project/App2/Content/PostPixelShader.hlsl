texture2D screen : register(t0);
sampler Sampler : register(s0);

struct ToPixel
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

float4 main( ToPixel IN ) : SV_TARGET
{
	float4 color = screen.Sample(Sampler, IN.uv);

	float gray = dot(color.rgb, float3(0.299, 0.587, 0.114));

	return gray;
}