#pragma once

#include <DirectXMath.h>
#include "WindowEvents.h"
#include "InputEvents.h"

class Camera : public Events::WindowListener, public Events::InputListener
{
public:
	Camera();

	void Update();

	constexpr DirectX::XMMATRIX GetView() { return m_View; }
	constexpr DirectX::XMMATRIX GetProjection() { return m_Projection; }

	// Window Events
	void OnResize(int width, int height) override;

	// Input Events
	void OnKeyDown(Events::KeyData&& data) override;
	void OnMouseWheel(Events::MouseWheelEvent* e) override;

private:
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;

	float m_Distance = -4;
};