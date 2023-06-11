#pragma once

#include <string>
#include <Windows.h>

#include "packet.h"

class SerialHandler {

private:
	static constexpr wchar_t PORT_PREFIX[] = L"\\\\.\\COM";
	static constexpr char REQUEST[] = "a";

	HANDLE serialHandle = nullptr;

public:
	SerialHandler();
	SerialHandler(const wchar_t* portNum);
	~SerialHandler();
	std::string read();
	void flush();
	size_t write(std::string bufferToSend);
	size_t request();
	SerialPacket getData();

private:
	void _printError();
};

