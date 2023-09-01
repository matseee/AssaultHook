#include "pch.h"
#include "esp.h"

void ESPBox::Render(AcEntity* entity, Vector3 screenCoordinates) {
	float x, y, width, height = 0.f;
	this->GetScaledEntityBox(entity, screenCoordinates, &x, &y, &height, &width);
	openGLDraw::DrawOutline(x, y, width, height, 1.0f, this->GetEntityColor(entity));
}


void ESPName::Render(AcEntity* entity, Vector3 screenCoordinates) {
	HDC currentHDC = wglGetCurrentDC();
	if (!this->openGLFont.bBuilt || currentHDC != this->openGLFont.hdc) {
		this->openGLFont.Build(ESP_FONT_HEIGHT);
	}

	float x, y, width, height = 0.f;
	this->GetScaledEntityBox(entity, screenCoordinates, &x, &y, &height, &width);

	float textX = this->openGLFont.centerText(x, width, (float)(strlen(entity->Name) * ESP_FONT_WIDTH));
	float textY = y - ESP_FONT_HEIGHT / 2;
	this->openGLFont.Print(textX, textY, this->GetEntityColor(entity), "%s", entity->Name);
}

void ESPHealth::Render(AcEntity* entity, Vector3 screenCoordinates) {
	const GLubyte* color; 
	if (entity->Health >= 66) {
		color = rgb::green;
	} else if (entity->Health >= 33) {
		color = rgb::orange;
	} else {
		color = rgb::red;
	}

	float x, y, width, height = 0.f;
	this->GetScaledEntityBox(entity, screenCoordinates, &x, &y, &height, &width);

	float tmp = height * entity->Health / 100;
	y += height - tmp;
	height = tmp;
	width = 3.f;
	x -= 8.f;

	openGLDraw::DrawFilledRect(x, y, width, height, color);
}

void ESPLine::Render(AcEntity* entity, Vector3 screenCoordinates) {
	Vector2 v2ScreenCoordinates;
	v2ScreenCoordinates.x = screenCoordinates.x;
	v2ScreenCoordinates.y = screenCoordinates.y;

	Vector2 v2PlayerCoordinates;
	v2PlayerCoordinates.y = (float)this->viewport[3];
	v2PlayerCoordinates.x = (float)this->viewport[2] / 2;

	openGLDraw::DrawLine(v2PlayerCoordinates, v2ScreenCoordinates, this->GetEntityColor(entity));
}

void ESPBase::Tick() {
	if (!this->IsActiveAndReady()) {
		return;
	}

	this->LoopOverEntities();
}

void ESPBase::LoopOverEntities() {
	if (!this->acState->EntityList || !this->acState->EntityList->Entities) {
		Log::Warning() << "ESPBase::LoopOverEntities(): \"acState->EntityList\" not available ..." << std::endl;
		this->acState->UpdateAttributes();
		return;
	}

	glGetIntegerv(GL_VIEWPORT, this->viewport);

	for (int i = 0; i < *this->acState->PlayerCount; i++) {
		if (!this->acState->IsValidEntity(this->acState->EntityList->Entities[i])) {
			continue;
		}

		AcEntity* entity = this->acState->EntityList->Entities[i];

		Vector3 entityCenter = entity->Origin;
		entityCenter.z = (entityCenter.z + entity->NewPosition.z) / 2;

		Vector3 screenCoordinates;
		if (WorldToScreen(entityCenter, screenCoordinates, this->acState->Matrix, this->viewport[2], this->viewport[3])) {
			this->Render(entity, screenCoordinates);
		}
	}
}

const GLubyte* ESPBase::GetEntityColor(AcEntity* entity) {
	return (this->acState->IsEnemy(entity)) ? rgb::red : rgb::blue;
}

float ESPBase::GetDistanceTo(AcEntity* entity) {
	return this->acState->LocalPlayer->Origin.Distance(entity->Origin);
}

void ESPBox::GetScaledEntityBox(AcEntity* entity, Vector3 screenCoordinates, float* x, float* y, float* height, float* width) {
	float scale = (GAME_UNIT_MAGIC / this->GetDistanceTo(entity)) * (this->viewport[2] / VIRTUAL_SCREEN_WIDTH);
	*x = screenCoordinates.x - scale;
	*y = screenCoordinates.y - scale * PLAYER_ASPECT_RATIO;
	*width = scale * 2;
	*height = scale * PLAYER_ASPECT_RATIO * 2;
}

