#pragma once
#include "memory.h"
#include "hook.h"
#include "../utils/log.h"

namespace memory {
	class TrampolineHook {
	public:
		TrampolineHook(const char* cModuleName, const char* cProcName, uintptr_t pDestination, uintptr_t dwLen);
		TrampolineHook(HMODULE hModule, const char* cProcName, uintptr_t pDestination, uintptr_t dwLen);
		TrampolineHook(uintptr_t pSource, uintptr_t pDestination, uintptr_t dwLen);
		~TrampolineHook();

		bool Activate();
		bool Deactivate();
		void Destroy();

		uintptr_t GetGateway();

	protected:
		uintptr_t pSource;
		uintptr_t pDestination;

		uintptr_t dwLen;
		uintptr_t pGateway;

		BYTE stolenBytes[DETOUR_MIN_LENGTH * 2];

		bool isGatewayCreated = false;
		bool isDetourCreated = false;

		void Create();

		bool CheckParameterValid();
		bool CreateGateway();
		bool CreateDetour();

		bool ReleaseGateway();
		bool ResetDetour();
	};
};