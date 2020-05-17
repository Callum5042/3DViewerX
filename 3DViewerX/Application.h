#pragma once

#include "Engine.h"
#include "WindowEvents.h"

#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "Viewport.h"
#include "Timer.h"
#include "Sky.h"

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
	constexpr Viewport* GetViewport() { return m_Viewport; }
	constexpr Timer* GetTimer() { return m_Timer; }

	constexpr double GetFPS() { return m_FPS; }

private:
	Renderer* m_Renderer = nullptr;
	Camera* m_Camera = nullptr;
	Model* m_Model = nullptr;
	Viewport* m_Viewport = nullptr;
	Timer* m_Timer = nullptr;

	void CalculateFrameStats();
	double m_FPS = 0;
};