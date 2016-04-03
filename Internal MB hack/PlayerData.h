#pragma once

class PlayerData
{
public:
	float health;
	int id;
	float vec[3];
	float vecRotation[3];
	bool active;
	int address;
	int offsets[11]{ 0x4, 0x8, 0x10, 0x14, 0x18, 0x40, 0x44, 0x48, 0x11c, 0x7b4, 0x6000 };
	int rider;
	float vec2Dpoint[3];
public:
	PlayerData(UINT32 playerPointer);
	void CheckData();
};

