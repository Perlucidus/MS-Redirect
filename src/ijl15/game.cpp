#include "stdafx.h"
#include "game.h"
#include "crc.h"
#include "settings.h"
#include <iostream>

using namespace std;



void game::Init()
{
	crc::Bypass();
	Hack();
}

void game::Hack() {
	*(DWORD*)GAME_WIDTH = GetPrivateProfileInt("Resolution", "Width", 1024, config::CONFIG_PATH);
	*(DWORD*)GAME_HEIGHT = GetPrivateProfileInt("Resolution", "Height", 768, config::CONFIG_PATH);
	//Enable Aran, Evan, DualBlade buttons
	*(BYTE*)0x0063BEA8 = 1;
	*(BYTE*)0x0063BEBA = 1;
	*(BYTE*)0x0063BECC = 1;
	//Allow button click
	*(BYTE*)0x0064287A = 0xEB; //jmp
	*((BYTE*)0x0064287A + 1) = 7; //skip the switch jumptable
	//swap comingsoon with transparent 1x1 png
	const char* transparent = "UI/UIWindow2.img/raise/5/0";
	int i;
	for (i = 0; transparent[i]; i++)
		*((WORD*)0x00DF6610 + i) = transparent[i];
	*((WORD*)0x00DF6610 + i) = 0; //string end
}
