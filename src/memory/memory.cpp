#include "memory.h"

void memory::Patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD memoryProtection;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &memoryProtection);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, memoryProtection, &memoryProtection);
}

void memory::Nop(BYTE* dst, unsigned int size) {
	DWORD memoryProtection;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &memoryProtection);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, memoryProtection, &memoryProtection);
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