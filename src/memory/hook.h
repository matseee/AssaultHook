#pragma once
#include <Windows.h>

#define ASM_INSTRUCTION_NOP 0x90

#ifdef _WIN64
#define DETOUR_MIN_LENGTH		16
#define ASM_INSTRUCTION_JMP		0x08EB0000000215FF
#else
#define DETOUR_MIN_LENGTH		5
#define ASM_INSTRUCTION_JMP		0xE9
#endif

namespace memory {
	bool Hook(uintptr_t pSource, uintptr_t pDestination, uintptr_t pLength, uintptr_t pSourcePadding = 0);
}
