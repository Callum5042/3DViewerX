#pragma once

#include "Events.h"
#include <vector>

namespace Events
{
	class WindowQuit : public IEvent
	{
	public:
		WindowQuit();
		virtual void Handle() override;
	};

	class WindowResize : public IEvent
	{
	public:
		WindowResize(int width, int height);
		virtual void Handle() override;

	private:
		int m_Width;
		int m_Height;
	};

	class WindowListener
	{
	public:
		WindowListener();
		virtual ~WindowListener() = default;

		virtual void OnQuit() { }
		virtual void OnResize(int width, int height) { }

	private:
		static std::vector<WindowListener*> m_WindowListeners;

		friend class WindowQuit;
		friend class WindowResize;
	};
}