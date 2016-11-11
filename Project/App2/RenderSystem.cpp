#include "pch.h"
#include "RenderSystem.h"

#include "..\Common\DirectXHelper.h"

RenderSystem::RenderSystem(std::shared_ptr<DX::DeviceResources> m_deviceResources)
{
	static const DirectX::XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const DirectX::XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&Camera, XMMatrixLookAtRH(eye, at, up));
	this->Models = new Model("Assets\\Turret.obj", &Camera , &Projection, m_deviceResources, &m_LightProperties);
	this->InstancedModels = new InstancedModel("Assets\\Helicopter.obj", &Camera, &Projection, m_deviceResources, &m_LightProperties, 2);
	this->Cube = new GeneratedCube(m_deviceResources, &Camera, &Projection, &m_LightProperties);
	this->skybox = new Skybox(m_deviceResources, &Camera, &Projection, &m_LightProperties);
	this->m_deviceResources = m_deviceResources;
	this->CreateWindowSizeDependentResources();
	this->CreateDeviceDependantResources();
	int i = 0;
}

RenderSystem::~RenderSystem()
{
	Models->ReleaseDeviceDependentResources();
	delete Models;
	Cube->ReleaseDeviceDependentResources();
	delete Cube;
	InstancedModels->ReleaseDeviceDependentResources();
	delete InstancedModels;
}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;
#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;

extern char buttons[256];

void RenderSystem::Update(DX::StepTimer const & timer)
{
	DirectX::XMMATRIX newcamera = XMLoadFloat4x4(&Camera);

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * (float)-timer.GetElapsedSeconds() * 5.0f;
	}

	if (a_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * (float)-timer.GetElapsedSeconds() *5.0f;
	}

	if (s_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * (float)timer.GetElapsedSeconds() * 5.0f;
	}

	if (d_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * (float)timer.GetElapsedSeconds() * 5.0f;
	}

	Windows::UI::Input::PointerPoint^ point = nullptr;

	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
	//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);

	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			DirectX::XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(0, 0, 0, 1));
			newcamera = DirectX::XMMatrixRotationX(-diffy*0.01f) * newcamera * DirectX::XMMatrixRotationY(-diffx*0.01f);
			newcamera.r[3] = pos;
		}
	}

	XMStoreFloat4x4(&Camera, newcamera);

	XMStoreFloat4(&m_LightProperties.EyePosition, newcamera.r[3]);

	static const XMVECTORF32 LightColors[MAX_LIGHTS] = {
		Colors::Red, Colors::Orange, Colors::Yellow, Colors::Green, Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
	};

	static const LightType LightTypes[MAX_LIGHTS] = {
		DirectionalLight, PointLight, SpotLight, PointLight, SpotLight, PointLight, SpotLight, PointLight
	};

	static const bool LightEnabled[MAX_LIGHTS] = {
		true, true, true, true, true, true, true, true
	};

	const int numLights = MAX_LIGHTS;
	float radius = 8.0f;
	float offset = 2.0f * XM_PI / numLights;
	for (int i = 0; i < numLights; ++i)
	{
		Light light;
		light.Enabled = static_cast<int>(LightEnabled[i]);
		light.lightType = LightTypes[i];
		light.Color = XMFLOAT4(LightColors[i]);
		light.SpotAngle = XMConvertToRadians(45.0f);
		light.ConstantAttenuation = 1.0f;
		light.LinearAttenuation = 0.08f;
		light.QuadraticAttenuation = 0.0f;
		XMFLOAT4 LightPosition = XMFLOAT4(std::sin(totalTime + offset * i) * radius, 9.0f, std::cos(totalTime + offset * i) * radius, 1.0f);
		light.Position = LightPosition;
		XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
		LightDirection = XMVector3Normalize(LightDirection);
		XMStoreFloat4(&light.Direction, LightDirection);

		m_LightProperties.Lights[i] = light;
	}
	m_LightProperties.GlobalAmbient = XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Models->Update(timer);
	Cube->Update(timer);
	skybox->Update(timer);
	InstancedModels->Update(timer);
	totalTime += (float)timer.GetElapsedSeconds();
}

