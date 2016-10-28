#pragma once
#include "ModelLoading.h"
#include "..\Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <DirectXMath.h>

#define MAX_LIGHTS 8


__declspec(align(16))  struct PlaneInstanceData
{
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX InverseTransposeWorldMatrix;
};

struct SingleModelData
{
	DirectX::XMMATRIX ViewProjection;
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX InverseTransposeWorldMatrix;
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

class Model
{
	char * modelName;
	VertexPTN *Mesh;
	SingleModelData data;
	lightProperties * Lights;
	MaterialProperties Props;
	DirectX::XMFLOAT4X4 * Camera;
	DirectX::XMFLOAT4X4 * Projection;
	bool m_loadingComplete = false;
	unsigned int indexCount;



	std::shared_ptr<DX::DeviceResources> m_deviceResources;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pixelShaderLightConstBuff;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pixelShaderMatConstBuff;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_sampler;

public:
	Model(char modelName[], DirectX::XMFLOAT4X4* camera, DirectX::XMFLOAT4X4* projection, std::shared_ptr<DX::DeviceResources> m_deviceResources, lightProperties * Lights);

	void Update(DX::StepTimer const& timer);
	bool Render();
	void CreateDeviceDependentResources();
	void ReleaseDeviceDependentResources();
};

