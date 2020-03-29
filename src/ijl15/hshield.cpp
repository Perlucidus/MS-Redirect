#include "stdafx.h"
#include "hshield.h"
#include "detours_util.h"
#include <thread>
#include <strsafe.h>
#include <iostream>
#include <stdexcept>

using namespace std;

typedef int(__cdecl* AhnHS_ServiceDispatch)(unsigned int code, void** params, unsigned int* error);
typedef int(__cdecl* AhnHS_Callback)(int lCode, int lParamSize, void* pParam);

bool heartbeat = false;
bool service = false;
AhnHS_Callback _AhnHS_Callback = 0;

int __stdcall hshield::AhnHS_ServiceDispatch_Hook(unsigned int code, void** params, unsigned int* error)
{
		*error = HS_ERR_OK;
		switch (code) {
		case HS_Initialize:
			cout << "AhnHS_ServiceDispatch: HS_Initialize" << endl;
			if (heartbeat) {
				*error = HS_ERR_ALREADY_INITIALIZED;
				return 0;
			}
			_AhnHS_Callback = reinterpret_cast<AhnHS_Callback>(*params);
			heartbeat = true; //TODO emulate heartbeat
			if (!heartbeat) {
				*error = HS_ERR_UNKNOWN;
				return 0;
			}
			break;
		case HS_StartService:
			cout << "AhnHS_ServiceDispatch: HS_StartService" << endl;
			if (service)
				*error = HS_ERR_ALREADY_SERVICE_RUNNING;
			else
				service = true;
			break;
		case HS_MakeResponse:
			cout << "AhnHS_ServiceDispatch: HS_MakeResponse" << endl;
			if (!heartbeat) {
				*error = HS_ERR_NOT_INITIALIZED;
				return 1;
			}
			if (false) { //heartbeat_emulator->make_response(params)
				*error = HS_ERR_UNKNOWN;
				return 1;
			}
			break;
		case HS_IsModuleSecure:
			cout << "AhnHS_ServiceDispatch: HS_IsModuleSecure" << endl;
			*error = HS_ERR_UNKNOWN;
			break;
		case HS_CheckHackShieldRunningStatus:
			cout << "AhnHS_ServiceDispatch: HS_CheckHackShieldRunningStatus" << endl;
			if (_AhnHS_Callback) {
				thread callback(_AhnHS_CallbackProc);
				callback.detach();
				cout << "Detached callback thread for AhnHS_Callback" << endl;
			}
			break;
		}
		return 1;
}

int __stdcall hshield::_AhnHS_CallbackProc()
{
	if (_AhnHS_Callback)
	{
		try
		{
			while (unsigned int status = HS_RUNNING_STATUS_CHECK_MONITORING_THREAD)
			{
				_AhnHS_Callback(AHNHS_ACTAPC_STATUS_HACKSHIELD_RUNNING, sizeof(unsigned int), &status);
				this_thread::sleep_for(chrono::seconds(25));
			}
		}
		catch (exception const& ex)
		{
			cout << "_AhnHS_CallbackProc: " << ex.what() << endl;
		}
	}
	return 0;
}

void hshield::DetourAhnHS_ServiceDispatch() {
	HMODULE hModule = detours::LoadLibraryS("HShield/ehsvc.dll");
	static auto _AhnHS_ServiceDispatch = (AhnHS_ServiceDispatch)(GetProcAddress(hModule, (LPCSTR)10));
	cout << "Redirect AhnHS_ServiceDispatch\t" << AhnHS_ServiceDispatch_Hook << endl;
	try {
		detours::Detour(reinterpret_cast<void**>(&_AhnHS_ServiceDispatch), AhnHS_ServiceDispatch_Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour AhnHS_ServiceDispatch" << endl;
		throw;
	}
}

void hshield::DetourCreateProcess()
{
	HMODULE hModule = detours::LoadLibraryS("KERNEL32");
	static auto _CreateProcessA = decltype(&CreateProcessA)(GetProcAddress(hModule, "CreateProcessA"));
	decltype(&CreateProcessA) Hook = [](
		LPCSTR lpApplicationName,
		LPSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation
		) -> BOOL
	{
		if (lpCommandLine) {
			bool hsupdate = strstr(lpCommandLine, "HSUpdate.exe");
			bool autoup = strstr(lpCommandLine, "autoup.exe");
			if (hsupdate || autoup) {
				if (hsupdate) {
					HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE,
						"Global\\EF81BA4B-4163-44f5-90E2-F05C1E49C12D");
					SetEvent(hEvent);
					CloseHandle(hEvent);
				}
				char lpszFilePath[256];
				StringCchCopy(lpszFilePath + GetSystemDirectory(lpszFilePath, 256), 256, "\\svchost.exe");
				return _CreateProcessA(lpszFilePath, const_cast<char*>("svchost.exe"),
					NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, lpStartupInfo, lpProcessInformation);
			}
		}
		cout << "CreateProcess " << lpCommandLine << endl;
		return _CreateProcessA(
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation
		);
	};
	cout << "Redirect CreateProcessA\t\t" << Hook << endl;
	try {
		detours::Detour(reinterpret_cast<void**>(&_CreateProcessA), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour CreateProcessA" << endl;
		throw;
	}
}

void hshield::BypassHShield()
{
	DetourCreateProcess();
}
