#include "stdafx.h"
#include "Player.h"


Player::Player(UINT32 playerPointer) : PlayerData(playerPointer)
{
	team = *(bool*)playerPointer;
}

void Player::checkDataPlayer()
{
	CheckData();
	team = *(bool*)address;
}