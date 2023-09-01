#pragma once
#include "../ac/acState.h"

typedef void(*ActiveChangedCallback)(bool);

class Hack
{
public:
	Hack() : Hack(false, nullptr) {};
	Hack(bool isActive, ActiveChangedCallback callback);
	virtual void Activate();
	virtual void Deactivate();

	virtual void Tick();
	bool IsActive();
	bool IsActiveAndReady();
	
protected:
	AcState* acState;
	bool isActive;

	ActiveChangedCallback callback;
	void CallActiveChangedCallback();
};

