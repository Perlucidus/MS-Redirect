#pragma once

#include <string>
#include <WinSock2.h>

namespace net {
	constexpr LPCTSTR HOST_IP = "77.138.87.25";
	constexpr LPCTSTR SEARCH_IP = "109.234.73.11";
	constexpr LPCTSTR NEXON_IP1 = "203.116.196.8";
	constexpr LPCTSTR NEXON_IP2 = "203.116.196.9";
}

namespace config {
	constexpr LPCTSTR CONFIG_PATH = "./config.ini";
}

namespace winapi {
	constexpr LPCTSTR WINDOW_NAME = net::HOST_IP;
	constexpr bool DISABLE_MUTEX = true;
}

namespace game {
	namespace crc {
		constexpr DWORD CRC_MAIN = 0x01059793; //Checks MSCRC1 and MSCRC2
		constexpr DWORD CRC1 = 0x01148B7D;
		constexpr DWORD CRC2 = 0x00B18B04;
	}

	constexpr DWORD GAME_WIDTH = 0x00A7ABB9;
	constexpr DWORD GAME_HEIGHT = 0x00A7ABBE;
}
