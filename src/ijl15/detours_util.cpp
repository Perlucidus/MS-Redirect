#include "stdafx.h"
#include "detours_util.h"
#include "detours.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

BOOL detours::Detour(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour)
{
	if (DetourTransactionBegin() != NO_ERROR)
		return FALSE;
	auto tid = GetCurrentThread();
	if (DetourUpdateThread(tid) == NO_ERROR)
	{
		auto func = bInstall ? DetourAttach : DetourDetach;
		if (func(ppvTarget, pvDetour) == NO_ERROR)
			if (DetourTransactionCommit() == NO_ERROR)
				return TRUE;
	}
	DetourTransactionAbort();
	return FALSE;
}

VOID detours::Detour(PVOID* ppvTarget, PVOID pvDetour)
{
	if (!Detour(true, ppvTarget, pvDetour)) {
		ostringstream oss;
		oss << "Could not detour " << ppvTarget << " to " << pvDetour;
		throw runtime_error(oss.str());
	}
}

HMODULE detours::LoadLibraryS(LPCSTR lpLibFileName)
{
	HMODULE hModule = LoadLibrary(lpLibFileName);
	if (!hModule) {
		ostringstream oss;
		oss << "Could not load module ";
		oss << lpLibFileName;
		throw runtime_error(oss.str());
	}
	return hModule;
}
