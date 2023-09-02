#include "trampoline_hook.h"

TrampolineHook::TrampolineHook(const char* cModuleName, const char* cProcName, uintptr_t pDestination, uintptr_t dwLen) 
	: TrampolineHook(GetModuleHandleA(cModuleName), cProcName, pDestination, dwLen) { }

TrampolineHook::TrampolineHook(HMODULE hModule, const char* cProcName, uintptr_t pDestination, uintptr_t dwLen) 
	: TrampolineHook((uintptr_t)GetProcAddress(hModule, cProcName), pDestination, dwLen) { }

TrampolineHook::TrampolineHook(uintptr_t pSource, uintptr_t pDestination, uintptr_t dwLen) {
	this->pSource = pSource;
	this->pDestination = pDestination;
	this->dwLen = dwLen;
	this->pGateway = NULL;

	this->Create();
}

TrampolineHook::~TrampolineHook() {
	this->Destroy();
}

// creates the detour (source -> destination) when a gateway is available
bool TrampolineHook::Activate() {
	if (!this->isGatewayCreated || !this->pGateway) {
		Log::Error() << "TrampolineHook::Activate(): Gateway is not created ..." << Log::Endl;
		return false;
	}

	if (!this->CreateDetour()) {
		return false;
	}

	Log::Info() << "TrampolineHook::Activate(): Activation of TrampolineHook successful ..." << Log::Endl;
	return true;
}

// only deactivate the detour (source -> destination)
bool TrampolineHook::Deactivate() {
	if (!this->isDetourCreated) {
		Log::Error() << "TrampolineHook::Deactivate(): Detour is not active ..." << Log::Endl;
		return false;
	}

	Log::Info() << "TrampolineHook::Deactivate(): Deactivation of TrampolineHook successful ..." << Log::Endl;
	return this->ResetDetour();
}

uintptr_t TrampolineHook::GetGateway() {
	return this->pGateway;
}

// deactivate detour and release allocated memory for the gateway
void TrampolineHook::Destroy() {
	if (!this->ResetDetour()) {
		return;
	}

	if (!this->ReleaseGateway()) {
		return;
	}
	Log::Info() << "TrampolineHook::Destroy(): Trampoline destroyed successfully ..." << Log::Endl;
}

void TrampolineHook::Create() {
	if (!this->CheckParameterValid()) {
		return;
	}

	if (!this->CreateGateway()) {
		return;
	}
}

bool TrampolineHook::CheckParameterValid() {
	if (!this->pSource || !this->pDestination) {
		Log::Error() << "TrampolineHook::CheckParameterValid(): \"pSource\" or \"pDestination\" not set ..." << Log::Endl;
		return false;
	}

	// check length for relative jmp instruction
	if (this->dwLen < DETOUR_MIN_LENGTH) {
		Log::Error() << "TrampolineHook::CheckParameterValid(): \"dwLen\" is smaller than " << DETOUR_MIN_LENGTH << " ..." << Log::Endl;
		return false;
	}

	return true;
}

// creates the gateway. it allocates a memory page, copies the stolen bytes
// and creates a detour towards the destination function. 
bool TrampolineHook::CreateGateway() {
	// allocate memory for the gateway
	this->pGateway = (uintptr_t)VirtualAlloc(0, this->dwLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!this->pGateway) {
		Log::Error() << "TrampolineHook::CreateGateway(): VirtualAlloc() failed ..." << Log::Endl;
		return false;
	}

	// backup the stolen bytes
	if (memcpy_s(this->stolenBytes, this->dwLen, (uintptr_t*)this->pSource, this->dwLen) != 0) {
		Log::Error() << "TrampolineHook::CreateGateway(): Backup of the stolen bytes failed ..." << Log::Endl;
		return false;
	}

	// write the stolen bytes to the previously allocated memory
	if (memcpy_s((uintptr_t*)this->pGateway, this->dwLen, (uintptr_t*)this->pSource, this->dwLen) != 0) {
		Log::Error() << "TrampolineHook::CreateGateway(): Writing stolen bytes to gateway failed ..." << Log::Endl;
		return false;
	}

	// create the detour towards the original hooked function
	if (!hook::Detour(this->pGateway, this->pSource, this->dwLen, this->dwLen)) {
		Log::Error() << "TrampolineHook::CreateGateway(): Detour from gateway to source failed ..." << Log::Endl;
		return false;
	}
	
	Log::Debug() << "TrampolineHook::CreateGateway(): \"" << (void*)this->pGateway << "\" => \"" << (void*)this->pSource << "\" successful ..." << Log::Endl;
	return this->isGatewayCreated = true;
}

// creates a detour from the hooked (source) function to our own (destination) function.
bool TrampolineHook::CreateDetour() {
	if (!hook::Detour(this->pSource, this->pDestination, this->dwLen)) {
		Log::Error() << "TrampolineHook::CreateDetour(): Detour from source to destination failed ..." << Log::Endl;
		return false;
	}

	Log::Debug() << "TrampolineHook::CreateDetour(): \"" << (void*)this->pSource << "\" => \"" << (void*)this->pDestination << "\" successful ..." << Log::Endl;
	return this->isDetourCreated = true;
}

// release the memory allocation for the gateway
bool TrampolineHook::ReleaseGateway() {
	if (!this->pGateway) {
		Log::Info() << "TrampolineHook::ReleaseGateway(): No gateway available ..." << Log::Endl;
		return true;
	}

	if (VirtualFree((uintptr_t*)this->pGateway, 0, MEM_RELEASE) != 0) {
		Log::Error() << "TrampolineHook::ReleaseGateway(): VirtualFree() of gateway \"" << (void*)this->pGateway << "\" failed ..." << Log::Endl;
		return false;
	}
	return true;
}

// patch the stolen bytes back to the destination function.
bool TrampolineHook::ResetDetour() {
	if (!this->isDetourCreated) {
		Log::Info() << "TrampolineHook::ResetDetour(): No detour available ..." << Log::Endl;
		return true;
	}

	memory::Patch((BYTE*)this->pSource, (BYTE*)this->stolenBytes, this->dwLen);
	this->isDetourCreated = false;
	return true;
}
