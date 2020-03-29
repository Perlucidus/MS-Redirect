#include "stdafx.h"
#include "memory.h"
#include "settings.h"
#include "detours_util.h"
#include <iostream>

using namespace std;

#define jmp(frm, to) (int)(((int)to - (int)frm) - 5)

constexpr BYTE JMP = 0xE9;
constexpr BYTE NOP = 0x90;

DWORD MSCRC_END;
constexpr DWORD MSCRC_MAIN_START1 = (MSCRC1 / 0x1000) * 0x1000;
constexpr DWORD MSCRC_MAIN_END1 = (MSCRC1 / 0x1000 + 1) * 0x1000;
constexpr DWORD MSCRC_MAIN_START2 = (MSCRC2 / 0x1000) * 0x1000;
constexpr DWORD MSCRC_MAIN_END2 = (MSCRC2 / 0x1000 + 1) * 0x1000;
constexpr DWORD MSCRCMainRet = MSCRC_MAIN + 5;
constexpr DWORD MSCRC1Ret = MSCRC1 + 5;
constexpr DWORD MSCRC2Ret = MSCRC2 + 5;
constexpr DWORD loc_1053E59 = 0x01053E59;

void* crcmainbuffer1;
void* crcmainbuffer2;
void* crcbuffer;

void __declspec(naked) MSCRCMainHook()
{
	__asm {
		//Test for the range of MSCRC1
		cmp edx, [MSCRC_MAIN_START1]
		jb Test2 //Jump if below MSCRC_MAIN_START1
		cmp edx, [MSCRC_MAIN_END1]
		ja Test2 //Jump if above MSCRC_MAIN_END1
		sub edx, [MSCRC_MAIN_START1] //edx -= MSCRC_MAIN_START1
		add edx, [crcmainbuffer1] //edx += crcmainbuffer1
		jmp OriginMain

		Test2: //Test for the range of MSCRC2
		cmp edx, [MSCRC_MAIN_START2]
		jb OriginMain //Jump if below MSCRC_MAIN_START2
		cmp edx, [MSCRC_MAIN_END2]
		ja OriginMain //Jump if above MSCRC_MAIN_END2
		sub edx, [MSCRC_MAIN_START2] //edx -= MSCRC_MAIN_START2
		add edx, [crcmainbuffer2] //edx += crcmainbuffer2
		jmp OriginMain

		OriginMain: //Original 7 Opcodes (2 nops are added on hijack)
		push [edx]
		jmp loc_1053E59
		jmp MSCRCMainRet //Not really required here but for future ref
	}
}

void __declspec(naked) MSCRC1Hook()
{
	__asm {
		cmp ebx, [MSCRC_START]
		jb Origin1 //Jump if below MSCRC_START
		cmp ebx, [MSCRC_END]
		ja Origin1 //Jump if above MSCRC_END
		sub ebx, [MSCRC_START] //ebx -= MSCRC_START
		add ebx, crcbuffer //ebx += crcbuffer

		Origin1: //Original 5 Opcodes
		add al, [ebx]
		pop ebx
		push bx
		jmp MSCRC1Ret
	}
}

void __declspec(naked) MSCRC2Hook()
{
	__asm {
		cmp ecx, [MSCRC_START]
		jb Origin2 //Jump if below MSCRC_START
		cmp ecx, [MSCRC_END]
		ja Origin2 //Jump if above MSCRC_END
		sub ecx, [MSCRC_START] //ebx -= MSCRC_START
		add ecx, crcbuffer //ebx += crcbuffer

		Origin2 : //Original 5 Opcodes
		mov dl, [ecx]
		add dl, 01
		jmp MSCRC2Ret
	}
}

void bypass()
{
	cout << "MSCRC Bypass" << endl;
	//Find end for MSCRC
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((void*)MSCRC_START, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	MSCRC_END = MSCRC_START + mbi.RegionSize;
	//Allocate space for the memory copy
	DWORD MSCRC_MAIN_RANGE1 = MSCRC_MAIN_END1 - MSCRC_MAIN_START1;
	DWORD MSCRC_MAIN_RANGE2 = MSCRC_MAIN_END2 - MSCRC_MAIN_START2;
	DWORD MSCRC_RANGE = MSCRC_END - MSCRC_START;
	crcmainbuffer1 = VirtualAlloc(NULL, MSCRC_MAIN_RANGE1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	crcmainbuffer2 = VirtualAlloc(NULL, MSCRC_MAIN_RANGE2, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	crcbuffer = VirtualAlloc(NULL, MSCRC_RANGE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//Unprotect memory
	unsigned long ulProtect;
	VirtualProtect((void*)MSCRC_MAIN_START1, MSCRC_MAIN_RANGE1, PAGE_EXECUTE_READWRITE, &ulProtect);
	VirtualProtect((void*)MSCRC_MAIN_START2, MSCRC_MAIN_RANGE2, PAGE_EXECUTE_READWRITE, &ulProtect);
	VirtualProtect((void*)MSCRC_START, MSCRC_RANGE, PAGE_EXECUTE_READWRITE, &ulProtect);
	//Make a copy of the memory
	memcpy(crcmainbuffer1, (void*)MSCRC_MAIN_START1, MSCRC_MAIN_RANGE1);
	memcpy(crcmainbuffer2, (void*)MSCRC_MAIN_START2, MSCRC_MAIN_RANGE2);
	memcpy(crcbuffer, (void*)MSCRC_START, MSCRC_RANGE);
	cout << "Redirect MSCRC_MAIN CRC1 Region\t" << crcmainbuffer1 << endl;
	cout << "Redirect MSCRC_MAIN CRC2 Region\t" << crcmainbuffer2 << endl;
	cout << "Redirect MSCRC1 Region\t\t" << crcbuffer << endl;
	cout << "Redirect MSCRC2 Region\t\t" << crcbuffer << endl;
	*(BYTE*)MSCRC_MAIN = JMP;
	*(DWORD*)(MSCRC_MAIN + 1) = jmp(MSCRC_MAIN, MSCRCMainHook);
	*(BYTE*)(MSCRC_MAIN + 5) = NOP;
	*(BYTE*)(MSCRC_MAIN + 6) = NOP;
	*(BYTE*)MSCRC1 = JMP;
	*(DWORD*)(MSCRC1 + 1) = jmp(MSCRC1, (int)MSCRC1Hook);
	*(BYTE*)MSCRC2 = JMP;
	*(DWORD*)(MSCRC2 + 1) = jmp(MSCRC2, (int)MSCRC2Hook);
	cout << "Redirect MSCRCMAIN\t\t" << MSCRCMainHook << endl;
	cout << "Redirect MSCRC1\t\t\t" << MSCRC1Hook << endl;
	cout << "Redirect MSCRC2\t\t\t" << MSCRC2Hook << endl;
}

void memedit() {
	*(DWORD*)GAME_WIDTH = GetPrivateProfileInt("Resolution", "Width", 1024, CONFIG_PATH);
	*(DWORD*)GAME_HEIGHT = GetPrivateProfileInt("Resolution", "Height", 768, CONFIG_PATH);
}
