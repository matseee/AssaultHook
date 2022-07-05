#pragma once
#include "pch.h"
#include "memory.h"

namespace Hook {

#ifdef _WIN64
#define DETOUR_MIN_LENGTH		16
#define DETOUR_JMP_INSTRUCTION	0x08EB0000000215FF
#else
#define DETOUR_MIN_LENGTH		5
#define DETOUR_JMP_INSTRUCTION	0xE9
#endif

	class TrampolineHook
	{
	protected:
		BYTE* pSource;
		BYTE* pDestination;
		
		uintptr_t dwLen;
		BYTE* pGateway;

	public:
		TrampolineHook();

		bool initialize(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen);

		BYTE* Enable();
		bool Disable();

		static bool Detour(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen);
		static bool CheckAllowed(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen);
	};

}