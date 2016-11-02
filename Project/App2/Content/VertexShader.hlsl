cbuffer perFrame : register(b0)
{
	matrix ViewMatrix;
	matrix ProjectionMatrix;
	matrix Matrix;
	matrix InverseTranspose;
}

struct AppData
{
	//Per-vertex data
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float3 TexCoord : TEXCOORD;
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
	matrix ViewProjectionMatrix = mul(ViewMatrix, ProjectionMatrix);
	matrix MVP = mul(ViewProjectionMatrix, Matrix);

	OUT.Position = mul(Matrix, float4(IN.Position, 1.0f));
	OUT.Position = mul(ViewMatrix, OUT.Position);
	OUT.Position = mul(ProjectionMatrix, OUT.Position);

	OUT.PositionWS = mul(Matrix, float4(IN.Position, 1.0f));
	OUT.NormalWS = mul((float3x3)InverseTranspose, IN.Normal);
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}