#include "memory.h"

bool memory::PatchBytes(BYTE* destination, BYTE* source, unsigned int size) {
	DWORD protection;
	if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
		return false;
	}
	if (memcpy_s(destination, size, source, size) != 0) {
		return false;
	}
	return VirtualProtect(destination, size, protection, &protection);
}

void memory::ReadBytes(BYTE* source, BYTE* destination, unsigned int size) {
	DWORD protection;
	VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &protection);
	memcpy_s(destination, size, source, size);
	VirtualProtect(source, size, protection, &protection);
}

void memory::NopBytes(BYTE* destination, unsigned int size) {
	DWORD protection;
	VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection);
	memset(destination, 0x90, size);
	VirtualProtect(destination, size, protection, &protection);
}

uintptr_t memory::FindDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		addr = *(uintptr_t*)addr;

		if (!addr) {
			return addr;
		}

		addr += offsets[i];
	}
	return addr;
}