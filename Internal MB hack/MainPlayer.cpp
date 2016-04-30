#include "stdafx.h"
#include "MainPlayer.h"
#include <exception>

MainPlayer::MainPlayer()
{
	/*CheckPlayer();*/
}

void MainPlayer::CheckPlayer()
{
	DWORD newPointer = *(DWORD*)0x03137044;

	if (newPointer != pointer && newPointer > 0x5000 && 51704380 != newPointer && newPointer != 0)
	{
		try {
			pointer = newPointer;
		}
		catch (std::exception& e) {
			return;
		}
	}

	if (pointer != 0)
	{
		for (int i = 0; i < 3; i++)
		{
		vecRotation[i] = *(float*)(pointer + offsets[i + 2]);
		}
		for (int i = 0; i < 3; i++)
		{
			vec[i] = *(float*)(pointer + offsets[i + 5]);
		}
		health = *(float*)(pointer + offsets[10]);
		team = *(int*)(pointer + offsets[9]);
	}
}
