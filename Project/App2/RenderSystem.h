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

struct AppData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
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

	AppData screenOrientedQuad[4];

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1> m_RTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D1>        m_RTVBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	    m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	    m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_Texture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_sampler;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1> m_RTV2;
	Microsoft::WRL::ComPtr<ID3D11Texture2D1>        m_RTVBuffer2;

	unsigned int m_indexCount;
	bool m_loaded = false;

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

