#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2spi.h>
#include <string>

namespace net {
	int WSPAPI GetPeerNameHook(SOCKET, struct sockaddr*, LPINT, LPINT);
	int WSPAPI WSPConnectHook(SOCKET, const struct sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS, LPINT);
	int WSPAPI WSPStartupHook(WORD, LPWSPDATA, LPWSAPROTOCOL_INFOW, WSPUPCALLTABLE, LPWSPPROC_TABLE);
	void Init();
}