bool RenderSystem::Render()
{
	if(!m_loaded)
	{
		return false;
	}
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();
	ID3D11RenderTargetView *const targets[1] = { m_RTV2.Get() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	skybox->Render();
	Models->Render(nullptr);
	Cube->Render();
	ID3D11RenderTargetView *const targety[1] = { m_RTV.Get() };
	context->OMSetRenderTargets(1, targety, m_deviceResources->GetDepthStencilView());
	device->CreateShaderResourceView(m_RTVBuffer2.Get(), nullptr, m_Texture.GetAddressOf());
	skybox->Render();
	Models->Render(m_Texture.Get());
	Cube->Render();
	InstancedModels->Render();
	m_Texture.Reset();
	ID3D11RenderTargetView *const target[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	device->CreateShaderResourceView(m_RTVBuffer.Get(), nullptr, m_Texture.GetAddressOf());
	context->OMSetRenderTargets(1, target, m_deviceResources->GetDepthStencilView());
	context->UpdateSubresource(m_vertexBuffer.Get(), 0, nullptr, &screenOrientedQuad, 0, 0);
	unsigned int stride = sizeof(AppData);
	unsigned int offset = 0;
	ID3D11Buffer* Buffers = m_vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &Buffers, &stride, &offset);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);;

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
	context->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());

	context->DrawIndexed(m_indexCount, 0, 0);
	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	context->PSSetShaderResources(0, 1, pSRV);
	m_Texture.Reset();
	return true;
}

void RenderSystem::CreateDeviceDependantResources()
{
	skybox->CreateDeviceDependentResources();
	Models->CreateDeviceDependentResources();
	Cube->CreateDeviceDependentResources();
	InstancedModels->CreateDeviceDependentResources();

	auto loadVSTask = DX::ReadDataAsync(L"PostVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"PostPixelShader.cso");

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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);
	});

	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(AppData) * 4, D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			nullptr,
			&m_vertexBuffer
		)
	);

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

	auto device = m_deviceResources->GetD3DDevice();
	device->CreateSamplerState(&samplerDesc, &m_sampler);

	static const unsigned short cubeIndices[] =
	{
		1,2,0, // -x w/N
		3,2,1,
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

	screenOrientedQuad[0].pos = DirectX::XMFLOAT3(-1, -1, 0);
	screenOrientedQuad[0].uv = DirectX::XMFLOAT2(0, 1);
	screenOrientedQuad[1].pos = DirectX::XMFLOAT3(-1, 1, 0);
	screenOrientedQuad[1].uv = DirectX::XMFLOAT2(0, 0);
	screenOrientedQuad[2].pos = DirectX::XMFLOAT3(1, -1, 0);
	screenOrientedQuad[2].uv = DirectX::XMFLOAT2(1, 1);
	screenOrientedQuad[3].pos = DirectX::XMFLOAT3(1, 1, 0);
	screenOrientedQuad[3].uv = DirectX::XMFLOAT2(1, 0);

	m_loaded = true;
}

void RenderSystem::CreateWindowSizeDependentResources()
{
	Windows::Foundation::Size outputSize = this->m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 60.0f * DirectX::XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	DirectX::XMFLOAT4X4 orientation = this->m_deviceResources->GetOrientationTransform3D();

	DirectX::XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&Projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	float halfwidth = outputSize.Width / 2, halfheight = outputSize.Height / 2;

	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		lround(outputSize.Width),
		lround(outputSize.Height),
		1,
		1,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
	);
	auto device = m_deviceResources->GetD3DDevice();
	device->CreateTexture2D1(&depthStencilDesc, nullptr, m_RTVBuffer.GetAddressOf());
	device->CreateTexture2D1(&depthStencilDesc, nullptr, m_RTVBuffer2.GetAddressOf());
	device->CreateRenderTargetView1(m_RTVBuffer.Get(), nullptr, m_RTV.GetAddressOf());
	device->CreateRenderTargetView1(m_RTVBuffer2.Get(), nullptr, m_RTV2.GetAddressOf());


	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
}

void RenderSystem::ReleaseDeviceDependantResources()
{
	Models->ReleaseDeviceDependentResources();
	skybox->ReleaseDeviceDependentResources();
	Cube->ReleaseDeviceDependentResources();
	InstancedModels->ReleaseDeviceDependentResources();
	m_RTV.Reset();
	m_RTVBuffer.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_indexBuffer.Reset();
	m_Texture.Reset();
	m_sampler.Reset();
	m_RTV2.Reset();
	m_RTVBuffer2.Reset();
	m_loaded = false;
}
