#include "stdafx.h"
#include "winapi.h"
#include "detours_util.h"
#include "settings.h"
#include "memory.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>

using namespace std;

void detourLoadLibrary()
{
	HMODULE hModule = LoadLibraryS("KERNEL32");
	static auto _LoadLibraryA = decltype(&LoadLibraryA)(GetProcAddress(hModule, "LoadLibraryA"));
	decltype(&LoadLibraryA) Hook = [](LPCSTR lpLibFileName) -> HMODULE
	{
		if (!strcmp(lpLibFileName, "setupapi.dll") || !strcmp(lpLibFileName, "cfgmgr32.dll"))
		{
			cout << "LoadLibrary " << lpLibFileName << " blocked" << endl;
			return NULL;
		}
		else if (!strcmp(lpLibFileName, "ws2_32.dll"))
			cout << "LoadLibrary " << lpLibFileName << endl;
		return _LoadLibraryA(lpLibFileName);
	};
	cout << "Redirect LoadLibraryA\t\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_LoadLibraryA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour LoadLibrary" << endl;
		throw;
	}
}

void detourCreateWindowEx()
{
	HMODULE hModule = LoadLibraryS("USER32");
	static auto _CreateWindowExA = decltype(&CreateWindowExA)(GetProcAddress(hModule, "CreateWindowExA"));
	decltype(&CreateWindowExA) Hook = [](
		DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWindowName,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpParam
		) -> HWND
	{
		auto lpLocalWndName = lpWindowName;
		if (!strcmp(lpClassName, "StartUpDlgClass"))
		{
			WSADATA wsaData;
			int err;
			if (err = WSAStartup(MAKEWORD(2, 2), &wsaData))
				cout << "WSAStartup failed with error " << err << endl;
			bypass();
			cout << "CreateWindowEx StartUpDlgClass" << endl;
			return NULL; // Skip startup
		}
		else if (!strcmp(lpClassName, "NexonADBallon")) {
			cout << "NexonADBallon" << endl;
			return NULL; // Skip ad
		}
		else if (!strcmp(lpClassName, "MapleStoryClass"))
		{
			lpLocalWndName = WINDOW_NAME; // Set window name
			cout << "CreateWindowEx with param " << lpParam << endl;
		}
		return _CreateWindowExA(dwExStyle, lpClassName, lpLocalWndName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};
	cout << "Redirect CreateWindowExA\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_CreateWindowExA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour CreateWindowExA" << endl;
		throw;
	}
}

void detourCreateMutex()
{
	HMODULE hModule = LoadLibraryS("KERNEL32");
	static auto _CreateMutexA = decltype(&CreateMutexA)(GetProcAddress(hModule, "CreateMutexA"));
	decltype(&CreateMutexA) Hook = [](
		LPSECURITY_ATTRIBUTES lpMutexAttributes,
		BOOL bInitialOwner,
		LPCSTR lpName
		) -> HANDLE
	{
		if (lpName && !strcmp(lpName, "WvsClientMtx"))
		{
			cout << "Faking handle for mutex " << lpName << endl;
			return FAKE_MUTEX_HANDLE; //Any handle which is not the real one
		}
		return _CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	};
	cout << "Redirect CreateMutexA\t\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_CreateMutexA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour CreateMutexA" << endl;
		throw;
	}
}
