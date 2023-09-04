#include "esp.h"

void ESPBox::Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {
	geometry::Rect rect = geometry::Rect();
	this->GetScaledEntityBox(entity, screenCoordinates, &rect);
	opengl::Draw::Outline(rect, 1.0f, this->GetEntityColor(entity));
}

void ESPName::Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {
	HDC currentHDC = wglGetCurrentDC();
	if (!this->m_Font.bBuilt || currentHDC != this->m_Font.hdc) {
		this->m_Font.Build(ESP_FONT_HEIGHT);
	}

	geometry::Rect rect = geometry::Rect();
	this->GetScaledEntityBox(entity, screenCoordinates, &rect);

	float textX = this->m_Font.CenterText(rect.x, rect.width, (float)(strlen(entity->Name) * ESP_FONT_WIDTH));
	float textY = rect.y - ESP_FONT_HEIGHT / 2;
	this->m_Font.Print(textX, textY, this->GetEntityColor(entity), "%s", entity->Name);
}

void ESPHealth::Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {
	const GLubyte* color; 
	if (entity->Health >= 66) {
		color = opengl::Color::GREEN;
	} else if (entity->Health >= 33) {
		color = opengl::Color::ORANGE;
	} else {
		color = opengl::Color::RED;
	}

	geometry::Rect rect = geometry::Rect();
	this->GetScaledEntityBox(entity, screenCoordinates, &rect);

	float tmp = rect.height * entity->Health / 100;
	rect.y += rect.height - tmp;
	rect.x -= 8.f;
	rect.width = 3.f;
	rect.height = tmp;

	opengl::Draw::FilledRect(rect, color);
}

void ESPLine::Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {
	geometry::Line line = geometry::Line(
		screenCoordinates,
		geometry::Point{ (float)this->m_Viewport.height, (float)this->m_Viewport.width / 2 }
	);

	opengl::Draw::Line(line, this->GetEntityColor(entity));
}

void ESPBase::Tick() {
	if (!this->IsActiveAndReady()) {
		return;
	}

	this->LoopOverEntities();
}

void ESPBase::LoopOverEntities() {
	if (!this->acState->EntityList || !this->acState->EntityList->Entities) {
		Log::Warning() << "ESPBase::LoopOverEntities(): \"acState->EntityList\" not available ..." << Log::Endl;
		this->acState->UpdateAttributes();
		return;
	}

	glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint*>(&this->m_Viewport));

	for (int i = 0; i < *this->acState->PlayerCount; i++) {
		if (!this->acState->IsValidEntity(this->acState->EntityList->Entities[i])) {
			continue;
		}

		AcEntity* entity = this->acState->EntityList->Entities[i];

		geometry::Vector2 screenCoordinates;
		if (geometry::WorldToScreen(entity->Origin, screenCoordinates, this->acState->Matrix, this->m_Viewport.width, this->m_Viewport.height)) {
			this->Render(entity, screenCoordinates);
		}
	}
}

const GLubyte* ESPBase::GetEntityColor(AcEntity* entity) {
	return (this->acState->IsEnemy(entity)) ? opengl::Color::RED : opengl::Color::BLUE;
}

float ESPBase::GetDistanceTo(AcEntity* entity) {
	return this->acState->LocalPlayer->Origin.Distance(entity->Origin);
}

void ESPBox::GetScaledEntityBox(AcEntity* entity, geometry::Vector2 screenCoordinates, geometry::Rect* rect) {
	glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint*>(&this->m_Viewport));

	geometry::Vector3 headWorldPos = geometry::Vector3(entity->Origin.x, entity->Origin.y, entity->Origin.z);
	headWorldPos.z += 0.8f;

	geometry::Vector3 feetWorldPos = headWorldPos.Copy();
	feetWorldPos.z -= entity->EyeHeight;

	geometry::Point headScreenPos, feetScreenPos;
	geometry::WorldToScreen(headWorldPos, headScreenPos, this->acState->Matrix, this->m_Viewport.width, this->m_Viewport.height);
	geometry::WorldToScreen(feetWorldPos, feetScreenPos, this->acState->Matrix, this->m_Viewport.width, this->m_Viewport.height);

	float squaredDistance = headScreenPos.Distance(feetScreenPos) * 1.2f;

	rect->x = headScreenPos.x - (squaredDistance / 4.0f);
	rect->y = headScreenPos.y;
	rect->width = squaredDistance / 2.0f;
	rect->height = squaredDistance;
}

