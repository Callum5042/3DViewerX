
#include "Events.h"
#include <SDL_events.h>
#include "InputEvents.h"
#include "WindowEvents.h"
#include "Engine.h"
#include <iostream>

#include "examples/imgui_impl_sdl.h"

namespace
{
	void PollSDLEvents()
	{
		auto dispatcher = Engine::GetInstance()->GetEventDispatcher();

		SDL_Event e{};
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			switch (e.type)
			{
			case SDL_QUIT:
				dispatcher->AddEvent(new Events::WindowQuit());
				break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					dispatcher->AddEvent(new Events::WindowResize(e.window.data1, e.window.data2));
				}
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				auto mouseEvent = new Events::MousePressedEvent();
				mouseEvent->data.button = static_cast<MouseButton>((1 << (e.button.button - 1)));
				mouseEvent->data.x = e.button.x;
				mouseEvent->data.y = e.button.y;

				dispatcher->AddEvent(mouseEvent);
				break;	
			}

			case SDL_MOUSEBUTTONUP:
			{
				auto mouseEvent = new Events::MouseReleasedEvent();
				mouseEvent->data.button = static_cast<MouseButton>((1 << (e.button.button - 1)));
				mouseEvent->data.x = e.button.x;
				mouseEvent->data.y = e.button.y;

				dispatcher->AddEvent(mouseEvent);
				break;
			}

			case SDL_KEYDOWN:
			{
				if (!e.key.repeat)
				{
					auto keyEvent = new Events::KeyDownEvent();
					keyEvent->key.key = e.key.keysym.scancode;

					dispatcher->AddEvent(keyEvent);
				}
				break;
			}

			case SDL_MOUSEWHEEL:
			{
				auto mouseEvent = new Events::MouseWheelEvent();

				if (e.wheel.y > 0)
				{
					mouseEvent->wheel = MouseWheel::WHEEL_UP;
				}
				else if (e.wheel.y < 0)
				{
					mouseEvent->wheel = MouseWheel::WHEEL_DOWN;
				}

				dispatcher->AddEvent(mouseEvent);
				break;
			}

			case SDL_MOUSEMOTION:
			{
				auto mouseEvent = new Events::MouseMotionEvent();
				mouseEvent->data.button = static_cast<MouseButton>(e.motion.state);
				mouseEvent->data.x = e.motion.x;
				mouseEvent->data.y = e.motion.y;
				mouseEvent->data.xrel = e.motion.xrel;
				mouseEvent->data.yrel = e.motion.yrel;

				dispatcher->AddEvent(mouseEvent);
				break;
			}
			}
		}
	}
}

void Events::EventDispatcher::PollEvents()
{
	PollSDLEvents();

	while (!m_Events.empty())
	{
		IEvent* e = m_Events.front();
		e->Handle();

		m_Events.pop();
		delete e;
	}
}

void Events::EventDispatcher::AddEvent(IEvent* e)
{
	m_Events.push(e);
}