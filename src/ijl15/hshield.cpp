#include "stdafx.h"
#include "hshield.h"
#include "detours_util.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void hook_hshield()
{
	HMODULE hModule = LoadLibraryA("HShield/ehsvc.dll");
	if (!hModule)
		throw exception("Could not load HShield/ehsvc.dll");
	typedef int(__cdecl* f)(int a, int b, int c);
	static auto _f = (f)(GetProcAddress(hModule, "\n"));
	f Hook = [](int a, int b, int c) -> int
	{
		cout << "HSHield/ehscv.dll call with " << a << ", " << b << ", " << c << endl;
		return 0;
	};
	cout << "Redirecting HSHield/ehscv to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_f), Hook))
		throw exception("Failed to hook HSHield/ehscv");
}
