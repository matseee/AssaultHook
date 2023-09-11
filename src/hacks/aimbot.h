#pragma once
#include "hack.h"
#include "../ac/acFunctions.h"
#include "../geometry/geometry.h"
#include "../utils/log.h"

class Aimbot : public Hack {
public:
    void Tick();

protected:
    AcEntity* GetBestEntity();

    void AimToEntity(AcEntity* entity);
    geometry::Vector3 CalcAngle(AcEntity* entity);

    bool IsVisible(AcEntity* entity);
};

