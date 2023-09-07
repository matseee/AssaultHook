#include "health.h"

DWORD LocalPlayerState;
DWORD JmpBackHealthHook = ADDR_DECREASE_HEALTH_INSTRUCTION + 5;

void _declspec(naked) HealthHook() {
	__asm {
		cmp ebx, [LocalPlayerState]		// compare current this-pointer with LocalPlayerState
		jne is_bot						// if not equal: jump over the next instruction
		mov esi, 0						// else set damage (esi) to 0
		is_bot:
		sub[ebx + 4], esi				// execute the stolen bytes
		mov eax, esi
		jmp JmpBackHealthHook			// jump back to original function
	}
}

Health::Health() : Hack() {
	// copy "stolen bytes", so that we can restore the original code execution
	// .text:0041C223 sub[ebx + 4], esi		- 29 73 04 
	// .text:0041C226 mov     eax, esi		- 8B C6
	memory::ReadBytes((BYTE*)ADDR_DECREASE_HEALTH_INSTRUCTION, this->m_StolenBytes, 5);
}

Health::~Health() {
	this->Deactivate();
}

void Health::Activate() {
	if (this->IsActive()) {
		return;
	}
	if (!memory::Hook((uintptr_t)ADDR_DECREASE_HEALTH_INSTRUCTION, (uintptr_t)HealthHook, 5, 0)) {
		return;
	}
	Hack::Activate();
}

void Health::Deactivate() {
	if (!this->IsActive()) {
		return;
	}
	if (!memory::PatchBytes((BYTE*)ADDR_DECREASE_HEALTH_INSTRUCTION, (BYTE*)this->m_StolenBytes, 5)) {
		return;
	}
	Hack::Deactivate();
}

void Health::Tick() {
	if (!this->m_AcState->IsReady()) {
		return;
	}
	LocalPlayerState = ((DWORD)&this->m_AcState->LocalPlayer->Health) - 4;
}
