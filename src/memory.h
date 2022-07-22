#pragma once
#include "pch.h"
#include "framework.h"

namespace memory {
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t FindDMAAddress(uintptr_t ptr, std::vector<unsigned int> offsets);
}