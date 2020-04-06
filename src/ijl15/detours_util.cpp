#include "stdafx.h"
#include "detours_util.h"
#include "detours.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

map<string, PVOID> detours::HookMap = map<string, PVOID>();

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

PVOID detours::Detour(PVOID ppvTarget, PVOID pvDetour, string key)
{
	if (!Detour(true, &ppvTarget, pvDetour)) {
		ostringstream oss;
		oss << "Failed to detour (" << key << ") " << ppvTarget << " to " << pvDetour;
		throw runtime_error(oss.str());
	}
	cout << "Detour" << endl
		<< "\t" << key << endl
		<< "\t" << ppvTarget << " -> " << pvDetour << endl;
	auto it = HookMap.insert(make_pair(key, ppvTarget));
	if (!it.second) {
		ostringstream oss;
		oss << key << " already detoured";
		throw runtime_error(oss.str());
	}
	return ppvTarget;
}

HMODULE detours::LoadLibraryS(LPCTSTR lpLibFileName)
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

PVOID detours::Detour(LPCTSTR lpLibFileName, LPCTSTR lpProcName, PVOID pvDetour)
{
	PVOID ppvTarget = (PVOID)GetProcAddress(detours::LoadLibraryS(lpLibFileName), lpProcName);
	try {
		ostringstream oss;
		oss << lpLibFileName << "::" << lpProcName;
		return detours::Detour(ppvTarget, pvDetour, oss.str());
	}
	catch (runtime_error e) {
		cout << e.what() << endl;
		throw;
	}
}
