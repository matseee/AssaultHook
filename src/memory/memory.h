#pragma once
#include <windows.h>
#include <vector>

namespace memory {
	bool PatchBytes(BYTE* destination, BYTE* source, unsigned int size);
	void ReadBytes(BYTE* source, BYTE* destination, unsigned int size);
	void NopBytes(BYTE* destination, unsigned int size);
	uintptr_t FindDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets);
}