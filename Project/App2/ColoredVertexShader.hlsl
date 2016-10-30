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
	float3 Color : COLOR;
};

struct VertexShaderOutput
{
	float4 PositionWS   : TEXCOORD1;
	float3 NormalWS     : TEXCOORD2;
	float4 Color     : COLOR;
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

	//OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
	OUT.PositionWS = mul(Matrix, float4(IN.Position, 1.0f));
	OUT.NormalWS = mul((float3x3)InverseTranspose, IN.Normal);
	OUT.Color = float4(IN.Color, 1);

	return OUT;
}