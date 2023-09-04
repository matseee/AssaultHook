#pragma once
#include "hack.h"
#include "../ac/acState.h"
#include "../opengl/opengl.h"

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

struct Viewport {
	int x, y, width, height = 0;
};

class ESPBase : public Hack {
public:
	void Tick();
protected:
	Viewport m_Viewport;
	
	void LoopOverEntities();
	virtual void Render(AcEntity* entity, geometry::Vector2 screenCoordinates) {};

	const GLubyte* GetEntityColor(AcEntity* entity);
	float GetDistanceTo(AcEntity* entity);
};

class ESPBox : public ESPBase {
protected:
	void Render(AcEntity* entity, geometry::Vector2 screenCoordinates);
	void GetScaledEntityBox(AcEntity* entity, geometry::Vector2 screenCoordinates, geometry::Rect* rect);
};

class ESPHealth : public ESPBox {
protected:
	void Render(AcEntity* entity, geometry::Vector2 screenCoordinates);
};

class ESPName : public ESPBox {
protected:
	opengl::Text::Font m_Font;
	void Render(AcEntity* entity, geometry::Vector2 screenCoordinates);
};

class ESPLine : public ESPBase {
protected:
	void Render(AcEntity* entity, geometry::Vector2 screenCoordinates);
};

