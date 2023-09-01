#include "pch.h"
#include "trampoline_hook.h"

TrampolineHook::TrampolineHook(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen) {
	this->pSource = pSource;
	this->pDestination = pDestination;
	this->dwLen = dwLen;
	this->pGateway = nullptr;
}

BYTE* TrampolineHook::Create() {
	if (!this->CreateGateway()) {
		return nullptr;
	}

	if (!this->Detour()) {
		return nullptr;
	}

	Log::Debug() << "TrampolineHook::Create(): Trampoline created successfully ..." << std::endl;
	return this->pGateway;
}

bool TrampolineHook::Destroy() {
	// TODO: rewrite stolen bytes
	return true;
}

bool TrampolineHook::CreateGateway() {
	this->pGateway = (BYTE*)VirtualAlloc(0, this->dwLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!this->pGateway) {
		Log::Error() << "TrampolineHook::CreateGateway(): VirtualAlloc() failed ..." << std::endl;
		return false;
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

	return true;
}

bool TrampolineHook::Detour() {
	// parameter valid?
	if (!this->CheckAllowed()) {
		return false;
	}

	// save current protection, to restore it after hooking
	DWORD currentProtection;
	VirtualProtect(this->pSource, this->dwLen, PAGE_EXECUTE_READWRITE, &currentProtection);

	// clear bytes that should be used for the jmp instruction
	memset(this->pSource, 0x90, this->dwLen);

#ifdef _WIN64
	// absolute jmp + ...
	*(uintptr_t*)this->pSource = DETOUR_JMP_INSTRUCTION;

	// ... + absolute address
	*(uintptr_t*)((uintptr_t)this->pSource + 8) = (uintptr_t)pDestination;
#else
	// calculate relative address for the jmp instruction
	uintptr_t relativeAddress = this->pDestination - this->pSource - 5;

	// jmp + ...
	*(uintptr_t*)this->pSource = DETOUR_JMP_INSTRUCTION;

	// ... + relativeAddress
	*(uintptr_t*)(this->pSource + 1) = relativeAddress;
#endif

	// restore protection
	VirtualProtect(this->pSource, this->dwLen, currentProtection, &currentProtection);

	return true;
}

bool TrampolineHook::CheckAllowed() {
	if (!this->pSource || !this->pDestination) {
		Log::Error() << "TrampolineHook::CheckAllowed(): \"pSource\" or \"pDestination\" not set ..." << std::endl;
		return false;
	}

	// check length for relative jmp instruction
	if (this->dwLen < DETOUR_MIN_LENGTH) {
		Log::Error() << "TrampolineHook::CheckAllowed(): \"dwLen\" is smaller than " << DETOUR_MIN_LENGTH << " ..." << std::endl;
		return false;
	}

	return true;
}
