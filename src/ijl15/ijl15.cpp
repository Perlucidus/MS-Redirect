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
	ijlGetLibVersion_proc = GetProcAddress(hModule, "ijlGetLibVersion");
	ijlInit_proc = GetProcAddress(hModule, "ijlInit");
	ijlFree_proc = GetProcAddress(hModule, "ijlFree");
	ijlRead_proc = GetProcAddress(hModule, "ijlRead");
	ijlWrite_proc = GetProcAddress(hModule, "ijlWrite");
	ijlErrorStr_proc = GetProcAddress(hModule, "ijlErrorStr");
	cout << "Redirect ijlGetLibVersion\t" << ijlGetLibVersion_proc << endl;
	cout << "Redirect ijlInit\t\t" << ijlInit_proc << endl;
	cout << "Redirect ijlFree\t\t" << ijlFree_proc << endl;
	cout << "Redirect ijlRead\t\t" << ijlRead_proc << endl;
	cout << "Redirect ijlWrite\t\t" << ijlWrite_proc << endl;
	cout << "Redirect ijlErrorStr\t\t" << ijlErrorStr_proc << endl;
}
