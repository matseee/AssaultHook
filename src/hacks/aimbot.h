#pragma once
#include "hack.h"
#include "../utils/log.h"
#include "../ac/acFunctions.h"
#include "../geometry/geometry.h"

class Aimbot : public Hack {
public:
	void Tick();

protected:
	AcEntity* GetBestEntity();

	void AimToEntity(AcEntity* entity);
	Vector3 CalcAngle(AcEntity* entity);
	
	bool IsVisible(AcEntity* entity);
};
