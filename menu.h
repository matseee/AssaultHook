#pragma once

struct MenuEntry {
	const char* Name;
	bool Active;
	bool (*OneTimeCallback)(bool);
	bool (*Callback)(void);
};

class Menu
{
public:
	Menu(const char* title, std::vector<MenuEntry> entries);
	void Tick();

protected:
	const char* Title;

	std::vector<MenuEntry> Entries;
	MenuEntry SelectedEntry;
	int SelectedIndex;

	void Draw();
};

