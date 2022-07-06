#pragma once
#include "AssaultCubeStructs.h"
#include "AssaultCubeAddresses.h"
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

class ESP {
public:
	void Initialize(Entity* localPlayer, EnitityList* entityList, float* matrix);

	void SetBoxActive(bool active);
	void SetSnaplineActive(bool active);

	bool IsInitialized();

	void Tick();
protected:
	bool isInitialized = false;

	bool isBoxActive = false;
	bool isSnaplineActive = false;

	int viewport[4];
	
	// int* gameMode = (int*)(ADDR_GAME_MODE);
	// int* numberOfPlayers = (int*)(ADDR_NUM_PLAYERS);
	float* matrix = nullptr;

	Entity* localPlayer = nullptr;
	EnitityList* entityList = nullptr;

	openGLText::Font openGLFont;

	bool IsTeamGame();
	bool IsEnemy(Entity* entity);
	bool IsValidEntity(Entity* entity);

	void DrawEntityBox(Entity* entity, Vector3 screen);
	void DrawEntitySnapline(Vector3 screenCordinates);
};

