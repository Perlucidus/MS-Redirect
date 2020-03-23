#include "stdafx.h"
#include "hshield.h"
#include "detours_util.h"
#include <thread>
#include <iostream>
#include <stdexcept>

using namespace std;

typedef int(__cdecl* AhnHS_ServiceDispatch)(unsigned int code, void** params, unsigned int* error);
typedef int(__cdecl* AhnHS_Callback)(int lCode, int lParamSize, void* pParam);

bool heartbeat = false;
bool service = false;
AhnHS_Callback _AhnHS_Callback = 0;

int __stdcall _AhnHS_CallbackProc(void)
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

void hook_hshield()
{
	HMODULE hModule = LoadLibraryA("HShield/ehsvc.dll");
	if (!hModule)
		throw exception("Could not load HShield/ehsvc.dll");
	static auto _AhnHS_ServiceDispatch = (AhnHS_ServiceDispatch)(GetProcAddress(hModule, (LPCSTR)10));
	AhnHS_ServiceDispatch Hook = [](unsigned int code, void** params, unsigned int* error) -> int
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
	};
	cout << "Redirect AhnHS_ServiceDispatch\t" << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_AhnHS_ServiceDispatch), Hook))
		throw exception("Failed to hook AhnHS_ServiceDispatch");
}
