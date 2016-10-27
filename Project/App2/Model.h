#pragma once
#include "ModelLoading.h"
#include "..\Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <DirectXMath.h>
class Model
{
	char * modelName;
	VertexPTN *Mesh;
	DirectX::XMFLOAT4X4 * Camera;
	bool m_loadingComplete = false;



	std::shared_ptr<DX::DeviceResources> m_deviceResources;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

public:
	Model(char modelName[], DirectX::XMFLOAT4X4* camera, std::shared_ptr<DX::DeviceResources> m_deviceResources);

	void Update(DX::StepTimer const& timer);
	bool Render();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void ReleaseDeviceDependentResources();
};

