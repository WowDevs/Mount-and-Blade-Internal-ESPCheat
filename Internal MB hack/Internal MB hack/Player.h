#pragma once
#include "PlayerData.h"

class Player: public PlayerData
{
public:
	int team;
	Player(UINT32 playerPointer);
	void checkDataPlayer();
};

