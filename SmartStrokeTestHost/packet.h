#pragma once

struct SerialPacket {
	int timeRaw = 0;
	int fsrOne = 0;
	int fsrTwo = 0;
	int timeSec = 0;
	int timeMs = 0;
	int accX = 0;
	int accY = 0;
	int accZ = 0;
};
