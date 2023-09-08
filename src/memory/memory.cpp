#include "memory.h"

addr memory::AllocateMemory(addr source, uint size) {
	return (addr)VirtualAlloc((addr*)source, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
}

bool memory::FreeMemory(addr source) {
	return VirtualFree((addr*)source, 0, MEM_RELEASE) == 0;
}

bool memory::PatchBytes(addr* destination, addr* source, uint size) {
	ulong protection;
	if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
		return false;
	}
	if (memcpy_s((addr*)destination, size, source, size) != 0) {
		return false;
	}
	return VirtualProtect(destination, size, protection, &protection);
}

bool memory::ReadBytes(addr* source, addr* destination, uint size) {
	ulong protection;
	if (!VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &protection)) {
		return false;
	}
	if (memcpy_s((addr*)destination, size, source, size) != 0) {
		return false;
	}
	return VirtualProtect(source, size, protection, &protection);
}

bool memory::NopBytes(addr* destination, uint size) {
	ulong protection;
	if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
		return false;
	}
	memset(destination, MEMORY_ASM_INSTRUCTION_NOP, size);
	return VirtualProtect(destination, size, protection, &protection) != 0;
}

addr memory::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
	addr address = baseAddress;
	for (uint i = 0; i < offsets.size(); ++i) {
		address = *(addr*)address;

		if (!address) {
			return address;
		}

		address += offsets[i];
	}
	return address;
}