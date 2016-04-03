#include "stdafx.h"
#include "PlayerData.h"

PlayerData::PlayerData(UINT32 playerPointer)
{
	address = playerPointer;
	CheckData();
	/*rider = *(int*)(playerPointer + offsets[11]);*/
	//8 player 9 team
}

void PlayerData::CheckData()
{
	active = *(bool*)(address + offsets[0]);
	id = *(int*)(address + offsets[1]);
	for (int i = 0; i < 3; i++)
	{
		vecRotation[i] = *(float*)(address + offsets[i + 2]);
	}
	for (int i = 0; i < 3; i++)
	{
		vec[i] = *(float*)(address + offsets[i + 5]);
	}
	health = *(float*)(address + offsets[10]);

}

