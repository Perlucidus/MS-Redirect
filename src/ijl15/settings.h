#pragma once

#include <string>
#include <WinSock2.h>

constexpr const char* HOST_IP = "127.0.0.1";
constexpr const char* SEARCH_IP = "109.234.73.11";
constexpr const char* NEXON_IP1 = "203.116.196.8";
constexpr const char* NEXON_IP2 = "203.116.196.9";

constexpr const char* WINDOW_NAME = HOST_IP;

constexpr bool DISABLE_MUTEX = true;
const HANDLE FAKE_MUTEX_HANDLE = (HANDLE)123456;
