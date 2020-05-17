#pragma once

#include <DirectXMath.h>
#include "InputEvents.h"

class Camera : public Events::InputListener
{
public:
	Camera();

	void Gui();
	void Update();
	void Resize();

	DirectX::XMMATRIX GetPosition() { return DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z); }
	constexpr DirectX::XMMATRIX GetView() { return m_View; }
	constexpr DirectX::XMMATRIX GetProjection() { return m_Projection; }
	constexpr bool IsFreeroam() { return m_Freeroam; }

	// Input Events
	void OnKeyDown(Events::KeyData&& data) override;
	void OnKeyReleased(Events::KeyData&& data) override;
	void OnMouseWheel(Events::MouseWheelEvent* e) override;

	// Mouse events
	void OnMouseMotion(MouseData&& data) override;
	void OnMouseReleased(MouseData&& data) override;

private:
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;
	DirectX::XMFLOAT3 m_Position;

	// Gui
	float m_FOV = 85.0;

	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	float m_Roll = 0.0f;

	bool m_Freeroam = true;

	// Keys
	bool m_WDown = false;
	bool m_ADown = false;
	bool m_SDown = false;
	bool m_DDown = false;
};