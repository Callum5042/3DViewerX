#pragma once

#include "Engine.h"
#include "WindowEvents.h"

#include "Renderer.h"
#include "Camera.h"
#include "Model.h"

class Application : public Engine, public Events::WindowListener
{
public:
	Application() = default;

	bool OnInitialise() override;
	void OnUpdate() override;
	void OnRender() override;

	// Window Events
	void OnQuit() override;
	void OnResize(int width, int height) override;

	constexpr Camera* GetCamera() { return m_Camera; }

private:
	Renderer* m_Renderer = nullptr;
	Camera* m_Camera = nullptr;
	Model* m_Model = nullptr;

	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_TextureMap = nullptr;
};