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
	Vector3 tmpTo = entity->Head.x >= 0 ? entity->Head : entity->Origin;
	vec from = vec(localPlayer->Origin.x, localPlayer->Origin.y, localPlayer->Origin.z);
	vec to = vec(tmpTo.x, tmpTo.y, tmpTo.z);

	AcIntersectGeometry(from, to);
	return to.x == tmpTo.x && to.y == tmpTo.y && to.z == tmpTo.z;
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
