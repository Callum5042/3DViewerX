#include "Camera.h"
#include "MainWindow.h"
#include "Application.h"

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

}

void Camera::OnResize(int width, int height)
{
	// Create the projection matrix for 3D rendering.
	float fieldOfView = 85 * DirectX::XM_PI / 180;
	float screenAspect = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.01f, 100.0f);
}

void Camera::OnKeyDown(Events::KeyData&& data)
{
	// Could make camera movements
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

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, m_Distance, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}