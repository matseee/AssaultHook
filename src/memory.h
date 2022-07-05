#pragma once
#include "pch.h"
#include "framework.h"

namespace memory {
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void PatchExternal(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);

	void Nop(BYTE* dst, unsigned int size);
	void NopExternal(BYTE* dst, unsigned int size, HANDLE hProcess);

	uintptr_t FindDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets);
}