#include "pch.h"
#include "Model.h"
#include "..\Common\DirectXHelper.h"

Model::Model(char modelName[], DirectX::XMFLOAT4X4 * camera, DirectX::XMFLOAT4X4* projection, std::shared_ptr<DX::DeviceResources> m_deviceResources, lightProperties * Lights)
{
	this->modelName = modelName;
	this->Camera = camera;
	this->Projection = projection;
	this->m_deviceResources = m_deviceResources;
	this->Lights = Lights;
	this->CreateDeviceDependentResources();
	Props.Material = App2::chrome;
	Props.Material.UseSpecular = true;
	Props.Material.UseTexture = true;
	Props.Material.UseNormal = true;
}

void Model::Update(DX::StepTimer const & timer)
{
	if (!m_loadingComplete)
	{
		return;
		lastUpdate = timer.GetTotalSeconds();
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	auto cam = DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(Camera));
	DirectX::XMStoreFloat4x4(&data.View, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranspose(cam)));
	DirectX::XMStoreFloat4x4(&data.Projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(Projection)));
	DirectX::XMStoreFloat4x4(&data.WorldMatrix, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
	DirectX::XMStoreFloat4x4(&data.InverseTransposeWorldMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixTranspose(DirectX::XMLoadFloat4x4(&data.WorldMatrix)))));

	if(lastUpdate + 5.0f < timer.GetTotalSeconds())
	{
		lastUpdate = timer.GetTotalSeconds();
		Props.Material.UseNormal = !Props.Material.UseNormal;
	}

	return;
}

bool Model::Render()
{
	if(!m_loadingComplete)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
	context->UpdateSubresource(m_pixelShaderLightConstBuff.Get(), 0, nullptr, Lights, 0, 0);
	context->UpdateSubresource(m_pixelShaderMatConstBuff.Get(), 0, nullptr, &Props, 0, 0);

	unsigned int stride = sizeof(VertexPTN);
	unsigned int offset = 0;
	ID3D11Buffer* buffers = m_vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &buffers, &stride, &offset);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	ID3D11Buffer* pixelShaderConstantBuffers[2] = {m_pixelShaderMatConstBuff.Get(), m_pixelShaderLightConstBuff.Get()};
	context->PSSetConstantBuffers(0, 2, pixelShaderConstantBuffers);
	context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
	ID3D11ShaderResourceView *texBuffers[3] = { m_Texture.Get() , m_SpecuMap.Get(), m_NormalMap.Get()};
	context->PSSetShaderResources(0, 3, texBuffers);

	context->DrawIndexed(indexCount, 0, 0);
	//context->Draw(indexCount, 0);

	return true;
}

void Model::CreateDeviceDependentResources()
{
	auto loadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"TexturedLitPixelShader.cso");

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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		//static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		//{
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//	{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		//	{ "INVERSETRANSPOSEWORLDMATRIX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		//};

		//D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		//{
		//	// Per-vertex data.
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	// Per-instance data.
		//	{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "INVERSETRANSPOSEWORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "INVERSETRANSPOSEWORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "INVERSETRANSPOSEWORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//	{ "INVERSETRANSPOSEWORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//};

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
				&lightBufferDesc,
				nullptr,
				&m_pixelShaderLightConstBuff
			)
		);

		CD3D11_BUFFER_DESC matBufferDesc(sizeof(MaterialProperties), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&matBufferDesc,
				nullptr,
				&m_pixelShaderMatConstBuff
			)
		);
	});

	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.

		ModelReturn r = LoadModel(modelName);
		this->Mesh = r.vert;
		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = this->Mesh;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPTN) * r.numVerts, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);
		auto device = m_deviceResources->GetD3DDevice();
		HRESULT hr = CreateDDSTextureFromFile(device, r.texture, nullptr, &m_Texture, 0);
		hr = CreateDDSTextureFromFile(device, r.specuMap, nullptr, &m_SpecuMap, 0);
		hr = CreateDDSTextureFromFile(device, r.normalMap, nullptr, &m_NormalMap, 0);
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
		uint16_t *indices = r.indices;
		this->indexCount = r.numIndices;


		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(uint16_t) * r.numIndices, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
	});

	createCubeTask.then([this]() {
		m_loadingComplete = true;
	});

}

void Model::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_inputLayout.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_pixelShaderLightConstBuff.Reset();
	m_pixelShaderMatConstBuff.Reset();
	m_Texture.Reset();
	m_SpecuMap.Reset();
	m_NormalMap.Reset();
	m_sampler.Reset();
}
