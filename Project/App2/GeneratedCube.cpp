#include "pch.h"
#include "GeneratedCube.h"

#include "..\Common\DirectXHelper.h"
App2::GeneratedCube::GeneratedCube(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT4X4 * Camera, XMFLOAT4X4 * Projection, lightProperties * lights)
{
	this->camera = Camera;
	this->light = lights;
	this->m_deviceResources = deviceResources;
	this->proj = Projection;
	m_degreesPerSecond = 45;
}

void App2::GeneratedCube::CreateDeviceDependentResources()
{
	auto loadVSTask = DX::ReadDataAsync(L"ColoredVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"ColoredLitPixelShader.cso");

	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(SingleModelData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);

		CD3D11_BUFFER_DESC lightBufferDesc(sizeof(lightProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_lightingBuffer
			)
		);

		CD3D11_BUFFER_DESC matBufferDesc(sizeof(MaterialProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_materialBuffer
			)
		);
	});

	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexNormalColor cubeVertices[] =
		{
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)},

			{ XMFLOAT3(0.5f, -0.5f, -0.5f),  XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(0.5f, -0.5f,  0.5f),  XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(0.5f,  0.5f, -0.5f),  XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{ XMFLOAT3(0.5f,  0.5f,  0.5f),  XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},

			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(0.5f, -0.5f, -0.5f),  XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(0.5f, -0.5f,  0.5f),  XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},

			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{ XMFLOAT3(0.5f,  0.5f, -0.5f),  XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{ XMFLOAT3(0.5f,  0.5f,  0.5f),  XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},

			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{ XMFLOAT3(0.5f, -0.5f, -0.5f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(0.5f,  0.5f, -0.5f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},

			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{ XMFLOAT3(0.5f, -0.5f,  0.5f),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{ XMFLOAT3(0.5f,  0.5f,  0.5f),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},

		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			0,2,1, // -x w/N
			1,2,3,

			4,5,6, // +x w/N
			5,7,6,

			8,9,11, // -y w/N
			8,11,10,

			12,14,15, //+y w/N
			12,15,13,

			16,18,19, //-z w/N
			16,19,17,

			20,21,23, //+z w/N
			20,23,22
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
	});

}

void App2::GeneratedCube::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_inputLayout.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_lightingBuffer.Reset();
	m_materialBuffer.Reset();
}

void App2::GeneratedCube::Update(DX::StepTimer const & timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}
	auto cam = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(camera));
	DirectX::XMStoreFloat4x4(&m_constantBufferData.View, cam);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.Projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(proj)));
	DirectX::XMStoreFloat4x4(&m_constantBufferData.InverseTransposeWorldMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_constantBufferData.WorldMatrix)))));;
}

void App2::GeneratedCube::Render()
{
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);
	context->UpdateSubresource(m_lightingBuffer.Get(), 0, nullptr, light, 0, 0);
	context->UpdateSubresource(m_materialBuffer.Get(), 0, nullptr, &mat, 0, 0);
	unsigned int stride = sizeof(VertexNormalColor);
	unsigned int offset = 0;
	ID3D11Buffer* buffers = m_vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &buffers, &stride, &offset);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	ID3D11Buffer* pixelShaderConstantBuffers[2] = { m_materialBuffer.Get(), m_lightingBuffer.Get() };
	context->PSSetConstantBuffers(0, 2, pixelShaderConstantBuffers);
	//context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
	//context->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());
	//
	context->DrawIndexed(m_indexCount, 0, 0);

	return;
}

void App2::GeneratedCube::StartTracking()
{
	m_tracking = true;
}

void App2::GeneratedCube::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void App2::GeneratedCube::StopTracking()
{
	m_tracking = false;
}

void App2::GeneratedCube::Rotate(float radians)
{
	XMStoreFloat4x4(&m_constantBufferData.WorldMatrix, XMMatrixTranspose(XMMatrixRotationY(radians)));
}
