#pragma once
#include "memory.h"
#include "../utils/log.h"

#ifdef _WIN64
#define DETOUR_MIN_LENGTH		16
#define DETOUR_JMP_INSTRUCTION	0x08EB0000000215FF
#else
#define DETOUR_MIN_LENGTH		5
#define DETOUR_JMP_INSTRUCTION	0xE9
#endif

class TrampolineHook
{
public:
	TrampolineHook(BYTE* pSource, BYTE* pDestination, uintptr_t dwLen);

	BYTE* Create();
	bool Destroy();

	bool CheckAllowed();
protected:
	BYTE* pSource;
	BYTE* pDestination;
	
	uintptr_t dwLen;
	BYTE* pGateway;

	bool CreateGateway();
	bool Detour();
};