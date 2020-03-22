#pragma once

extern "C" {
	__declspec(dllexport) void ijlGetLibVersion();
	__declspec(dllexport) void ijlInit();
	__declspec(dllexport) void ijlFree();
	__declspec(dllexport) void ijlRead();
	__declspec(dllexport) void ijlWrite();
	__declspec(dllexport) void ijlErrorStr();
}

void redirect_ijl_calls();
