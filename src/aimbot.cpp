#include "pch.h"
#include "aimbot.h"

void Aimbot::Tick() {
	if (!this->IsActiveAndReady() || !(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
		return;
	}

	AcEntity* entity = this->GetBestEntity();
	if (!entity) {
		Log::Debug() << "Aimbot::GetBestEntity(): No entity found!" << std::endl;
		return;
	}

	this->AimToEntity(entity);
}

AcEntity* Aimbot::GetBestEntity() {
	if (!this->acState->EntityList || !this->acState->EntityList->Entities) {
		Log::Warning() << "Aimbot::GetBestEntity(): \"acState->EntityList\" not available ..." << std::endl;
		this->acState->UpdateAttributes();
		return nullptr;
	}

	AcEntity* bestEntity = nullptr;
	AcEntity* entity = nullptr;

	float bestDistance = 0;
	float distance = 0;

	for (int i = 0; i < *this->acState->PlayerCount; i++) {
		if (!this->acState->IsValidEntity(this->acState->EntityList->Entities[i])) {
			continue;
		}

		entity = this->acState->EntityList->Entities[i];
		if (!this->acState->IsEnemy(entity) || !this->IsVisible(entity)) {
			continue;
		}

		distance = this->acState->LocalPlayer->Origin.Distance(entity->Origin);
		if (distance < bestDistance || bestDistance == 0) {
			bestDistance = distance;
			bestEntity = entity;
		}
	}
	return bestEntity;
}

bool Aimbot::IsVisible(AcEntity* entity) {
	AcEntity* localPlayer = this->acState->LocalPlayer;
	int hitzone = -1;
	float bestDistanceSquared = 0.f;
	
	Vector3 tmpTo = entity->Head.x >= 0 ? entity->Head : entity->Origin;

	vec from = vec(localPlayer->Origin.x, localPlayer->Origin.y, localPlayer->Origin.z);
	vec to = vec(tmpTo.x, tmpTo.y, tmpTo.z);

	AcEntity* foundPlayerEntity = this->CallIntersectClosest(from, to, localPlayer, bestDistanceSquared, hitzone, true);

	if (!hitzone || foundPlayerEntity != entity || foundPlayerEntity == this->acState->LocalPlayer) {
		return false;
	}

	Log::Debug() << "Aimbot::IsVisible: Found player entity => " << (void*)foundPlayerEntity << " / Hitzone => " << hitzone << std::endl;
	return true;
}
// Wrapper for original AssaultCube "intersectclosest" function (at = LocalPlayer)
// int __usercall IntersectClosest@<eax>(int to@<edx>, int at, float* bestdistsquared, _DWORD* hitzone, char aAiming) <= at 0x004CA250
AcEntity* Aimbot::CallIntersectClosest(const vec& from, const vec& to, const AcEntity* at, float& bestdistsquared, int& hitzone, bool aiming) {
	DWORD FuncIntersectClosest = ADDR_INTERSECTCLOSEST_FUNCTION;
	AcEntity* resultEntity = nullptr;

	void* aHitzone = &hitzone;
	void* aBestDist = &bestdistsquared;
	void* aTo = (void*)&to;

	// Compiler magic happens here. This function gets called from four different places, but everytime the "at"-Entity is the LocalPlayer
	// and also the "from"-argument is the position of the LocalPlayer - SO the compiler decided to throw away the from-argument and only use
	// the "at" entity (which is the same address)...
	__asm
	{
		push [aiming]
		push aHitzone
		push aBestDist
		push at
		mov edx, aTo
		call FuncIntersectClosest
		mov resultEntity, eax
		add esp, 10h
	}

	return resultEntity;
}

void Aimbot::AimToEntity(AcEntity* entity) {
	Vector3 angle = this->CalcAngle(entity);
	this->acState->LocalPlayer->Angle.x = angle.x;
	this->acState->LocalPlayer->Angle.y = angle.y;
}

Vector3 Aimbot::CalcAngle(AcEntity* entity) {
	Vector3 v = entity->Head.x >= 0 ? entity->Head : entity->Origin;

	v = this->acState->LocalPlayer->Origin - v;

	Vector3 angles;
	angles.x = ::atanf(v.x / v.y) * -57.2957795f;
	angles.y = ::atanf(v.z / ::sqrtf(v.x * v.x + v.y * v.y)) * -57.2957795f;
	angles.z = 0.f;

	if (v.y < 0.0f)
		angles.x += 180.0f;

	while (angles.x < 0.f)
		angles.x += 360.f;

	while (angles.x >= 360.f)
		angles.x -= 360.f;

	if (angles.y > 90.f)
		angles.y = 90.f;

	if (angles.y < -90.f)
		angles.y = -90.0;

	return angles;
}
