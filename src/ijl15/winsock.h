#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2spi.h>
#include <string>

int WINAPI GetPeerName(SOCKET s, struct sockaddr *name, LPINT namelen, LPINT lpErrno);
int WINAPI Connect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno);
void redirect_winsock();
