#include "pch.h"
#include "hack.h"

Hack::Hack(bool isActive, ActiveChangedCallback callback) {
	this->isActive = isActive;
	this->callback = callback;
	this->acState = AcState::Get();
}

void Hack::Activate() {
	this->isActive = true;
	this->CallActiveChangedCallback();
}

void Hack::Deactivate() {
	this->isActive = false;
	this->CallActiveChangedCallback();
}

bool Hack::IsActive() {
	return this->isActive;
}

bool Hack::IsActiveAndReady() {
	return this->isActive && this->acState && this->acState->IsReady();
}

void Hack::Tick() {}

void Hack::CallActiveChangedCallback() {
	if (this->callback) {
		this->callback(this->isActive);
	}
}
