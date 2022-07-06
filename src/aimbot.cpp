#include "pch.h"
#include "aimbot.h"

void Aimbot::Initialize(Entity* localPlayer, EnitityList* entityList, int* playerCount) {
	this->localPlayer = localPlayer;
	this->entityList = entityList;
	this->playerCount = playerCount;

	this->isInitialized = true;
}

void Aimbot::SetEnabled(bool enabled) {
	this->isEnabled = enabled;
}

void Aimbot::Tick() {
	if (!this->isEnabled || !this->isInitialized) {
		return;
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
		Entity* entity = this->GetBestEntity();

		if (entity) {
			this->AimToEntity(entity);
		}
	}
}

bool Aimbot::IsInitialized() {
	return this->isInitialized;
}

Entity* Aimbot::GetBestEntity() {
	float distance = 0;
	float newDistance = 0;

	Entity* entity = nullptr;

	Vector3 localAngles;
	localAngles.x = this->localPlayer->Angle.x;
	localAngles.y = this->localPlayer->Angle.y;

	for (int i = 0; i < *this->playerCount; i++) {
		if (this->entityList && this->entityList->Entities[i]) {
			Entity* maybeEntity = this->entityList->Entities[i];

			if (maybeEntity->Health <= 0) {
				continue;
			}

			Vector3 angle = this->CalcAngle(maybeEntity);
			newDistance = localAngles.Distance(angle);

			if (newDistance < distance || distance == 0) {
				distance = newDistance;
				entity = maybeEntity;
			}
		}
	}

	return entity;
}

void Aimbot::AimToEntity(Entity* entity) {
	Vector3 angle = this->CalcAngle(entity);

	localPlayer->Angle.x = angle.x;
	localPlayer->Angle.y = angle.y;
}

Vector3 Aimbot::CalcAngle(Entity* entity) {
	Vector3 v = entity->PositionHead;
	v.z -= .1f;

	v = this->localPlayer->PositionHead - v;

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
