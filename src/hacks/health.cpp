#include "health.h"

// Original instructions of DoDamage()
// .text:0041C223 sub[ebx + 4], esi		- 29 73 04 
// .text:0041C226 mov     eax, esi		- 8B C6

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
	m_DoDamageHook = new memory::Hook(ADDR_DECREASE_HEALTH_INSTRUCTION, (addr)HealthHook, 5);
}

Health::~Health() {
	this->Deactivate();
}

void Health::Activate() {
	if (this->IsActive()) {
		return;
	}
	m_IsActive = m_DoDamageHook->Activate();
}

void Health::Deactivate() {
	if (!this->IsActive()) {
		return;
	}
	m_IsActive = !m_DoDamageHook->Deactivate();
}

void Health::Tick() {
	if (!this->m_AcState->IsReady()) {
		return;
	}
	LocalPlayerState = ((ulong)&this->m_AcState->LocalPlayer->Health) - 4;
}
