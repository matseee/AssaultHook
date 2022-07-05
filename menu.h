#pragma once
#include "openGLDraw.h"
#include "openGLText.h"

struct MenuEntry {
	const char* name;
	bool active;
	void (*oneTimeCallback)(bool);
	void (*callback)(void);
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

	void Draw();
};

