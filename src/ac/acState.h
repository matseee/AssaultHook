#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include "../utils/log.h"
#include "acAddresses.h"
#include "acStructs.h"
#include "../memory/memory.h"
#include "../memory/signature_scanner.h"

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

	addr ModuleBase = NULL;

	addr NoRecoil = NULL;
	addr DecreaseAmmo = NULL;
	addr DecreaseHealth = NULL;
	addr IntersectClosest = NULL;
	addr IntersectGeometry = NULL;

	int* GameMode = nullptr;
	int* PlayerCount = nullptr;
	float* Matrix = nullptr;
	AcEntity* LocalPlayer = nullptr;
	AcEntityList* EntityList = nullptr;

protected:
	AcState();
	~AcState();

	void Setup();
	bool CheckReady();
	void LoadModules();
	bool ScanForSignatures();

	static AcState* Instance;
};

