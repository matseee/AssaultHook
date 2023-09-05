#include "memhack.h"

Patch::Patch(uintptr_t address, uintptr_t valueOn,
	uintptr_t valueOff, unsigned long size) : Hack() {
	this->m_Address = address;
	this->m_ValueOn = valueOn;
	this->m_ValueOff = valueOff;
	this->m_Size = size;
}

Patch::~Patch() {
	this->Deactivate();
	this->m_Address = NULL;
	this->m_ValueOn = NULL;
	this->m_ValueOff = NULL;
	this->m_Size = NULL;
	this->m_AcState = nullptr;
}

void Patch::Activate() {
	Hack::Activate();
	this->MemPatch();
}

void Patch::Deactivate() {
	Hack::Deactivate();
	this->MemPatch();
}

void Patch::MemPatch() {
	memory::PatchBytes((BYTE*)this->m_Address, (BYTE*)(this->IsActive() ? this->m_ValueOn : this->m_ValueOff), this->m_Size);
}
