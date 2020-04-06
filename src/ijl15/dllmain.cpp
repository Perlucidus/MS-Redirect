// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ijl15.h"
#include "winsock.h"
#include "settings.h"
#include "winapi.h"
#include "hshield.h"
#include "wzcrypto.h"
#include "dinput8.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

void Attach();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		Attach();
		DisableThreadLibraryCalls(hModule);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void Attach() {
#if _DEBUG
	// Start debugging console
	AllocConsole();
	DWORD pid = GetCurrentProcessId();
	AttachConsole(pid);
	FILE* f = nullptr;
	freopen_s(&f, "CON", "w", stdout);
	ostringstream ss;
	ss << "PID: " << pid;
	SetConsoleTitleA(ss.str().c_str());
#endif
	try {
		ijl15::HijackAPICalls(); // Redirect ijl15 calls to original library
		hshield::BypassHShield();
		net::Init();
		wz::Init();
		dinput8::Init();
		winapi::Init();
	}
	catch (exception const& e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "FATAL ERROR" << endl;
	}
}
