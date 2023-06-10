#pragma once

#include <string>
#include <Windows.h>

class SerialHandler {

private:
	static constexpr wchar_t PORT_PREFIX[] = L"\\\\.\\COM";
	static constexpr char REQUEST[] = "a";

	HANDLE serialHandle;

public:
	SerialHandler(const wchar_t* portNum);
	~SerialHandler();
	char* read();
	void flush();
	size_t write(std::string bufferToSend);
	size_t request();

private:
	void _printError();
};

