#pragma once

#include <DirectXMath.h>
#include "InputEvents.h"

class Camera : public Events::InputListener
{
public:
	Camera();

	void Update();

	constexpr DirectX::XMMATRIX GetView() { return m_View; }
	constexpr DirectX::XMMATRIX GetProjection() { return m_Projection; }

	// Input Events
	void OnKeyDown(Events::KeyData&& data) override;
	void OnMouseWheel(Events::MouseWheelEvent* e) override;

private:
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;

	float m_Distance = -20;
	float m_PosX = 0;
	float m_FOV = 85.0;

	void Resize();
};