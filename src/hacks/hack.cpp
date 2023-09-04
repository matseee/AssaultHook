#include "hack.h"

Hack::Hack(bool isActive, ActiveChangedCallback callback) {
	this->m_IsActive = isActive;
	this->m_Callback = callback;
	this->m_AcState = AcState::Get();
}

Hack::~Hack() {
	this->Deactivate();
	this->m_AcState = nullptr;
}

void Hack::Activate() {
	this->m_IsActive = true;
	this->CallActiveChangedCallback();
}

void Hack::Deactivate() {
	this->m_IsActive = false;
	this->CallActiveChangedCallback();
}

bool Hack::IsActive() {
	return this->m_IsActive;
}

bool Hack::IsActiveAndReady() {
	return this->m_IsActive && this->m_AcState && this->m_AcState->IsReady();
}

void Hack::Tick() {}

void Hack::CallActiveChangedCallback() {
	if (this->m_Callback) {
		this->m_Callback(this->m_IsActive);
	}
}
