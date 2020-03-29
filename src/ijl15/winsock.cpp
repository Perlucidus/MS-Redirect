#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock.h"
#include "detours_util.h"
#include "settings.h"
#include <iostream>
#include <stdexcept>

using namespace std;

WSPPROC_TABLE WSPProcTable;

int WINAPI GetPeerName(SOCKET s, struct sockaddr *name, LPINT namelen, LPINT lpErrno)
{
	int nRet = WSPProcTable.lpWSPGetPeerName(s, name, namelen, lpErrno);
	if (nRet == SOCKET_ERROR) {
		cout << "WSPGetPeerName Error " << *lpErrno << endl;
		return nRet;
	}
	char szAddr[50];
	DWORD dwLen = 50;
	WSAAddressToStringA((sockaddr*)name, *namelen, NULL, szAddr, &dwLen);
	sockaddr_in* service = (sockaddr_in*)name;
	u_short nPort = ntohs(service->sin_port);
	service->sin_addr.S_un.S_addr = inet_addr(NEXON_IP1);
	cout << "WSPGetPeerName from " << szAddr << " redirected to " << NEXON_IP1 << ":" << nPort << endl;
	return nRet;
}

int WINAPI Connect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno)
{
	char szAddr[50];
	DWORD dwLen = 50;
	WSAAddressToStringA((sockaddr*)name, namelen, NULL, szAddr, &dwLen);
	sockaddr_in* service = (sockaddr_in*)name;
	u_short nPort = ntohs(service->sin_port);
	service->sin_addr.S_un.S_addr = inet_addr(HOST_IP);
	cout << "WSPConnect from " << szAddr << " redirected to " << HOST_IP << ":" << nPort << endl;
	return WSPProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

void detourWSPStartup()
{
	HMODULE hModule = LoadLibraryS("MSWSOCK");
	static auto _WSPStartup = decltype(&WSPStartup)(GetProcAddress(hModule, "WSPStartup"));
	decltype(&WSPStartup) Hook = [](WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable) -> int
	{
		int ret = _WSPStartup(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
		WSPProcTable = *lpProcTable;
		lpProcTable->lpWSPConnect = Connect; // Redirect Connection
		lpProcTable->lpWSPGetPeerName = GetPeerName; // Redirect GetPeerName
		cout << "WSPProcTable@" << lpProcTable << endl;
		cout << "Redirect WSPConnect\t\t" << Connect << endl;
		cout << "Redirect WSPGetPeerName\t\t" << GetPeerName << endl;
		return ret;
	};
	cout << "Redirect WSPStartup\t\t" << Hook << endl;
	try {
		Detour(reinterpret_cast<void**>(&_WSPStartup), Hook);
	}
	catch (runtime_error e) {
		cout << "Failed to detour WSPStartup" << endl;
		throw;
	}
}
