#pragma once
#include "hack.h"
#include "../memory/hook.h"

class Health : public Hack {
public:
	Health();
	~Health();

	void Activate();
	void Deactivate();
	void Tick();

protected:
	memory::Hook* m_DoDamageHook;
};
