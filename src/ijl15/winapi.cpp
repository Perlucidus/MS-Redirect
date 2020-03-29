#include "stdafx.h"
#include "winapi.h"
#include "detours_util.h"
#include "settings.h"
#include "memory.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>

using namespace std;

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
		cout << "CreateWindowEx " << lpClassName << endl;
		if (!strcmp(lpClassName, "StartUpDlgClass"))
		{
			cout << "CreateWindowEx StartUpDlgClass" << endl;
			return NULL; // Skip startup
		}
		else if (!strcmp(lpClassName, "NexonADBallon")) {
			cout << "NexonADBallon" << endl;
			return NULL; // Skip ad
		}
		else if (!strcmp(lpClassName, "MapleStoryClass"))
		{
			bypass();
			memedit();
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

void detourFindFirstFile()
{
	HMODULE hModule = LoadLibraryS("KERNEL32");
	static auto _FindFirstFileA = decltype(&FindFirstFileA)(GetProcAddress(hModule, "FindFirstFileA"));
	decltype(&FindFirstFileA) Hook = [](LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) -> HANDLE
	{
		if (lpFileName && !strcmp(lpFileName, "*"))
			return INVALID_FILE_HANDLE;
		return _FindFirstFileA(lpFileName, lpFindFileData);
	};
	cout << "Redirect FindFirstFileA\t\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_FindFirstFileA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour FindFirstFileA" << endl;
		throw;
	}
}

void detourGetModuleFileName()
{
	HMODULE hModule = LoadLibraryS("KERNEL32");
	static auto _GetModuleFileNameA = decltype(&GetModuleFileNameA)(GetProcAddress(hModule, "GetModuleFileNameA"));
	decltype(&GetModuleFileNameA) Hook = [](HMODULE hModule, LPSTR lpFilename, DWORD nSize) -> DWORD
	{
		DWORD result = _GetModuleFileNameA(hModule, lpFilename, nSize);
		if (!result) {
			cout << "GetModuleFileNameA failed for " << lpFilename << endl;
			result = _GetModuleFileNameA(NULL, lpFilename, nSize);
		}
		return result;
	};
	cout << "Redirect GetModuleFileNameA\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_GetModuleFileNameA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour GetModuleFileNameA" << endl;
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
			return INVALID_MUTEX_HANDLE; //Any handle which is not the real one
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
