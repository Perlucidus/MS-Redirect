// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ijl15.h"
#include "winsock.h"
#include "settings.h"
#include "window.h"
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
		redirect_ijl_calls(); // Redirect ijl15 calls to original library
		redirect_winsock(); // Hook Connect/GetPeerName
		hook_window(); // Hook CreateWindowEx
		if (DISABLE_MUTEX)
			hook_mutex(); // Hook CreateMutex
	}
	catch (exception const& e) {
		cout << e.what() << endl;
	}
}
