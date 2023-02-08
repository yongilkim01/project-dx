#include "Mouse.h"
#include <Windows.h>

std::pair<int, int> Mouse::GetPos() const noexcept { return { x, y }; }
int Mouse::GetPosX() const noexcept { return x; }
int Mouse::GetPosY() const noexcept { return y; }
bool Mouse::IsInWindow() const noexcept { return isInWindow; }
bool Mouse::LeftIsPressed() const noexcept { return leftIsPressed; }
bool Mouse::RightIsPressed() const noexcept { return rightIsPressed; }
void Mouse::Flush() noexcept { buffer = std::queue<Event>(); }
bool Mouse::IsEmpty() const noexcept { return buffer.empty(); }

Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event _e = buffer.front();
		buffer.pop();
		return _e;
	}
	else
	{
		return Mouse::Event();
	}
}

/******************** private side ********************/
void Mouse::OnMouseMove(int _x, int _y) noexcept
{
	x = _x;
	y = _y;

	buffer.push(Mouse::Event( Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int _x, int _y) noexcept
{
	leftIsPressed = true;
	
	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int _x, int _y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int _x, int _y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int _x, int _y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int _x, int _y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int _x, int _y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

void Mouse::OnWheelDelta(int _x, int _y, int _delta) noexcept
{
	wheelDeltaCarry += _delta;
	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(_x, _y);
	}

	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(_x, _y);
	}
}