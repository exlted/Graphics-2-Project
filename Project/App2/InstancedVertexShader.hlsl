cbuffer perFrame : register(b0)
{
	matrix View;
	matrix Projection;
}

struct AppData
{
	//Per-vertex data
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float3 TexCoord : TEXCOORD;
	//Per-instance data
	matrix Matrix   : WORLDMATRIX;
	matrix InverseTranspose : INVERSETRANSPOSEWORLDMATRIX;
};

struct VertexShaderOutput
{
	float4 PositionWS   : TEXCOORD1;
	float3 NormalWS     : TEXCOORD2;
	float2 TexCoord     : TEXCOORD0;
	float4 Position     : SV_Position;
};

VertexShaderOutput main(AppData IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(IN.Matrix, float4(IN.Position, 1.0f));
	OUT.Position = mul(View, OUT.Position);
	OUT.Position = mul(Projection, OUT.Position);

	OUT.PositionWS = mul(IN.Matrix, float4(IN.Position, 1.0f));
	OUT.NormalWS = mul((float3x3)IN.InverseTranspose, IN.Normal);
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}