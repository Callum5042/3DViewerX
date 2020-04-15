
#include "InputEvents.h"
using Events::EventType;

std::vector<Events::InputListener*> Events::InputListener::m_InputListeners;

Events::InputListener::InputListener()
{
	m_InputListeners.push_back(this);
}

Events::MousePressedEvent::MousePressedEvent()
{
	m_EventType = EventType::MOUSE_PRESSED;
}

void Events::MousePressedEvent::Handle()
{
	for (auto& listener : InputListener::m_InputListeners)
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
	for (auto& listener : InputListener::m_InputListeners)
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
	for (auto& listener : InputListener::m_InputListeners)
	{
		listener->OnMouseMotion(std::move(data));
	}
}

Events::MouseWheelEvent::MouseWheelEvent()
{
	m_EventType = EventType::MOUSE_WHEEL;
}

void Events::MouseWheelEvent::Handle()
{
	for (auto& listener : InputListener::m_InputListeners)
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
	for (auto& listener : InputListener::m_InputListeners)
	{
		listener->OnKeyDown(std::move(key));
	}
}