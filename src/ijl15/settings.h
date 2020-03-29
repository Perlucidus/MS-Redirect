#pragma once

#include <string>
#include <WinSock2.h>

namespace net {
	constexpr const char* HOST_IP = "127.0.0.1";
	constexpr const char* SEARCH_IP = "109.234.73.11";
	constexpr const char* NEXON_IP1 = "203.116.196.8";
	constexpr const char* NEXON_IP2 = "203.116.196.9";
}

namespace config {
	constexpr const char* CONFIG_PATH = "./config.ini";
}

namespace winapi {
	constexpr const char* WINDOW_NAME = net::HOST_IP;

	constexpr bool DISABLE_MUTEX = true;
	const HANDLE INVALID_MUTEX_HANDLE = (HANDLE)123456;
	const HANDLE INVALID_FILE_HANDLE = (HANDLE)123456;
}

namespace memory {
	namespace crc {
		constexpr DWORD MSCRC_MAIN = 0x01059793; //Checks MSCRC1 and MSCRC2
		constexpr DWORD MSCRC1 = 0x01148B7D;
		constexpr DWORD MSCRC2 = 0x00B18B04;
		constexpr DWORD MSCRC_START = 0x00401000; //Shouldn't ever change
	}

	namespace hack {
		constexpr DWORD GAME_WIDTH = 0x00A7ABB9;
		constexpr DWORD GAME_HEIGHT = 0x00A7ABBE;
	}
}
