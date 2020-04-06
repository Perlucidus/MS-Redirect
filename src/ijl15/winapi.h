#pragma once

namespace winapi {
	HWND WINAPI CreateWindowExHook(DWORD, LPCTSTR, LPCTSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
	HANDLE WINAPI FindFirstFileHook(LPCTSTR, LPWIN32_FIND_DATAA);
	DWORD WINAPI GetModuleFileNameHook(HMODULE, LPTSTR, DWORD);
	HANDLE WINAPI CreateMutexHook(LPSECURITY_ATTRIBUTES, BOOL, LPCTSTR);
	void Init();
}
