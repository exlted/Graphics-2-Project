#include "pch.h"
#include "RenderSystem.h"

#include "..\Common\DirectXHelper.h"

RenderSystem::RenderSystem(std::shared_ptr<DX::DeviceResources> m_deviceResources)
{
	static const DirectX::XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const DirectX::XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&Camera, XMMatrixLookAtRH(eye, at, up));
	//this->Models = new Model("Assets\\Helicopter.obj", &Camera , &Projection, m_deviceResources, &m_LightProperties);
	this->InstancedModels = new InstancedModel("Assets\\Helicopter.obj", &Camera, &Projection, m_deviceResources, &m_LightProperties, 2);
	this->Cube = new GeneratedCube(m_deviceResources, &Camera, &Projection, &m_LightProperties);
	this->skybox = new Skybox(m_deviceResources, &Camera, &Projection, &m_LightProperties);
	this->m_deviceResources = m_deviceResources;
	this->CreateWindowSizeDependentResources();
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
		SpotLight, PointLight, SpotLight, PointLight, SpotLight, PointLight, SpotLight, PointLight
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
	//Models->Update(timer);
	Cube->Update(timer);
	skybox->Update(timer);
	InstancedModels->Update(timer);
	totalTime += (float)timer.GetElapsedSeconds();
}

bool RenderSystem::Render()
{
	//Models->Render();
	skybox->Render();
	Cube->Render();
	InstancedModels->Render();
	return true;
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

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
}
