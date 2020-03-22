// ijl15.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ijl15.h"
#include <Windows.h>
#include <iostream>
#include <stdexcept>

using namespace std;

FARPROC
ijlGetLibVersion_proc,
ijlInit_proc,
ijlFree_proc,
ijlRead_proc,
ijlWrite_proc,
ijlErrorStr_proc;

void ijlGetLibVersion()
{
	__asm 	 jmp dword ptr[ijlGetLibVersion_proc]
}

void ijlInit()
{
	__asm 	 jmp dword ptr[ijlInit_proc]
}

void ijlFree()
{
	__asm 	 jmp dword ptr[ijlFree_proc]
}

void ijlRead()
{
	__asm 	 jmp dword ptr[ijlRead_proc]
}

void ijlWrite()
{
	__asm 	 jmp dword ptr[ijlWrite_proc]
}

void ijlErrorStr()
{
	__asm 	 jmp dword ptr[ijlErrorStr_proc]
}

void redirect_ijl_calls()
{
	HMODULE hModule = LoadLibraryA("ijl15");
	if (!hModule)
		throw exception("Failed to load ijl15");
	cout << "ijl15 loaded with hModule " << hModule << endl;
	ijlGetLibVersion_proc = GetProcAddress(hModule, "ijlGetLibVersion");
	ijlInit_proc = GetProcAddress(hModule, "ijlInit");
	ijlFree_proc = GetProcAddress(hModule, "ijlFree");
	ijlRead_proc = GetProcAddress(hModule, "ijlRead");
	ijlWrite_proc = GetProcAddress(hModule, "ijlWrite");
	ijlErrorStr_proc = GetProcAddress(hModule, "ijlErrorStr");
	cout << "Redirected ijlGetLibVersion to " << ijlGetLibVersion_proc << endl;
	cout << "Redirected ijlInit to " << ijlInit_proc << endl;
	cout << "Redirected ijlFree to " << ijlFree_proc << endl;
	cout << "Redirected ijlRead to " << ijlRead_proc << endl;
	cout << "Redirected ijlWrite to " << ijlWrite_proc << endl;
	cout << "Redirected ijlErrorStr to " << ijlErrorStr_proc << endl;
}
