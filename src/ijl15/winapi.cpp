#include "stdafx.h"
#include "winapi.h"
#include "detours_util.h"
#include "settings.h"
#include "memory.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>

using namespace std;

void hook_load_library()
{
	HMODULE hModule = LoadLibraryA("KERNEL32");
	if (!hModule)
		throw exception("Could not load KERNEL32");
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
	cout << "Redirecting LoadLibraryA to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_LoadLibraryA), Hook))
		throw exception("Failed to hook LoadLibraryA");
}

void hook_create_process()
{
	HMODULE hModule = LoadLibraryA("KERNEL32");
	if (!hModule)
		throw exception("Could not load KERNEL32");
	static auto _CreateProcessA = decltype(&CreateProcessA)(GetProcAddress(hModule, "CreateProcessA"));
	decltype(&CreateProcessA) Hook = [](
		LPCSTR lpApplicationName,
		LPSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation
		) -> BOOL
	{
		cout << "CreateProcess " << lpCommandLine << endl;
		return _CreateProcessA(
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes, 
			bInheritHandles, 
			dwCreationFlags, 
			lpEnvironment, 
			lpCurrentDirectory, 
			lpStartupInfo, 
			lpProcessInformation
		);
	};
	cout << "Redirecting CreateProcessA to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_CreateProcessA), Hook))
		throw exception("Failed to hook CreateProcessA");
}

void hook_create_window()
{
	HMODULE hModule = LoadLibraryA("USER32");
	if (!hModule)
		throw exception("Could not load USER32");
	cout << "USER32 loaded with hModule: " << hModule << endl;
	static auto _CreateWindowExA = decltype(&CreateWindowExA)(GetProcAddress(hModule, "CreateWindowExA"));
	decltype(&CreateWindowExA) Hook = [](DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) -> HWND
	{
		auto lpLocalWndName = lpWindowName;
		if (!strcmp(lpClassName, "StartUpDlgClass"))
		{
			WSADATA wsaData;
			int err;
			if (err = WSAStartup(MAKEWORD(2, 2), &wsaData))
				cout << "WSAStartup failed with error " << err << endl;
			bypass();
			return NULL; // Skip startup
		}
		else if (!strcmp(lpClassName, "NexonADBallon"))
			return NULL; // Skip ad
		else if (!strcmp(lpClassName, "MapleStoryClass"))
		{
			lpLocalWndName = WINDOW_NAME; // Set window name
			cout << "CreateWindowEx with param " << lpParam << endl;
		}
		return _CreateWindowExA(dwExStyle, lpClassName, lpLocalWndName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};
	cout << "Redirecting CreateWindowExA to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_CreateWindowExA), Hook))
		throw exception("Failed to hook CreateWindowExA");
}

void hook_create_mutex()
{
	HMODULE hModule = LoadLibraryA("KERNEL32");
	if (!hModule)
		throw exception("Could not load KERNEL32");
	static auto _CreateMutexA = decltype(&CreateMutexA)(GetProcAddress(hModule, "CreateMutexA"));
	decltype(&CreateMutexA) Hook = [](LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) -> HANDLE
	{
		if (lpName && !strcmp(lpName, "WvsClientMtx"))
		{
			cout << "Faking mutex handle " << lpName << endl;
			return (HANDLE)123; //Any handle which is not the real one
		}
		return _CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	};
	cout << "Redirecting CreateMutexA to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_CreateMutexA), Hook))
		throw exception("Failed to hook CreateMutexA");
}
