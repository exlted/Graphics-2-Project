
Texture2D Texture : register(t0);
sampler Sampler : register(s0);

struct PixelShaderInput
{
	float4 PositionWS : TEXCOORD1;
	float2 TexCoord   : TEXCOORD0;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	float4 texColor = Texture.Sample(Sampler, IN.TexCoord);
	return texColor;
}