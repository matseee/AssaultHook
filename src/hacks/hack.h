#pragma once
#include "../ac/acState.h"

typedef void(*ActiveChangedCallback)(bool);

class Hack
{
public:
	Hack() : Hack(false, nullptr) {};
	Hack(bool isActive, ActiveChangedCallback callback);
	~Hack();
	virtual void Activate();
	virtual void Deactivate();

	virtual void Tick();
	bool IsActive();
	bool IsActiveAndReady();
	
protected:
	AcState* m_AcState;
	bool m_IsActive;

	ActiveChangedCallback m_Callback;
	void CallActiveChangedCallback();
};

