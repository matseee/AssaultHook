#include "memhack.h"

Patch::Patch(uintptr_t address, uintptr_t valueOn,
	uintptr_t valueOff, unsigned long size) : Hack() {
	this->address = address;
	this->valueOn = valueOn;
	this->valueOff = valueOff;
	this->size = size;
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
	memory::Patch((BYTE*)this->acState->ModuleBase + this->address, (BYTE*)(this->IsActive() ? this->valueOn : this->valueOff), this->size);
}
