#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <DirectXMath.h>
using namespace DirectX;

namespace App2
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Skybox
	{
	public:
		Skybox(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT4X4 *Camera, XMFLOAT4X4 *Projection, lightProperties *lights);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_lightingBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>        m_materialBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>  m_sampler;

		// System resources for cube geometry.
		SingleModelData	m_constantBufferData;
		MaterialProperties mat;
		lightProperties *light;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		XMFLOAT4X4 world, *camera, *proj;
	};
}

