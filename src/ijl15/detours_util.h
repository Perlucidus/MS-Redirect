#pragma once

#pragma comment(lib, "detours.lib")

#include <windef.h>

BOOL Detour(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour);
VOID Detour(PVOID* ppvTarget, PVOID pvDetour);
HMODULE LoadLibraryS(LPCSTR lpLibFileName);
