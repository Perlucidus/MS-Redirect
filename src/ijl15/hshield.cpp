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

int __stdcall hshield::AhnHS_ServiceDispatchHook(unsigned int code, void** params, unsigned int* error)
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

BOOL WINAPI hshield::CreateProcessHook(LPCTSTR lpApplicationName, LPTSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	auto _CreateProcess = decltype(&CreateProcess)(detours::HookMap["KERNEL32::CreateProcess"]);
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
			return _CreateProcess(lpszFilePath, const_cast<char*>("svchost.exe"),
				NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, lpStartupInfo, lpProcessInformation);
		}
	}
	cout << "CreateProcess " << lpCommandLine << endl;
	return _CreateProcess(lpApplicationName, lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

void hshield::BypassHShield()
{
	detours::Detour(CreateProcess, CreateProcessHook, "KERNEL32::CreateProcess");
	//detours::Detour("HShield/ehsvc.dll", reinterpret_cast<LPCTSTR>(10), AhnHS_ServiceDispatchHook);
}
