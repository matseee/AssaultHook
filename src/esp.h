#pragma once
#include "hack.h"
#include "acState.h"
#include "openGLDraw.h"
#include "openGLText.h"

const int VIRTUAL_SCREEN_WIDTH = 800;
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;
const float EYE_HEIGHT = 4.5f;
const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

class ESPBase : public Hack {
public:
	void Tick();
protected:
	int viewport[4];
	
	void LoopOverEntities();
	virtual void Render(AcEntity* entity, Vector3 screenCoordinates) {};

	const GLubyte* GetEntityColor(AcEntity* entity);
	float GetDistanceTo(AcEntity* entity);
};

class ESPBox : public ESPBase {
protected:
	void Render(AcEntity* entity, Vector3 screenCoordinates);
	void GetScaledEntityBox(AcEntity* entity, Vector3 screenCoordinates, float* x, float* y, float* height, float* width);
};

class ESPHealth : public ESPBox {
protected:
	void Render(AcEntity* entity, Vector3 screenCoordinates);
};

class ESPName : public ESPBox {
protected:
	openGLText::Font openGLFont;
	void Render(AcEntity* entity, Vector3 screenCoordinates);
};

class ESPLine : public ESPBase {
protected:
	void Render(AcEntity* entity, Vector3 screenCoordinates);
};

