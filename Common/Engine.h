#pragma once

#include "MainWindow.h"
#include "Events.h"

class Engine
{
public:
	Engine();
	virtual ~Engine();

	int Execute(int argc, char** argv);

	static Engine* GetInstance() { return m_Instance; }
	Events::EventDispatcher* GetEventDispatcher() const { return m_EventDispatcher; }
	MainWindow* GetWindow() const { return m_MainWindow; }

	void Exit();

protected:
	virtual bool OnInitialise();
	virtual void OnUpdate();
	virtual void OnRender();

private:
	static Engine* m_Instance;

	bool m_Running = true;

	MainWindow* m_MainWindow = nullptr;
	Events::EventDispatcher* m_EventDispatcher = nullptr;

	bool Initialise();
};