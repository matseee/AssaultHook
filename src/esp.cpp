#include "pch.h"
#include "esp.h"

void ESP::Tick() {
	if (!this->isActive || !this->entityList || !this->entityList->Entities) {
		return;
	}

	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 0; i < 12; i++) {
		if (this->IsValidEntity(this->entityList->Entities[i])) {
			Entity* entity = this->entityList->Entities[i];

			Vector3 entityCenter = entity->PositionHead;
			entityCenter.z = (entity->PositionHead.z + entity->Position.z) / 2;

			Vector3 screenCordinates;

			if (WorldToScreen(entityCenter, screenCordinates, matrix, viewport[2], viewport[3])) {
				this->DrawEntity(entity, screenCordinates);
			}
		}
	}
}

void ESP::DrawEntity(Entity* entity, Vector3 screen) {
	HDC currentHDC = wglGetCurrentDC();

	if (!this->openGLFont.bBuilt || currentHDC != this->openGLFont.hdc) {
		this->openGLFont.Build(ESP_FONT_HEIGHT);
	}

	const GLubyte* color = rgb::red;

	float dist = localPlayer->Position.Distance(entity->Position);

	float scale = (GAME_UNIT_MAGIC / dist) * (viewport[2] / VIRTUAL_SCREEN_WIDTH);
	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	openGLDraw::DrawOutline(x, y, width, height, 2.0f, color);

	float textX = this->openGLFont.centerText(x, width, strlen(entity->Name) * ESP_FONT_WIDTH);
	float textY = y - ESP_FONT_HEIGHT / 2;
	this->openGLFont.Print(textX, textY, color, "%s", entity->Name);
}

void ESP::Initialize(Entity* localPlayer, EnitityList* entityList, float* matrix) {
	this->localPlayer = localPlayer;
	this->entityList = entityList;
	this->matrix = matrix;
}

void ESP::Activate() {
	this->isActive = true;
}

void ESP::Deactivate() {
	this->isActive = false;
}

bool ESP::IsTeamGame() {
	return false;
	//return *gameMode == 0 || *gameMode == 4 
	//	|| *gameMode == 5 || *gameMode == 7 
	//	|| *gameMode == 11 || *gameMode == 13 
	//	|| *gameMode == 14 || *gameMode == 16 
	//	|| *gameMode == 17 || *gameMode == 20 
	//	|| *gameMode == 21;
}

bool ESP::IsEnemy(Entity* entity) {
	// TODO
	return false;
}

bool ESP::IsValidEntity(Entity* entity) {
	return entity && entity->VTable == VAL_ENTITY_VTABLE_COMPARE
		&& entity->Health > 0;
}