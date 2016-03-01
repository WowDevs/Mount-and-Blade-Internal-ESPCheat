#pragma once
class MainPlayer
{
public:
	float health;
	float vec[3];
	float vecRotation[3];
	DWORD pointer = 0;
	int offsets[11]{ 0x4, 0x8, 0x10, 0x14, 0x18, 0x40, 0x44, 0x48, 0x11c, 0x7b4, 0x6000 };
	MainPlayer();
	void CheckPlayer();
};

