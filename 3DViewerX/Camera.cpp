#include "Camera.h"
#include "MainWindow.h"
#include "Viewport.h"
#include "Application.h"
#include <imgui.h>
#include <iostream>

Camera::Camera()
{
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);

	m_Position = DirectX::XMMatrixIdentity();
	m_Position *= DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f);

	Resize();
}

void Camera::Gui()
{
	if (ImGui::SliderFloat("FOV", &m_FOV, 10.0f, 150.0f))
	{
		Resize();
	}
}

void Camera::Update()
{
	Timer* timer = reinterpret_cast<Application*>(Application::GetInstance())->GetTimer();

	if (m_ADown)
	{
		m_PosX -= 5.0 * timer->DeltaTime();
	}

	if (m_DDown)
	{
		m_PosX += 5.0 * timer->DeltaTime();
	}

	if (m_WDown)
	{
		m_PosZ += 5.0 * timer->DeltaTime();
	}
	
	if (m_SDown)
	{
		m_PosZ -= 5.0 * timer->DeltaTime();
	}

	m_Position = DirectX::XMMatrixIdentity();
	m_Position *= DirectX::XMMatrixTranslation(m_PosX, m_PosY, m_PosZ);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(m_PosX, m_PosY, m_PosZ, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}

void Camera::OnKeyDown(Events::KeyData&& data)
{
	if (data.key == SDL_SCANCODE_A)
	{
		m_ADown = true;
	}
	else if (data.key == SDL_SCANCODE_D)
	{
		m_DDown = true;
	}

	if (data.key == SDL_SCANCODE_W)
	{
		m_WDown = true;
	}
	else if (data.key == SDL_SCANCODE_S)
	{
		m_SDown = true;
	}
}

void Camera::OnKeyReleased(Events::KeyData&& data)
{
	if (data.key == SDL_SCANCODE_A)
	{
		m_ADown = false;
	}
	else if (data.key == SDL_SCANCODE_D)
	{
		m_DDown = false;
	}

	if (data.key == SDL_SCANCODE_W)
	{
		m_WDown = false;
	}
	else if (data.key == SDL_SCANCODE_S)
	{
		m_SDown = false;
	}
}

void Camera::OnMouseWheel(Events::MouseWheelEvent* e)
{
	if (e->wheel == MouseWheel::WHEEL_UP)
	{ 
		if (m_FOV > 10.0f)
		{
			m_FOV -= 1.0f;
		}
	}
	else if (e->wheel == MouseWheel::WHEEL_DOWN)
	{
		if (m_FOV < 150.0f)
		{
			m_FOV += 1.0f;
		}
	}

	Resize();
}

void Camera::Resize()
{
	Viewport* viewport = reinterpret_cast<Application*>(Engine::GetInstance())->GetViewport();
	int width = viewport->GetWidth();
	int height = viewport->GetHeight();

	float fieldOfView = m_FOV * DirectX::XM_PI / 180;
	float screenAspect = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.01f, 100.0f);
}