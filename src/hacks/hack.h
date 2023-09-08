#pragma once
#include "../ac/acState.h"

typedef void(*ActiveChangedCallback)(bool);

class Hack
{
public:
	Hack() : Hack(false, nullptr) {};
	Hack(bool isActive, ActiveChangedCallback callback) {
		m_IsActive = isActive;
		m_Callback = callback;
		m_AcState = AcState::Get();
	};
	~Hack() {
		Deactivate();
		m_AcState = nullptr;
	};
	virtual void Activate() {
		m_IsActive = true;
		CallActiveChangedCallback();
	};
	virtual void Deactivate() {
		m_IsActive = false;
		CallActiveChangedCallback();
	};
	bool IsActive() {
		return m_IsActive;
	};
	bool IsActiveAndReady() {
		return m_IsActive && m_AcState && m_AcState->IsReady();
	};
	virtual void Tick() {};
	
protected:
	void CallActiveChangedCallback() {
		if (m_Callback) {
			m_Callback(m_IsActive);
		}
	};

	ActiveChangedCallback m_Callback;
	AcState* m_AcState;
	bool m_IsActive;
};

