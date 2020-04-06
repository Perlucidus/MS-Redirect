#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock.h"
#include "detours_util.h"
#include "settings.h"
#include <stdexcept>
#include <iostream>

using namespace std;

WSPPROC_TABLE WSPProcTable;

int WSPAPI net::GetPeerNameHook(SOCKET s, struct sockaddr *name, LPINT namelen, LPINT lpErrno)
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

int WSPAPI net::WSPConnectHook(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno)
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

int WSPAPI net::WSPStartupHook(WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable)
{
	PVOID proc = detours::HookMap["MSWSOCK::WSPStartup"];
	auto _WSPStartup = decltype(&WSPStartup)(proc);
	int result = _WSPStartup(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
	WSPProcTable = *lpProcTable;
	lpProcTable->lpWSPConnect = WSPConnectHook; //Redirect Connection
	lpProcTable->lpWSPGetPeerName = GetPeerNameHook; //Redirect GetPeerName
	cout << "WSPProcTable@" << lpProcTable << endl;
	cout << "Redirect WSPConnect\t\t" << WSPConnectHook << endl;
	cout << "Redirect WSPGetPeerName\t\t" << GetPeerNameHook << endl;
	return result;
};

void net::Init()
{
	detours::Detour("MSWSOCK", "WSPStartup", WSPStartupHook);
}
