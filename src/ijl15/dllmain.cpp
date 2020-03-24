// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ijl15.h"
#include "winsock.h"
#include "settings.h"
#include "winapi.h"
#include "hshield.h"
#include "wzcrypto.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

void Initialize();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Initialize();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void Initialize() {
	// Start debugging console
	AllocConsole();
	DWORD pid = GetCurrentProcessId();
	AttachConsole(pid);
	freopen("CON", "w", stdout);
	ostringstream ss;
	ss << "PID: " << pid;
	SetConsoleTitleA(ss.str().c_str());
	try {
		srand(pid);
		redirect_ijl_calls(); // Redirect ijl15 calls to original library
		detourWSPStartup(); // Detour Connect/GetPeerName
		detourLoadLibrary(); // Detour LoadLibrary
		detourCreateWindowEx(); // Detour CreateWindowEx
		if (DISABLE_MUTEX)
			detourCreateMutex(); // Detour CreateMutex
		//hook_hshield(); // HShield emulator (not completed)
		detourWzRSAEncryptString(); // Detour WzRSAEncryptString
	}
	catch (exception const& e) {
		cout << e.what() << endl;
	}
}
