#include "hook.h"

// also called detour - we simply overwrite some instructions of the targets process with an unconditional
// jump (JMP) to manipulate the program flow to our needs.
bool memory::Hook(uintptr_t pSource, uintptr_t pDestination, uintptr_t pLength, uintptr_t pSourcePadding) {
	if (!pSource || !pDestination || pLength < DETOUR_MIN_LENGTH) {
		return false;
	}

	// safe current protection
	DWORD memoryProtection;
	VirtualProtect((uintptr_t*)(pSource + pSourcePadding), pLength, PAGE_EXECUTE_READWRITE, &memoryProtection);

	// clear bytes that should be used for the JMP instruction
	memset((uintptr_t*)(pSource + pSourcePadding), ASM_INSTRUCTION_NOP, pLength);
	
#ifdef _WIN64
	// windows 64bit: we have to do an absolute jmp to the destination address
	// absolute JMP instruction + ...
	*(uintptr_t*)(pSource + pPadding) = ASM_INSTRUCTION_JMP;

	// ... + absolute address
	*(uintptr_t*)(pSource + pPadding + 8) = pDestination;
#else
	// windows 32bit: we have to do a relative jmp to the destination address
	// calculate the relative destination address 
	uintptr_t relativeAddress = pDestination - pSource - 5;

	// relative JMP instruction + ...
	*(uintptr_t*)(pSource + pSourcePadding) = ASM_INSTRUCTION_JMP;

	// ... + the relative address (jmp "to")
	*(uintptr_t*)(pSource + pSourcePadding + 1) = relativeAddress;
#endif

	// restore the protection
	VirtualProtect((uintptr_t*)(pSource + pSourcePadding), pLength, memoryProtection, &memoryProtection);
	return true;
}
