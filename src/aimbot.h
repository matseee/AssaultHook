#pragma once
#include "framework.h"
#include "log.h"
#include "acFunctions.h"
#include "geomatry.h"
#include "hack.h"

class Aimbot : public Hack {
public:
	void Tick();

protected:
	AcEntity* GetBestEntity();

	void AimToEntity(AcEntity* entity);
	Vector3 CalcAngle(AcEntity* entity);
	
	bool IsVisible(AcEntity* entity);
};

