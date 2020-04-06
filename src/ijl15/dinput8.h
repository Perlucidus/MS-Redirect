#pragma once

#include <ShlObj.h>

namespace dinput8 {
	HRESULT WINAPI DirectInput8Create(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
	HRESULT WINAPI DllCanUnloadNow();
	HRESULT WINAPI DllGetClassObject(REFCLSID, REFIID, LPVOID*);
	HRESULT WINAPI DllRegisterServer();
	HRESULT WINAPI DllUnregisterServer();

	void Init();
}
