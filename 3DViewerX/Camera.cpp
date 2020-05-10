#include "Camera.h"
#include "MainWindow.h"
#include "Application.h"
#include <imgui.h>
#include <iostream>

Camera::Camera()
{
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);

	MainWindow* window = Engine::GetInstance()->GetWindow();
	OnResize(window->GetWidth(), window->GetHeight());
}

void Camera::Update()
{
	ImGui::Begin("Camera");

	if (ImGui::SliderFloat("FOV", &m_POV, 0.0f, 360.0f))
	{
		MainWindow* window = Engine::GetInstance()->GetWindow();
		OnResize(window->GetWidth(), window->GetHeight());
	}

	ImGui::End();
}

void Camera::OnResize(int width, int height)
{
	// Create the projection matrix for 3D rendering.
	float fieldOfView = m_POV * DirectX::XM_PI / 180;
	float screenAspect = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.01f, 100.0f);
}

void Camera::OnKeyDown(Events::KeyData&& data)
{
	// Could make camera movements
	if (data.key == SDL_SCANCODE_A)
	{
		m_PosX -= 1.0f;
	}
	else if (data.key == SDL_SCANCODE_D)
	{
		m_PosX += 1.0f;
	}

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(m_PosX, 0.0f, m_Distance, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}

void Camera::OnMouseWheel(Events::MouseWheelEvent* e)
{
	if (e->wheel == MouseWheel::WHEEL_UP)
	{ 
		m_Distance += 0.1f;
	}
	else if (e->wheel == MouseWheel::WHEEL_DOWN)
	{
		m_Distance -= 0.1f;
	}

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(m_PosX, 0.0f, m_Distance, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}