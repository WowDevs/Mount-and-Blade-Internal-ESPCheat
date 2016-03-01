#pragma once
#include "PlayerData.h"

class Player: public PlayerData
{
public:
	bool team;
	Player(UINT32 playerPointer);
	void checkDataPlayer();
};

