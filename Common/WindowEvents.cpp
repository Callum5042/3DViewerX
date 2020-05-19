
#include "WindowEvents.h"
using Events::EventType;

std::vector<Events::WindowListener*> Events::WindowListener::m_WindowListeners;

Events::WindowListener::WindowListener()
{
	m_WindowListeners.push_back(this);
}

Events::WindowListener::~WindowListener()
{
	auto it = std::find(m_WindowListeners.begin(), m_WindowListeners.end(), this);
	m_WindowListeners.erase(it);
}

Events::WindowQuit::WindowQuit()
{
	m_EventType = EventType::WINDOW_QUIT;
}

void Events::WindowQuit::Handle()
{
	std::vector<Events::WindowListener*> list;
	list.insert(list.end(), WindowListener::m_WindowListeners.begin(), WindowListener::m_WindowListeners.end());

	for (auto& listener : list)
	{
		listener->OnQuit();
	}
}

Events::WindowResize::WindowResize(int width, int height) : m_Width(width), m_Height(height)
{
	m_EventType = EventType::WINDOW_RESIZE;
}

void Events::WindowResize::Handle()
{
	std::vector<Events::WindowListener*> list;
	list.insert(list.end(), WindowListener::m_WindowListeners.begin(), WindowListener::m_WindowListeners.end());

	for (auto& listener : list)
	{
		listener->OnResize(m_Width, m_Height);
	}
}