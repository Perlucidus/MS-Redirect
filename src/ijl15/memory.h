#pragma once

namespace memory {
	namespace crc {
		void BypassMSCRC();
	}

	namespace hack {
		void Hack();
	}

	void SetJMP(void* pos, void* target, int padding = 0);
}
