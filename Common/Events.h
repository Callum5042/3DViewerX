#pragma once

#include <queue>

namespace Events
{
	enum class EventType
	{
		NONE,
		MOUSE_PRESSED, MOUSE_RELEASED, MOUSE_MOTION, MOUSE_WHEEL,
		KEY_DOWN,
		WINDOW_QUIT, WINDOW_RESIZE
	};

	class IEvent
	{
	public:
		virtual ~IEvent() = default;
		virtual void Handle() = 0;

	protected:
		EventType m_EventType = EventType::NONE;
	};

	class EventDispatcher
	{
	public:
		void PollEvents();
		void AddEvent(IEvent* e);

	private:
		std::queue<IEvent*> m_Events;
	};
}