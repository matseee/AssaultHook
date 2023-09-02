#pragma once
#include <Windows.h>
#include <iostream>
#include "../utils/log.h"
#include "acAddresses.h"
#include "acStructs.h"

class AcState
{
public:
	static AcState* Get();
	static void Destroy();

	bool IsReady();
	void UpdateAttributes();

	bool IsTeamGame();
	bool IsEnemy(AcEntity* entity);
	bool IsValidEntity(AcEntity* entity);

	uintptr_t ModuleBase = NULL;

	int* GameMode = nullptr;
	int* PlayerCount = nullptr;
	float* Matrix = nullptr;
	AcEntity* LocalPlayer = nullptr;
	AcEntityList* EntityList = nullptr;

protected:
	static AcState* Instance;

	AcState();
	~AcState();

	void Setup();
	bool CheckReady();
	void LoadModules();
};

