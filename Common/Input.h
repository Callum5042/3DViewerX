#pragma once

template<typename T>
constexpr auto BITMASK(T x) { return (1 << x); }

enum class MouseButton
{
	MOUSE_NONE = 0,
	MOUSE_LMASK = BITMASK(0),
	MOUSE_MMASK = BITMASK(1),
	MOUSE_RMASK = BITMASK(2),
	MOUSE_X1MASK = BITMASK(3),
	MOUSE_X2MASK = BITMASK(4)
};

enum class MouseWheel
{
	WHEEL_NONE = 0,
	WHEEL_UP = 1,
	WHEEL_DOWN = -1,
};

struct MouseData
{
	MouseButton button = MouseButton::MOUSE_NONE;
	int x = 0;
	int y = 0;
	int xrel = 0;
	int yrel = 0;
};

class Input
{
public:

protected:

};