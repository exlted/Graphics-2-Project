#include "pch.h"
#include "Skybox.h"

#include "..\Common\DirectXHelper.h"
App2::Skybox::Skybox(const std::shared_ptr<DX::DeviceResources>& deviceResources, XMFLOAT4X4 * Camera, XMFLOAT4X4 * Projection, lightProperties * lights)
{
	this->camera = Camera;
	this->light = lights;
	this->m_deviceResources = deviceResources;
	this->proj = Projection;
	m_degreesPerSecond = 45;
	CreateDeviceDependentResources();
}

void App2::Skybox::CreateDeviceDependentResources()
{
	auto loadVSTask = DX::ReadDataAsync(L"SkyboxVertShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SkyboxPixelShader.cso");

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
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

		//Load mesh vertices. Each vertex has a position and a color.
		static const VertexTex cubeVertices[] =
		{//
			{ XMFLOAT3(-0.99f, -1.0f, -1.0f),  XMFLOAT2(0.33333333f, 1.0f) }, //x- WEST GOOD
			{ XMFLOAT3(-0.99f, -1.0f,  1.0f),  XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-0.99f,  1.0f, -1.0f),  XMFLOAT2(0.33333333f, 0.5f) },
			{ XMFLOAT3(-0.99f,  1.0f,  1.0f),  XMFLOAT2(0.0f, 0.5f) },

			{ XMFLOAT3(0.99f, -1.0f, -1.0f),   XMFLOAT2(0.66666666f, 1.0f) }, //x+ EAST BAD
			{ XMFLOAT3(0.99f, -1.0f,  1.0f),   XMFLOAT2(1.0f, 1.0f) }, //BAD
			{ XMFLOAT3(0.99f,  1.0f, -1.0f),   XMFLOAT2(0.66666666f, 0.5f) },
			{ XMFLOAT3(0.99f,  1.0f,  1.0f),   XMFLOAT2(1.0f, 0.5f) }, //BAD

			{ XMFLOAT3(-1.0f, -0.99f, -1.0f),  XMFLOAT2(0.0f, 0.0f) }, //y- BOTTOM GOOD
			{ XMFLOAT3(-1.0f, -0.99f,  1.0f),  XMFLOAT2(0.0f, 0.5f) },
			{ XMFLOAT3(1.0f, -0.99f, -1.0f),   XMFLOAT2(0.3333333333f, 0.0f) },
			{ XMFLOAT3(1.0f, -0.99f,  1.0f),   XMFLOAT2(0.3333333333f, 0.5f) }, //BAD

			{ XMFLOAT3(-1.0f,  0.99f, -1.0f),  XMFLOAT2(0.33333333f, 0.5f) }, //y+ TOP
			{ XMFLOAT3(-1.0f,  0.99f,  1.0f),  XMFLOAT2(0.33333333f, 0.0f) },
			{ XMFLOAT3(1.0f,  0.99f, -1.0f),   XMFLOAT2(0.66666666f, 0.5f) },
			{ XMFLOAT3(1.0f,  0.99f,  1.0f),   XMFLOAT2(0.66666666f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -0.99f),  XMFLOAT2(0.33333333f, 1.0f) }, //z- NORTH GOOD
			{ XMFLOAT3(-1.0f,  1.0f, -0.99f),  XMFLOAT2(0.33333333f, 0.5f) },
			{ XMFLOAT3(1.0f, -1.0f, -0.99f),   XMFLOAT2(0.66666666f, 1.0f) },
			{ XMFLOAT3(1.0f,  1.0f, -0.99f),   XMFLOAT2(0.66666666f, 0.5f) },

			{ XMFLOAT3(-1.0f, -1.0f,  0.99f),  XMFLOAT2(1.0f, 0.5f) }, //z+ SOUTH BAD
			{ XMFLOAT3(-1.0f,  1.0f,  0.99f),  XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f,  0.99f),   XMFLOAT2(0.66666666f, 0.5f) }, //BAD
			{ XMFLOAT3(1.0f,  1.0f,  0.99f),  XMFLOAT2( 0.66666666f, 0.0f) }  //BAD
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
		auto device = m_deviceResources->GetD3DDevice();

		HRESULT hr = CreateDDSTextureFromFile(device, L"Assets\\night_skybox.dds", nullptr, &m_Texture, 0);

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		hr = device->CreateSamplerState(&samplerDesc, &m_sampler);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			1,2,0, // -x w/N
			3,2,1,

			6,5,4, // +x w/N
			6,7,5,

			11,9,8, // -y w/N
			10,11,8,

			15,14,12, //+y w/N
			13,15,12,

			19,18,16, //-z w/N
			17,19,16,

			23,21,20, //+z w/N
			22,23,20
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

void App2::Skybox::ReleaseDeviceDependentResources()
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
	m_Texture.Reset();
	m_sampler.Reset();
}
void App2::Skybox::Update(DX::StepTimer const & timer)
{
	auto cam = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(camera));
	DirectX::XMStoreFloat4x4(&m_constantBufferData.View, cam);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.Projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(proj)));
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, XMLoadFloat4x4(camera).r[3]);
	DirectX::XMStoreFloat4x4(&m_constantBufferData.WorldMatrix, XMMatrixTranslationFromVector(XMLoadFloat4(&temp)));
	DirectX::XMStoreFloat4x4(&m_constantBufferData.InverseTransposeWorldMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_constantBufferData.WorldMatrix)))));;
}

void App2::Skybox::Render()
{
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);
	unsigned int stride = sizeof(VertexTex);
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
	context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
	context->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());
	//
	context->DrawIndexed(m_indexCount, 0, 0);\
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}