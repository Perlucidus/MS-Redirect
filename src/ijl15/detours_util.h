#pragma once

#pragma comment(lib, "detours.lib")

#include <windef.h>

BOOL SetHook(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour);
