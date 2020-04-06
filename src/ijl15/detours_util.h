#pragma once

#pragma comment(lib, "detours.lib")

#include <windef.h>
#include <string>
#include <map>

namespace detours {
	extern std::map<std::string, PVOID> HookMap;

	BOOL Detour(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour);
	PVOID Detour(PVOID ppvTarget, PVOID pvDetour, std::string key);
	HMODULE LoadLibraryS(LPCTSTR lpLibFileName);
	PVOID Detour(LPCTSTR lpLibFileName, LPCTSTR lpProcName, PVOID pvDetour);
}
