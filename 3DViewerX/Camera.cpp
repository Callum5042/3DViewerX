#include "Camera.h"
#include "MainWindow.h"
#include "Viewport.h"
#include "Application.h"
#include <imgui.h>
#include <iostream>
#include "Input.h"
#include <SDL_mouse.h>

Camera::Camera()
{
	m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, -4.0f);

	Resize();
}

void Camera::Gui()
{
	ImGui::Checkbox("Freeroam", &m_Freeroam);

	if (ImGui::SliderFloat("FOV", &m_FOV, 10.0f, 150.0f))
	{
		Resize();
	}

	ImGui::SliderFloat("Pitch", &m_Pitch, 0.0f, 360.0f);
	ImGui::SliderFloat("Yaw", &m_Yaw, 0.0f, 360.0f);
	ImGui::SliderFloat("Roll", &m_Roll, 0.0f, 360.0f);
}

void Camera::Update()
{
	Timer* timer = reinterpret_cast<Application*>(Application::GetInstance())->GetTimer();

	if (m_Freeroam)
	{
		if (m_WDown)
		{
			DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
			DirectX::XMVECTOR camTarget = XMVector3TransformCoord(forward, camRotationMatrix);

			DirectX::XMVECTOR delta = DirectX::XMVectorReplicate((float)(timer->DeltaTime() * 5.0));
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
			DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(delta, camTarget, position));
		}
		else if (m_SDown)
		{
			DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
			DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
			DirectX::XMVECTOR camTarget = XMVector3TransformCoord(forward, camRotationMatrix);

			DirectX::XMVECTOR delta = DirectX::XMVectorReplicate((float)(timer->DeltaTime() * 5.0));
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
			DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(delta, camTarget, position));
		}

		if (m_ADown)
		{
			DirectX::XMVECTOR forward = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
			DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
			DirectX::XMVECTOR camTarget = XMVector3TransformCoord(forward, camRotationMatrix);

			DirectX::XMVECTOR delta = DirectX::XMVectorReplicate((float)(timer->DeltaTime() * 5.0));
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
			DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(delta, camTarget, position));
		}
		else if (m_DDown)
		{
			DirectX::XMVECTOR forward = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
			DirectX::XMVECTOR camTarget = XMVector3TransformCoord(forward, camRotationMatrix);

			DirectX::XMVECTOR delta = DirectX::XMVectorReplicate((float)(timer->DeltaTime() * 5.0));
			DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
			DirectX::XMStoreFloat3(&m_Position, DirectX::XMVectorMultiplyAdd(delta, camTarget, position));
		}
	}

	// Update camera
	if (m_Freeroam)
	{
		DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
		DirectX::XMVECTOR camTarget = XMVector3TransformCoord(forward, camRotationMatrix);

		DirectX::XMVECTOR eye = DirectX::XMVectorSet(m_Position.x, m_Position.y, m_Position.z, 1.0f);
		DirectX::XMVECTOR at = DirectX::XMVectorAdd(eye, camTarget);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
	}
	else
	{
		DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
		position = XMVector3TransformCoord(position, camRotationMatrix);

		DirectX::XMVECTOR eye = position;
		DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
	}
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

void Camera::OnMouseMotion(MouseData&& data)
{
	if (data.button == MouseButton::MOUSE_LMASK)
	{
		Viewport* viewport = reinterpret_cast<Application*>(Application::GetInstance()->GetInstance())->GetViewport();
		if (viewport->IsFocused())
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);

			m_Yaw += (data.xrel * 0.25f);
			m_Pitch += (data.yrel * 0.25f);

			// Make sure it stays between 0-360
			if (m_Pitch > 360)
			{
				m_Pitch = m_Pitch - 360.0f;
			}
			else if (m_Pitch < 0)
			{
				m_Pitch = m_Pitch + 360.0f;
			}

			// Make sure it stays between 0-360
			if (m_Yaw > 360)
			{
				m_Yaw = m_Yaw - 360.0f;
			}
			else if (m_Yaw < 0)
			{
				m_Yaw = m_Yaw + 360.0f;
			}
		}
	}
}

void Camera::OnMouseReleased(MouseData&& data)
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
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