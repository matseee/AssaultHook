#include "aimbot.h"

void Aimbot::Tick() {
	if (!this->IsActiveAndReady() || !(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
		return;
	}

	AcEntity* entity = this->GetBestEntity();
	if (!entity) {
		Log::Debug() << "Aimbot::GetBestEntity(): No entity found!" << Log::Endl;
		return;
	}

	this->AimToEntity(entity);
}

AcEntity* Aimbot::GetBestEntity() {
	if (!this->m_AcState->EntityList || !this->m_AcState->EntityList->Entities) {
		Log::Warning() << "Aimbot::GetBestEntity(): \"m_AcState->EntityList\" not available ..." << Log::Endl;
		this->m_AcState->UpdateAttributes();
		return nullptr;
	}

	AcEntity* bestEntity = nullptr;
	AcEntity* entity = nullptr;

	float bestDistance = 0;
	float distance = 0;

	for (int i = 0; i < *this->m_AcState->PlayerCount; i++) {
		if (!this->m_AcState->IsValidEntity(this->m_AcState->EntityList->Entities[i])) {
			continue;
		}

		entity = this->m_AcState->EntityList->Entities[i];
		if (!this->m_AcState->IsEnemy(entity) || !this->IsVisible(entity)) {
			continue;
		}

		distance = this->m_AcState->LocalPlayer->Origin.Distance(entity->Origin);
		if (distance < bestDistance || bestDistance == 0) {
			bestDistance = distance;
			bestEntity = entity;
		}
	}
	return bestEntity;
}

bool Aimbot::IsVisible(AcEntity* entity) {
	AcEntity* localPlayer = this->m_AcState->LocalPlayer;
	geometry::Vector3 tmpTo = entity->Head.x >= 0 ? entity->Head : entity->Origin;
	vec from = vec(localPlayer->Origin.x, localPlayer->Origin.y, localPlayer->Origin.z);
	vec to = vec(tmpTo.x, tmpTo.y, tmpTo.z);

	AcIntersectGeometry(from, to);
	return to.x == tmpTo.x && to.y == tmpTo.y && to.z == tmpTo.z;
}

void Aimbot::AimToEntity(AcEntity* entity) {
	geometry::Vector3 angle = this->CalcAngle(entity);
	this->m_AcState->LocalPlayer->Angle.x = angle.x;
	this->m_AcState->LocalPlayer->Angle.y = angle.y;
}

geometry::Vector3 Aimbot::CalcAngle(AcEntity* entity) {
	geometry::Vector3 v = entity->Head.x >= 0 ? entity->Head : entity->Origin;

	v = this->m_AcState->LocalPlayer->Origin - v;

	geometry::Vector3 angles;
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
