#pragma once
#include "../utils/log.h"
#include "../hacks/hack.h"
#include "../opengl/opengl.h"

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
	opengl::Text::Font openGLFont;

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

