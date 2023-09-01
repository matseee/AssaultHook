#pragma once
#include "acAddresses.h"
#include "acStructs.h"

void AcIntersectGeometry(const vec& from, vec& to);
AcEntity* AcIntersectClosest(const vec & from, const vec & to, const AcEntity * at, float& bestdistsquared, int& hitzone, bool aiming);

