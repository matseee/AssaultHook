#pragma once
#include "framework.h"
#include "trampoline_hook.h"

namespace AssaultHook {
	// functions / variables for console output
	void AllocateConsole();
	void DeallocateConsole();

	// thread that creates the hook
	DWORD __stdcall Thread(HMODULE hModule);
};