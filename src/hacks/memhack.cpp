#include "memhack.h"

Patch::Patch(uintptr_t address, uintptr_t valueOn,
	uintptr_t valueOff, unsigned long size) : Hack() {
	this->address = address;
	this->valueOn = valueOn;
	this->valueOff = valueOff;
	this->size = size;
}

Patch::~Patch() {
	this->Deactivate();
	this->address = NULL;
	this->valueOn = NULL;
	this->valueOff = NULL;
	this->size = NULL;
	this->acState = nullptr;
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
