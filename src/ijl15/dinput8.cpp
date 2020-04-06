#include "stdafx.h"
#include "dinput8.h"
#include "detours_util.h"
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

HRESULT WINAPI dinput8::DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	PVOID proc = detours::HookMap["dinput8.dll::DirectInput8Create"];
	auto _DirectInput8Create = decltype(&DirectInput8Create)(proc);
	HRESULT result = _DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	cout << "DirectInput8Create" << endl;
	return result;
}

HRESULT WINAPI dinput8::DllCanUnloadNow()
{
	PVOID proc = detours::HookMap["dinput8.dll::DllCanUnloadNow"];
	auto _DllCanUnloadNow = decltype(&DllCanUnloadNow)(proc);
	cout << "DllCanUnloadNow" << endl;
	return _DllCanUnloadNow();
}

HRESULT WINAPI dinput8::DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	PVOID proc = detours::HookMap["dinput8.dll::DllGetClassObject"];
	auto _DllGetClassObject = decltype(&DllGetClassObject)(proc);
	cout << "DllGetClassObject" << endl;
	return _DllGetClassObject(rclsid, riid, ppv);
}

HRESULT WINAPI dinput8::DllRegisterServer()
{
	PVOID proc = detours::HookMap["dinput8.dll::DllRegisterServer"];
	auto _DllRegisterServer = decltype(&DllRegisterServer)(proc);
	cout << "DllRegisterServer" << endl;
	return _DllRegisterServer();
}

HRESULT WINAPI dinput8::DllUnregisterServer()
{
	PVOID proc = detours::HookMap["dinput8.dll::DllUnregisterServer"];
	auto _DllUnregisterServer = decltype(&DllUnregisterServer)(proc);
	cout << "DllUnregisterServer" << endl;
	return _DllUnregisterServer();
}

void dinput8::Init()
{
	detours::Detour("dinput8.dll", "DirectInput8Create", &DirectInput8Create);
	detours::Detour("dinput8.dll", "DllCanUnloadNow", &DllCanUnloadNow);
	detours::Detour("dinput8.dll", "DllGetClassObject", &DllGetClassObject);
	detours::Detour("dinput8.dll", "DllRegisterServer", &DllRegisterServer);
	detours::Detour("dinput8.dll", "DllUnregisterServer", &DllUnregisterServer);
}
