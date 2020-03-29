#pragma once

#pragma comment(lib, "detours.lib")

#include <windef.h>

namespace detours {
	BOOL Detour(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour);
	VOID Detour(PVOID* ppvTarget, PVOID pvDetour);
	HMODULE LoadLibraryS(LPCSTR lpLibFileName);
}
