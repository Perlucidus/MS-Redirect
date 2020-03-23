#include "stdafx.h"
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
	if (nPort == PORT)
	{
		service->sin_addr.S_un.S_addr = inet_addr(NEXON_IP);
		cout << "WSPGetPeerName redirected to " << NEXON_IP << endl;
	}
	else
		cout << "WSPGetPeerName from " << szAddr << endl;
	return nRet;
}

int WINAPI Connect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno)
{
	char szAddr[50];
	DWORD dwLen = 50;
	WSAAddressToStringA((sockaddr*)name, namelen, NULL, szAddr, &dwLen);
	sockaddr_in* service = (sockaddr_in*)name;
	if (strstr(szAddr, SEARCH_IP)) // Contains SEARCH_IP
	{
		service->sin_addr.S_un.S_addr = inet_addr(HOST_IP);
		cout << "WSPConnect redirected to " << HOST_IP << endl;
	}
	else
		cout << "WSPConnect from " << szAddr;
	return WSPProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

void redirect_winsock()
{
	HMODULE hModule = LoadLibraryA("MSWSOCK");
	if (!hModule)
		throw exception("Could not load MSWSOCK");
	cout << "MSWSOCK loaded with hModule: " << hModule << endl;
	static auto _WSPStartup = decltype(&WSPStartup)(GetProcAddress(hModule, "WSPStartup"));
	decltype(&WSPStartup) Hook = [](WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable) -> int
	{
		int ret = _WSPStartup(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
		WSPProcTable = *lpProcTable;
		cout << "found WSPProcTable at " << lpProcTable << endl;
		lpProcTable->lpWSPConnect = Connect; // Redirect Connection
		cout << "Redirected WSPConnect to " << Connect << endl;
		lpProcTable->lpWSPGetPeerName = GetPeerName; // Redirect GetPeerName
		cout << "Redirected WSPGetPeerName to " << GetPeerName << endl;
		return ret;
	};
	cout << "Redirecting WSPStartup to " << Hook << endl;
	if (!SetHook(true, reinterpret_cast<void**>(&_WSPStartup), Hook))
		throw exception("Failed to hook WSPStartup");
}
