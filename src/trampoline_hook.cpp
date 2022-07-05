#include "pch.h"
#include "trampoline_hook.h"

using namespace Hook;

TrampolineHook::TrampolineHook() {
	this->pSource = nullptr;
	this->pDestination = nullptr;
	this->dwLen = NULL;

	this->pGateway = nullptr;
}

bool TrampolineHook::initialize(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen) {
	this->pSource = pSource;
	this->pDestination = pDestination;
	this->dwLen = dwLen;

	this->pGateway = nullptr;

	// parameter valid?
	return TrampolineHook::CheckAllowed(this->pSource, this->pDestination, this->dwLen);
}

BYTE* TrampolineHook::Enable() {

	// create gateway
	this->pGateway = (BYTE*)VirtualAlloc(0, this->dwLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!this->pGateway) {
		return nullptr;
	}

	// write stolen bytes to gateway
	memcpy_s(this->pGateway, this->dwLen, this->pSource, this->dwLen);

#ifdef _WIN64
	// absolute jmp + ...
	*(uintptr_t*)(this->pGateway + this->dwLen) = DETOUR_JMP_INSTRUCTION;

	// ... + absolute address
	*(uintptr_t*)((uintptr_t)this->pGateway + this->dwLen + 8) = *this->pGateway;
#else
	// calculate relative address for the jmp instruction
	uintptr_t gatewayRelativeAddress = this->pSource - this->pGateway - 5;

	// jmp + ...
	*(this->pGateway + this->dwLen) = DETOUR_JMP_INSTRUCTION;

	// ... + relativeAddress
	*(uintptr_t*)((uintptr_t)this->pGateway + this->dwLen + 1) = gatewayRelativeAddress;
#endif
	
	if (this->Detour(this->pSource, this->pDestination, this->dwLen)) {
		return this->pGateway;
	}

	return nullptr;
}

bool TrampolineHook::Disable() {
	// TODO: rewrite stolen bytes
	return true;
}

bool TrampolineHook::Detour(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen) {

	// parameter valid?
	if (!TrampolineHook::CheckAllowed(pSource, pDestination, dwLen)) {
		return false;
	}

	// save current protection, to restore it after hooking
	DWORD currentProtection;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &currentProtection);

	// clear bytes that should be used for the jmp instruction
	memset(pSource, 0x90, dwLen);

#ifdef _WIN64
	// absolute jmp + ...
	*(uintptr_t*)pSource = DETOUR_JMP_INSTRUCTION;

	// ... + absolute address
	*(uintptr_t*)((uintptr_t)pSource + 8) = (uintptr_t)pDestination;
#else
	// calculate relative address for the jmp instruction
	uintptr_t relativeAddress = pDestination - pSource - 5;

	// jmp + ...
	*(uintptr_t*)pSource = DETOUR_JMP_INSTRUCTION;

	// ... + relativeAddress
	*(uintptr_t*)(pSource + 1) = relativeAddress;
#endif

	// restore protection
	VirtualProtect(pSource, dwLen, currentProtection, &currentProtection);

	return true;
}

bool TrampolineHook::CheckAllowed(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen) {
	if (!pSource || !pDestination) {
		return false;
	}

	// check length for relative jmp instruction
	if (dwLen < DETOUR_MIN_LENGTH) {
		return false;
	}

	return true;
}
