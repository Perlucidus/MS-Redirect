#include "stdafx.h"
#include "wzcrypto.h"
#include "detours_util.h"
#include <iostream>
#include <stdexcept>

using namespace std;

int wz::WzRSAEncryptStringHook(char* key, const char* rn, char* text, char* result) {
	memset(result, 0, 300);
	memcpy(result, text, 300); // Copy without encryption
	cout << "WzRSAEncryptString " << text << endl;
	return 0;
}

void wz::Init()
{
	detours::Detour("WzCrypto.dll", "WzRSAEncryptString", WzRSAEncryptStringHook);
}
