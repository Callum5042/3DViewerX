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

	constexpr DirectX::XMMATRIX GetPosition() { return m_Position; }
	constexpr DirectX::XMMATRIX GetView() { return m_View; }
	constexpr DirectX::XMMATRIX GetProjection() { return m_Projection; }

	// Input Events
	void OnKeyDown(Events::KeyData&& data) override;
	void OnKeyReleased(Events::KeyData&& data) override;
	void OnMouseWheel(Events::MouseWheelEvent* e) override;

private:
	DirectX::XMMATRIX m_Position;
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;

	float m_PosX = 0;
	float m_PosY = 0;
	float m_PosZ = -4.0f;
	float m_FOV = 85.0;

	bool m_WDown = false;
	bool m_ADown = false;
	bool m_SDown = false;
	bool m_DDown = false;
};