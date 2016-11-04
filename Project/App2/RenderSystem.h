#pragma once
#include "Model.h"
#include "Content\InstancedModel.h"
#include <DirectXMath.h>
#include "Content\ShaderStructures.h"
#include "GeneratedCube.h"
#include "Content\Skybox.h"

using namespace DirectX;
struct PerFrameConstantBufferData
{
	DirectX::XMMATRIX ViewProjectionMatrix;
};

class RenderSystem
{
	Model *Models;
	InstancedModel *InstancedModels;
	GeneratedCube *Cube;
	Skybox *skybox;
	PerFrameConstantBufferData data;
	std::shared_ptr<DX::DeviceResources> m_deviceResources;
	DirectX::XMFLOAT4X4 Camera, Projection;
	lightProperties m_LightProperties;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;

	float totalTime;
public:
	RenderSystem(std::shared_ptr<DX::DeviceResources> m_deviceResources);
	~RenderSystem();
	void Update(DX::StepTimer const& timer);
	bool Render();
	void CreateDeviceDependantResources();
	void CreateWindowSizeDependentResources();
	void ReleaseDeviceDependantResources();
};

