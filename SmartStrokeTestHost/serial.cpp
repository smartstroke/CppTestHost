#include "serial.h"

#include <iostream>
#include <sstream>

#define BUFFER_SIZE 255

SerialHandler::SerialHandler() {
}

SerialHandler::SerialHandler(const wchar_t* portNum) {
	wchar_t portName[16] = {};
	wcscpy_s(portName, PORT_PREFIX);
	wcscat_s(portName, portNum);

	serialHandle = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (serialHandle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			//serial port does not exist. Inform user.
			_printError();
		}
		//some other error occurred. Inform user.
		_printError();
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(serialHandle, &dcbSerialParams)) {
		//error getting state
		_printError();
	}
	dcbSerialParams.BaudRate = CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(serialHandle, &dcbSerialParams)) {
		//error setting serial port state
		_printError();
	}

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 10;
	timeouts.ReadTotalTimeoutConstant = 10;
	timeouts.ReadTotalTimeoutMultiplier = 5;
	timeouts.WriteTotalTimeoutConstant = 10;
	timeouts.WriteTotalTimeoutMultiplier = 5;
	if (!SetCommTimeouts(serialHandle, &timeouts)) {
		//error occureed. Inform user
		_printError();
	}

}

SerialHandler::~SerialHandler() {
	CloseHandle(serialHandle);
}

void SerialHandler::flush() {
	char szBuff[BUFFER_SIZE + 1] = { 0 };
	DWORD dwBytesRead = 0;
	do {
		if (!ReadFile(serialHandle, szBuff, BUFFER_SIZE, &dwBytesRead, NULL)) {
			//error occurred. Report to user.
			_printError();
		}
		std::cout << "[SERIAL FLUSH]" << szBuff << std::endl;
	} while (dwBytesRead);
}

std::string SerialHandler::read() {
	char szBuff[BUFFER_SIZE + 1] = { 0 };
	DWORD dwBytesRead = 0;
	if (!ReadFile(serialHandle, szBuff, BUFFER_SIZE, &dwBytesRead, NULL)) {
		//error occurred. Report to user.
		_printError();
	}
	return std::string(szBuff);
}

size_t SerialHandler::write(std::string bufferToSend) {
	char szBuff[BUFFER_SIZE + 1] = { 0 };
	memcpy(szBuff, bufferToSend.c_str(), bufferToSend.size());
	DWORD dwBytesWritten = 0;
	if (!WriteFile(serialHandle, szBuff, BUFFER_SIZE, &dwBytesWritten, NULL)) {
		//error occurred. Report to user.
		_printError();
	}
	return (size_t)dwBytesWritten;
}

size_t SerialHandler::request() {
	char requestBuffer[2];
	strcpy_s(requestBuffer, REQUEST);

	DWORD dwBytesWritten = 0;
	if (!WriteFile(serialHandle, requestBuffer, 1, &dwBytesWritten, NULL)) {
		//error occurred. Report to user.
		_printError();
	}
	return (size_t)dwBytesWritten;
}

SerialPacket SerialHandler::getData() {
	SerialPacket parsedPacket;
	if (serialHandle == nullptr) {
		return parsedPacket;
	}
	request();
	std::istringstream(read()) 
		>> parsedPacket.timeRaw 
		>> parsedPacket.fsrOne 
		>> parsedPacket.fsrTwo 
		>> parsedPacket.timeSec 
		>> parsedPacket.timeMs
		>> parsedPacket.accX
		>> parsedPacket.accY
		>> parsedPacket.accZ;

	return parsedPacket;
}

void SerialHandler::_printError() {
	wchar_t lastError[1024];
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lastError,
		1024,
		NULL);
	std::wcout << "[SERIAL ERROR]" << lastError << std::endl;
}
