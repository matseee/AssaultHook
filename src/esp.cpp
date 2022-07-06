#include "pch.h"
#include "esp.h"

void ESP::Tick() {
	if (!this->entityList || !this->entityList->Entities) {
		return;
	}

	glGetIntegerv(GL_VIEWPORT, this->viewport);

	for (int i = 0; i < *this->playerCount; i++) {
		if (this->IsValidEntity(this->entityList->Entities[i])) {
			Entity* entity = this->entityList->Entities[i];

			Vector3 entityCenter = entity->PositionHead;
			entityCenter.z = (entity->PositionHead.z + entity->Position.z) / 2;

			Vector3 screenCoordinates;

			if (WorldToScreen(entityCenter, screenCoordinates, matrix, this->viewport[2], this->viewport[3])) {
				if (this->isBoxActive) {
					this->DrawEntityBox(entity, screenCoordinates);
				}

				if (this->isSnaplineActive) {
					this->DrawEntitySnapline(entity, screenCoordinates);
				}
			}
		}
	}
}

void ESP::DrawEntityBox(Entity* entity, Vector3 screenCoordinates) {
	HDC currentHDC = wglGetCurrentDC();

	if (!this->openGLFont.bBuilt || currentHDC != this->openGLFont.hdc) {
		this->openGLFont.Build(ESP_FONT_HEIGHT);
	}

	const GLubyte* color = rgb::blue;

	if (!this->IsTeamGame() || this->IsEnemy(entity)) {
		color = rgb::red;
	}

	float dist = localPlayer->Position.Distance(entity->Position);

	float scale = (GAME_UNIT_MAGIC / dist) * (this->viewport[2] / VIRTUAL_SCREEN_WIDTH);
	float x = screenCoordinates.x - scale;
	float y = screenCoordinates.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	openGLDraw::DrawOutline(x, y, width, height, 2.0f, color);

	float textX = this->openGLFont.centerText(x, width, strlen(entity->Name) * ESP_FONT_WIDTH);
	float textY = y - ESP_FONT_HEIGHT / 2;
	this->openGLFont.Print(textX, textY, color, "%s", entity->Name);
}

void ESP::DrawEntitySnapline(Entity* entity, Vector3 screenCoordinates) {
	const GLubyte* color = rgb::blue;

	if (!this->IsTeamGame() || this->IsEnemy(entity)) {
		color = rgb::red;
	}

	Vector2 v2ScreenCoordinates;
	v2ScreenCoordinates.x = screenCoordinates.x;
	v2ScreenCoordinates.y = screenCoordinates.y;

	Vector2 v2PlayerCoordinates;
	v2PlayerCoordinates.y = this->viewport[3];
	v2PlayerCoordinates.x = this->viewport[2] / 2;

	openGLDraw::DrawLine(v2PlayerCoordinates, v2ScreenCoordinates, color);
}

void ESP::Initialize(Entity* localPlayer, EnitityList* entityList, float* matrix, int* gameMode, int* playerCount) {
	this->localPlayer = localPlayer;
	this->entityList = entityList;
	this->matrix = matrix;
	this->gameMode = gameMode;
	this->playerCount = playerCount;
}

void ESP::SetBoxActive(bool active) {
	this->isBoxActive = active;
}

void ESP::SetSnaplineActive(bool active) {
	this->isSnaplineActive = active;
}

bool ESP::IsInitialized() {
	return this->isInitialized;
}

bool ESP::IsTeamGame() {
	return *gameMode == 0 || *gameMode == 4 
		|| *gameMode == 5 || *gameMode == 7 
		|| *gameMode == 11 || *gameMode == 13 
		|| *gameMode == 14 || *gameMode == 16 
		|| *gameMode == 17 || *gameMode == 20 
		|| *gameMode == 21;
}

bool ESP::IsEnemy(Entity* entity) {
	return entity->Team != this->localPlayer->Team;
}

bool ESP::IsValidEntity(Entity* entity) {
	return entity && entity->VTable == VAL_ENTITY_VTABLE_COMPARE
		&& entity->Health > 0;
}