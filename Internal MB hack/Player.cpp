#include "stdafx.h"
#include "Player.h"


Player::Player(UINT32 playerPointer) : PlayerData(playerPointer)
{
	team = *(int*)playerPointer;
}

void Player::checkDataPlayer()
{
	CheckData();
	team = *(int*)(address + offsets[9]);
}