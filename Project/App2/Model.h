#pragma once
#include "ModelLoading.h"
#include "..\Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <DirectXMath.h>

using namespace App2;

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SpecuMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_sampler;

public:
	Model(char modelName[], DirectX::XMFLOAT4X4* camera, DirectX::XMFLOAT4X4* projection, std::shared_ptr<DX::DeviceResources> m_deviceResources, lightProperties * Lights);

	void Update(DX::StepTimer const& timer);
	bool Render();
	void CreateDeviceDependentResources();
	void ReleaseDeviceDependentResources();
};

