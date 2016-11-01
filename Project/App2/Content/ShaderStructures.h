#pragma once
#define MAX_LIGHTS 8

namespace App2
{
	// static constant buffer used to send MVP matrices to the vertex shader.
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
			, UseSpecular(false)
		{}
		_Material(DirectX::XMFLOAT4 emissive, DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, float specularPower, bool useTexture)
		{
			Emissive = emissive;
			Ambient = ambient;
			Diffuse = diffuse;
			Specular = specular;
			SpecularPower = specularPower;
			UseTexture = useTexture;
		}

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

		int                 UseSpecular;
		// Add some padding to complete the 16 byte boundary.
		float                 Padding;
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


		static const _Material emerald = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.0215f, .1745f, .0215f, 1.0f),
			DirectX::XMFLOAT4(.07568f, .61424f, .07568f, 1.0f),
			DirectX::XMFLOAT4(.633f, .727811f, .633f, 1.0f),
			.6f, false);
		static const _Material jade = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4( .135f, .2225f, .1575f, 1.0f),
			DirectX::XMFLOAT4( .54f, .89f, .63f, 1.0f),
			DirectX::XMFLOAT4( .316228f, .316228f, .316228f, 1.0f),
			.1f , false);
		static const _Material obsidian = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.05375f, .05f, .06625f, 1.0f),
			DirectX::XMFLOAT4(.18275f, .17f, .22525f, 1.0f),
			DirectX::XMFLOAT4(.332741f, .328634f, .346435f, 1.0f),
			.3f, false);
		static const _Material pearl = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.25f, .20725f, .20725f, 1.0f),
			DirectX::XMFLOAT4(1.0f, .829f, .63f, 1.0f),
			DirectX::XMFLOAT4(.296648f, .296648f, .296648f, 1.0f),
			.088f, false);
		static const _Material ruby = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.1745f, .01175f, .01175f, 1.0f),
			DirectX::XMFLOAT4(.61424f, .04136f, .04136f, 1.0f),
			DirectX::XMFLOAT4(.727811f, .626959f, .626959f, 1.0f),
			.6f, false);
		static const _Material turqoise = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.1f, .18725f, .1745f, 1.0f),
			DirectX::XMFLOAT4(.396f, .74151f, .69102f, 1.0f),
			DirectX::XMFLOAT4(.297254f, .30829f, .306678f, 1.0f),
			.1f, false);
		static const _Material brass = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.329412f, .223529f, .027451f, 1.0f),
			DirectX::XMFLOAT4(.780392f, .568627f, .113725f, 1.0f),
			DirectX::XMFLOAT4(.992157f, .941176f, .807843f, 1.0f),
			.21794872f, false);
		static const _Material bronze = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.2125f, .1275f, .054f, 1.0f),
			DirectX::XMFLOAT4(.714f, .4284f, .18144f, 1.0f),
			DirectX::XMFLOAT4(.393548f, .271906f, .166721f, 1.0f),
			.2f, false);
		static const _Material chrome = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT4(.25f, .25f, .25f, 1.0f),
			DirectX::XMFLOAT4(.4f, .4f, .4f, 1.0f),
			DirectX::XMFLOAT4(.774597f, .774597f, .774597f, 1.0f),
			.6f, false);
		static const _Material copper = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.19125f, .735f, .225f, 1.0f),
			DirectX::XMFLOAT4(.7038f, .27048f, .0828f, 1.0f),
			DirectX::XMFLOAT4(.256777f, .137622f, .086014f, 1.0f),
			.1f, false);
		static const _Material gold = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.24725f, .1995f, .0745f, 1.0f),
			DirectX::XMFLOAT4(.75164f, .60648f, .22648f, 1.0f),
			DirectX::XMFLOAT4(.628281f, .55802f, .366065f, 1.0f),
			.4f, false);
		static const _Material silver = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.19225f, .19225f, .19225f, 1.0f),
			DirectX::XMFLOAT4(.50754f, .50754f, .50754f, 1.0f),
			DirectX::XMFLOAT4(.50754f, .508273f, .508273f, 1.0f),
			.4f, false);
		static const _Material blackPlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f),
			DirectX::XMFLOAT4(.5f, .5f, .5f, 1.0f),
			.25f, false);
		static const _Material cyanPlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.1f, 0.06f, 1.0f),
			DirectX::XMFLOAT4(0.0f, .50980392f, .50980392f, 1.0f),
			DirectX::XMFLOAT4(.5096078f, .50196078f, .50196078f, 1.0f),
			.25f, false);
		static const _Material greenPlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.1f, 0.35f, 0.1f, 1.0f),
			DirectX::XMFLOAT4(0.45f, 0.55f, 0.45f, 1.0f),
			.25f, false);
		static const _Material redPlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.7f, 0.6f, 0.6f, 1.0f),
			0.25f, false);
		static const _Material whitePlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f),
			DirectX::XMFLOAT4(0.70f, 0.70f, 0.70f, 1.0f),
			0.25f, false);
		static const _Material yellowPlastic = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.6f, 0.6f, 0.5f, 1.0f),
			0.25f, false);
		static const _Material blackRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f),
			DirectX::XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f),
			DirectX::XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f),
			.78125f, false);
		static const _Material cyanRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.05f, 0.05f, 1.0f),
			DirectX::XMFLOAT4(0.4f, 0.5f, 0.5f, 1.0f),
			DirectX::XMFLOAT4(0.04f, 0.7f, 0.7f, 1.0f),
			.78125f, false);
		static const _Material greenRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(0.0f, 0.05f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.4f, 0.5f, 0.4f, 1.0f),
			DirectX::XMFLOAT4(0.04f, 0.7f, 0.04f, 1.0f),
			.78125f, false);
		static const _Material redRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.05f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(0.5f, 0.4f, 0.4f, 1.0f),
			DirectX::XMFLOAT4(0.7f, .04f, .04f, 1.0f),
			.78125f, false);
		static const _Material whiteRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.05f, .05f, .05f, 1.0f),
			DirectX::XMFLOAT4(.5f, .5f, .5f, 1.0f),
			DirectX::XMFLOAT4(.7f, .7f, .7f, 1.0f),
			.78125f, false);
		static const _Material yellowRubber = _Material(
			DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0),
			DirectX::XMFLOAT4(.05f, .05f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(.5f, .5f, .4f, 1.0f),
			DirectX::XMFLOAT4(.7f, .7f, .04f, 1.0f),
			.78125f, false);
}