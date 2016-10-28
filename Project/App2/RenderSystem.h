#pragma once
#include "Model.h"

#include <DirectXMath.h>
using namespace DirectX;
struct PerFrameConstantBufferData
{
	DirectX::XMMATRIX ViewProjectionMatrix;
};

class RenderSystem
{
	Model *Models;
	PerFrameConstantBufferData data;
	std::shared_ptr<DX::DeviceResources> m_deviceResources;
	DirectX::XMFLOAT4X4 Camera, Projection;
	lightProperties m_LightProperties;
	float totalTime;
public:
	RenderSystem(std::shared_ptr<DX::DeviceResources> m_deviceResources);
	~RenderSystem();
	void Update(DX::StepTimer const& timer);
	bool Render();
	void CreateWindowSizeDependentResources();
};

