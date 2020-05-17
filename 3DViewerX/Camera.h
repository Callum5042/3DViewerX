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

	DirectX::XMMATRIX GetPosition() 
	{ 
		DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_Position);
		DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(m_Pitch), DirectX::XMConvertToRadians(m_Yaw), 0);
		position = XMVector3TransformCoord(position, camRotationMatrix);

		DirectX::XMFLOAT4 pos;
		DirectX::XMStoreFloat4(&pos, position);

		return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}


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

	DirectX::XMFLOAT3 m_LockStartPosition;

	// Gui
	float m_FOV = 85.0;

	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	float m_Roll = 0.0f;

	bool m_Freeroam = false;

	// Keys
	bool m_WDown = false;
	bool m_ADown = false;
	bool m_SDown = false;
	bool m_DDown = false;
};