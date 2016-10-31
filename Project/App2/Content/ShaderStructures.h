#pragma once
#define MAX_LIGHTS 8

namespace App2
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct VertexTex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	struct VertexNormalColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT3 color;
	};

	struct WholeInstanceData
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Projection;
	};

	struct ModelInstanceData
	{
		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT4X4 InverseTransposeWorldMatrix;
	};

	struct SingleModelData
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Projection;
		DirectX::XMFLOAT4X4 WorldMatrix;
		DirectX::XMFLOAT4X4 InverseTransposeWorldMatrix;
	};

	struct _Material
	{
		_Material()
			: Emissive(0.0f, 0.0f, 0.0f, 1.0f)
			, Ambient(0.1f, 0.1f, 0.1f, 1.0f)
			, Diffuse(1.0f, 1.0f, 1.0f, 1.0f)
			, Specular(1.0f, 1.0f, 1.0f, 1.0f)
			, SpecularPower(128.0f)
			, UseTexture(false)
		{}

		DirectX::XMFLOAT4   Emissive;
		//----------------------------------- (16 byte boundary)
		DirectX::XMFLOAT4   Ambient;
		//----------------------------------- (16 byte boundary)
		DirectX::XMFLOAT4   Diffuse;
		//----------------------------------- (16 byte boundary)
		DirectX::XMFLOAT4   Specular;
		//----------------------------------- (16 byte boundary)
		float               SpecularPower;
		// Add some padding complete the 16 byte boundary.
		int                 UseTexture;
		// Add some padding to complete the 16 byte boundary.
		float                 Padding[2];
		//----------------------------------- (16 byte boundary)
	}; // Total:                                80 bytes (5 * 16)

	struct MaterialProperties
	{
		_Material   Material;
	};

	enum LightType
	{
		DirectionalLight = 0, PointLight = 1, SpotLight = 2
	};

	struct Light
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT4 Direction;
		DirectX::XMFLOAT4 Color;
		float SpotAngle;
		float ConstantAttenuation;
		float LinearAttenuation;
		float QuadraticAttenuation;
		LightType lightType;
		bool Enabled;
		int Padding;
		int Padding2;
	};

	struct lightProperties
	{
		DirectX::XMFLOAT4 EyePosition;
		DirectX::XMFLOAT4 GlobalAmbient;
		Light Lights[MAX_LIGHTS];
	};
}