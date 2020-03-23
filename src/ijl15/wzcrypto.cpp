#include "stdafx.h"
#include "wzcrypto.h"
#include "detours_util.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void Hook_WriteStageLogA()
{
	HMODULE hModule = LoadLibraryA("WZCrypto.dll");
	if (!hModule)
		throw exception("Could not load WZCrypto.dll");
	typedef int(__cdecl* WzRSAEncryptString)(char*, const char*, char*, char*);
	static auto _WzRSAEncryptString = (WzRSAEncryptString)(GetProcAddress(hModule, "WzRSAEncryptString"));
	WzRSAEncryptString Hook = [](char* key, const char* rn, char* text, char* result) -> int
	{
		memset(result, 0, 300);
		strcpy(result, text); // Copy without encryption
		cout << "WzRSAEncryptString " << text << endl;
		return 0;
	};
	cout << "Redirecting WzRSAEncryptString to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_WzRSAEncryptString), Hook))
		throw exception("Failed to hook WzRSAEncryptString");
}

void hook_wz_crypto()
{
	Hook_WriteStageLogA();
}
