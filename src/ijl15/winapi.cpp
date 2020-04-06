#include "stdafx.h"
#include "winapi.h"
#include "detours_util.h"
#include "settings.h"
#include "game.h"
#include <iostream>
#include <stdexcept>

using namespace std;

HWND WINAPI winapi::CreateWindowExHook(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	PVOID proc = detours::HookMap["USER32::CreateWindowEx"];
	auto _CreateWindowEx = decltype(&CreateWindowEx)(proc);
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
		game::Init();
		lpLocalWndName = WINDOW_NAME; // Set window name
		cout << "CreateWindowEx with param " << lpParam << endl;
	}
	return _CreateWindowEx(dwExStyle, lpClassName, lpLocalWndName, dwStyle, x, y,
		nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

HANDLE WINAPI winapi::FindFirstFileHook(LPCTSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
	PVOID proc = detours::HookMap["KERNEL32::FindFirstFile"];
	auto _FindFirstFile = decltype(&FindFirstFile)(proc);
	if (lpFileName && !strcmp(lpFileName, "*"))
		return INVALID_HANDLE_VALUE;
	return _FindFirstFile(lpFileName, lpFindFileData);
}

DWORD WINAPI winapi::GetModuleFileNameHook(HMODULE hModule, LPTSTR lpFilename, DWORD nSize) {
	PVOID proc = detours::HookMap["KERNEL32::GetModuleFileName"];
	auto _GetModuleFileName = decltype(&GetModuleFileName)(proc);
	DWORD result = _GetModuleFileName(hModule, lpFilename, nSize);
	if (!result)
		result = _GetModuleFileName(NULL, lpFilename, nSize);
	return result;
}

HANDLE WINAPI winapi::CreateMutexHook(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName) {
	PVOID proc = detours::HookMap["KERNEL32::CreateMutex"];
	auto _CreateMutexA = decltype(&CreateMutex)(proc);
	if (lpName && !strcmp(lpName, "WvsClientMtx"))
	{
		cout << "Faking handle for mutex " << lpName << endl;
		return (HANDLE)0x12345678;
	}
	return _CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}

void winapi::Init()
{
	detours::Detour(CreateWindowEx, CreateWindowExHook, "USER32::CreateWindowEx");
	detours::Detour(FindFirstFile, FindFirstFileHook, "KERNEL32::FindFirstFile");
	detours::Detour(GetModuleFileName, GetModuleFileNameHook, "KERNEL32::GetModuleFileName");
	if (DISABLE_MUTEX)
		detours::Detour(CreateMutex, CreateMutexHook, "KERNEL32::CreateMutex");
}
