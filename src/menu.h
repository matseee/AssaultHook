#pragma once
#include "hack.h"
#include "log.h"
#include "openGLDraw.h"
#include "openGLText.h"

struct MenuEntry {
	const char* name;
	Hack* hack;
};

class Menu
{
public:
	Menu(const char* title, std::vector<MenuEntry> entries);
	void Tick();

protected:
	openGLText::Font openGLFont;

	const char* title;
	bool isOpen;

	std::vector<MenuEntry> entries;
	MenuEntry selectedEntry;
	int selectedIndex;

	void Input();
	void Render();
	void RenderMenu();
	void ForwardTick();
};

