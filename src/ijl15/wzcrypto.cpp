#include "stdafx.h"
#include "wzcrypto.h"
#include "detours_util.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void detourWzRSAEncryptString()
{
	HMODULE hModule = LoadLibraryS("WZCrypto.dll");
	typedef int(__cdecl* WzRSAEncryptString)(char*, const char*, char*, char*);
	static auto _WzRSAEncryptString = (WzRSAEncryptString)(GetProcAddress(hModule, "WzRSAEncryptString"));
	WzRSAEncryptString Hook = [](char* key, const char* rn, char* text, char* result) -> int
	{
		memset(result, 0, 300);
		strcpy(result, text); // Copy without encryption
		cout << "WzRSAEncryptString " << text << endl;
		return 0;
	};
	cout << "Redirect WzRSAEncryptString\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_WzRSAEncryptString), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour WzRSAEncryptString" << endl;
		throw;
	}
}
