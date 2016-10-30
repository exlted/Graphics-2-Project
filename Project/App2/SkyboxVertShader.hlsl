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
	float2 TexCoord : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 PositionWS   : TEXCOORD1;
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
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}