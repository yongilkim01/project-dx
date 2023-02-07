#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char _keycode) const noexcept
{
	return keyStates[_keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keyBuffer.size() > 0u) 
	{
		Keyboard::Event _e = keyBuffer.front();
		keyBuffer.pop();
		return _e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (charBuffer.size() > 0u)
	{
		unsigned char _charCode = charBuffer.front();
		charBuffer.pop();
		return _charCode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autoRepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autoRepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autoRepeatEnabled;
}

/// <summary>
/// Private Side
/// </summary>
/// <param name="_keyCode"></param>

void Keyboard::OnKeyPressed(unsigned char _keyCode) noexcept
{
	keyStates[_keyCode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, _keyCode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char _keyCode) noexcept
{
	keyStates[_keyCode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, _keyCode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char _character) noexcept
{
	charBuffer.push(_character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& _buffer) noexcept
{
	while (_buffer.size() > bufferSize)
	{
		_buffer.pop();
	}
}

