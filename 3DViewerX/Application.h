#pragma once

#include "Engine.h"
#include "WindowEvents.h"

#include "Model.h"
#include "Renderer.h"

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

private:
	Renderer* m_Renderer = nullptr;
	Model* m_Model = nullptr;
};