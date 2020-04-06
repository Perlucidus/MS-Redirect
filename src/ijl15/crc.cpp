#include "stdafx.h"
#include "crc.h"
#include "detours_util.h"
#include "settings.h"

PCHAR ImageBase;
PCHAR ImageEnd;
PVOID ImageCopy;
PVOID CrcMain;
PVOID Crc1;
PVOID Crc2;

void __declspec(naked) CrcMainHook()
{
	__asm {
		push eax
		mov eax, [game::crc::CRC1]
		sub eax, 0x10
		cmp edx, eax
		jb Crc1CheckEnd //Jump if below CRC1 - 0x10
		add eax, 0x20
		cmp edx, eax
		ja Crc1CheckEnd //Jump if above CRC1 + 0x10
		sub edx, [ImageBase]
		add edx, [ImageCopy]
		jmp CrcMainEnd

		Crc1CheckEnd:

		mov eax, [game::crc::CRC2]
		sub eax, 0x10
		cmp edx, eax
		jb Crc2CheckEnd //Jump if below CRC2 - 0x10
		add eax, 0x20
		cmp edx, eax
		ja Crc2CheckEnd //Jump if above CRC2 + 0x10
		sub edx, [ImageBase]
		add edx, [ImageCopy]
		jmp CrcMainEnd

		Crc2CheckEnd:

		CrcMainEnd:

		pop eax
		jmp CrcMain
	}
}

void __declspec(naked) Crc1Hook()
{
	__asm {
		cmp ebx, [ImageBase]
		jb Crc1End
		cmp ebx, [ImageEnd]
		ja Crc1End
		sub ebx, [ImageBase]
		add ebx, [ImageCopy]

		Crc1End:

		jmp Crc1
	}
}

void __declspec(naked) Crc2Hook()
{
	__asm {
		cmp ecx, [ImageBase]
		jb Crc2End
		cmp ecx, [ImageEnd]
		ja Crc2End
		sub ecx, [ImageBase]
		add ecx, [ImageCopy]

		Crc2End:

		jmp Crc2
	}
}

void game::crc::Bypass()
{
	ImageBase = (PCHAR)GetModuleHandle("MapleStory.exe");
	IMAGE_NT_HEADERS* nt_header = PIMAGE_NT_HEADERS(ImageBase + PIMAGE_DOS_HEADER(ImageBase)->e_lfanew);
	ImageEnd = ImageBase + nt_header->OptionalHeader.SizeOfImage;
	//Allocate space for the memory copy
	DWORD CrcRange = ImageEnd - ImageBase;
	ImageCopy = VirtualAlloc(NULL, CrcRange, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//Unprotect memory
	unsigned long ulProtect;
	VirtualProtect(ImageBase, CrcRange, PAGE_EXECUTE_READWRITE, &ulProtect);
	//Make a copy of the memory
	memcpy(ImageCopy, ImageBase, CrcRange);
	CrcMain = detours::Detour((PVOID)CRC_MAIN, CrcMainHook, "MapleStory::CrcMain");
	Crc1 = detours::Detour((PVOID)CRC1, Crc1Hook, "MapleStory::Crc1");
	Crc2 = detours::Detour((PVOID)CRC2, Crc2Hook, "MapleStory::Crc2");
}
