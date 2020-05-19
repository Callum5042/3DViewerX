
#include "InputEvents.h"
using Events::EventType;

std::vector<Events::InputListener*> Events::InputListener::m_InputListeners;

Events::InputListener::InputListener()
{
	m_InputListeners.push_back(this);
}

Events::InputListener::~InputListener()
{
	auto it = std::find(m_InputListeners.begin(), m_InputListeners.end(), this);
	m_InputListeners.erase(it);
}

Events::MousePressedEvent::MousePressedEvent()
{
	m_EventType = EventType::MOUSE_PRESSED;
}

void Events::MousePressedEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnMouseDown(std::move(data));
	}
}

Events::MouseReleasedEvent::MouseReleasedEvent()
{
	m_EventType = EventType::MOUSE_RELEASED;
}

void Events::MouseReleasedEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnMouseReleased(std::move(data));
	}
}

Events::MouseMotionEvent::MouseMotionEvent()
{
	m_EventType = EventType::MOUSE_MOTION;
}

void Events::MouseMotionEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnMouseMotion(std::move(data));
	}
}

Events::MouseWheelEvent::MouseWheelEvent()
{
	wheel = MouseWheel::WHEEL_NONE;
	m_EventType = EventType::MOUSE_WHEEL;
}

void Events::MouseWheelEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnMouseWheel(this);
	}
}

Events::KeyDownEvent::KeyDownEvent()
{
	EventType::KEY_DOWN;
}

void Events::KeyDownEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnKeyDown(std::move(key));
	}
}

Events::KeyReleasedEvent::KeyReleasedEvent()
{
	EventType::KEY_RELEASED;
}

void Events::KeyReleasedEvent::Handle()
{
	std::vector<Events::InputListener*> list;
	list.insert(list.end(), InputListener::m_InputListeners.begin(), InputListener::m_InputListeners.end());

	for (auto& listener : list)
	{
		listener->OnKeyReleased(std::move(key));
	}
}