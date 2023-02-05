#include "DxException.h"
#include <sstream>


DxException::DxException(int _line, const char* _file) noexcept
	:
	line(_line),
	file(_file)
{}

const char* DxException::what() const noexcept
{
	std::ostringstream _oss;
	_oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = _oss.str();
	return whatBuffer.c_str();
}

const char* DxException::GetType() const noexcept
{
	return "Chili Exception";
}

int DxException::GetLine() const noexcept
{
	return line;
}

const std::string& DxException::GetFile() const noexcept
{
	return file;
}

std::string DxException::GetOriginString() const noexcept
{
	std::ostringstream _oss;
	_oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return _oss.str();
}