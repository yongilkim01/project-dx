#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>

class WindowsMessageMap {
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;

private:
	std::unordered_map<DWORD, std::string> map;
};