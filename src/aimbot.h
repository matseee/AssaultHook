#pragma once
#include "framework.h"
#include "AssaultCubeStructs.h"

class Aimbot {
public:
	void Initialize(Entity* localPlayer, EnitityList* entityList);
	void SetEnabled(bool enabled);

	void Tick();

	bool IsInitialized();

protected:
	Entity* localPlayer;
	EnitityList* entityList;

	bool isInitialized = false;
	bool isEnabled = false;

	Entity* GetBestEntity();

	void AimToEntity(Entity* entity);
	Vector3 CalcAngle(Entity* entity);
};

