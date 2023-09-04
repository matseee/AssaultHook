#include "menu.h"

Menu::Menu(std::vector<MenuEntry> m_Entries) {
	this->m_IsOpen = false;
	this->m_Entries = m_Entries;

	for (unsigned int i = 0; i < this->m_Entries.size(); ++i) {
		this->m_SelectedEntry = m_Entries[i];
		this->m_SelectedIndex = i;
		if (this->m_SelectedEntry.type == MenuEntryType::HACK) {
			break;
		}
	}
}

Menu::~Menu() {
	for (unsigned int i = 0; i < this->m_Entries.size(); i++) {
		if (this->m_Entries[i].type != MenuEntryType::HACK) {
			continue;
		}
		this->m_Entries[i].hack->Deactivate();
	}
}

void Menu::Tick() {
	this->Input();
	this->Render();
}

void Menu::Input() {
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		this->m_IsOpen = !this->m_IsOpen;
		Log::Debug() << "Menu::Input(): Menu => " << (this->m_IsOpen ? "ON" : "OFF") << Log::Endl;
	}
	if (!this->m_IsOpen) {
		return;
	}

	if (GetAsyncKeyState(VK_UP) & 1) { this->Previous(0); }
	if (GetAsyncKeyState(VK_DOWN) & 1) { this->Next(0); }

	this->m_SelectedEntry = this->m_Entries[this->m_SelectedIndex];
	if (GetAsyncKeyState(VK_LEFT) & 1 && this->m_SelectedEntry.hack->IsActive()) {
		this->m_SelectedEntry.hack->Deactivate();
		Log::Debug() << "Menu::Input(): " << this->m_SelectedEntry.name << " => OFF" << Log::Endl;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 1 && !this->m_SelectedEntry.hack->IsActive()) {
		this->m_SelectedEntry.hack->Activate();
		Log::Debug() << "Menu::Input(): " << this->m_SelectedEntry.name << " => ON" << Log::Endl;
	}
}

void Menu::Render() {
	opengl::SetupOrtho();
	this->ForwardTick();
	this->RenderMenu();
	opengl::RestoreGL();
}

void Menu::RenderMenu() {
	if (!this->m_IsOpen) {
		return;
	}

	HDC currentHDC = wglGetCurrentDC();

	geometry::Rect menuRect(MENU_POSITION, MENU_WIDTH,	this->CalculateMenuHeight());
	float currentPosY = MENU_POSITION.y;

	if (!this->m_Font.bBuilt || currentHDC != this->m_Font.hdc) {
		this->m_Font.Build((int)FONT_HEIGHT);
	}

	opengl::Draw::FilledRect(menuRect, opengl::Color::BLACK);


	for (unsigned int i = 0; i < this->m_Entries.size(); ++i) {
		MenuEntry entry = this->m_Entries[i];
		currentPosY += entry.GetHeight();

		if (entry.type == MenuEntryType::SPACER) {
			continue;
		}

		if (entry.type == MenuEntryType::TITLE) {
			this->m_Font.Print(this->m_Font.CenterText(menuRect.x, menuRect.width, (float)(strlen(entry.name) * FONT_WIDTH)), currentPosY, opengl::Color::WHITE, "%s", entry.name);
			continue;
		}

		if (i == this->m_SelectedIndex) {
			this->m_Font.Print(menuRect.x + 5.0f, currentPosY, opengl::Color::WHITE, "%s", entry.name);
		}
		else {
			this->m_Font.Print(menuRect.x + 5.0f, currentPosY, opengl::Color::GRAY, "%s", entry.name);
		}

		if (entry.hack->IsActive()) {
			const GLubyte* green = (i == this->m_SelectedIndex) ? opengl::Color::GREENLIGHT : opengl::Color::GREEN;
			this->m_Font.Print(this->m_Font.EndText(menuRect.x, menuRect.width, ((float)strlen("ON") * FONT_WIDTH)), currentPosY, green, "%s", "ON");
		}
		else {
			const GLubyte* red = (i == this->m_SelectedIndex) ? opengl::Color::REDLIGHT : opengl::Color::RED;
			this->m_Font.Print(this->m_Font.EndText(menuRect.x, menuRect.width, ((float)strlen("OFF") * FONT_WIDTH)), currentPosY, red, "%s", "OFF");
		}
	}
}

float Menu::CalculateMenuHeight() {
	float menuHeight = 5.0f;
	for (unsigned int i = 0; i < this->m_Entries.size(); ++i) {
		menuHeight += this->m_Entries[i].GetHeight();
	}
	return menuHeight;
}

void Menu::ForwardTick() {
	for (int i = 0; i < (int)this->m_Entries.size(); ++i) {
		if (this->m_Entries[i].hack) {
			this->m_Entries[i].hack->Tick();
		}
	}
}

void Menu::Next(int skip) {
	if (this->m_SelectedIndex + 1 == (int)this->m_Entries.size()) {
		this->m_SelectedIndex -= skip;
		return;
	}

	this->m_SelectedIndex++;
	if (this->m_Entries[this->m_SelectedIndex].type != MenuEntryType::HACK) {
		this->Next(skip + 1);
	}
}

void Menu::Previous(int skip) {
	if (this->m_SelectedIndex - 1 == 0) {
		this->m_SelectedIndex += skip;
		return;
	}

	this->m_SelectedIndex--;
	if (this->m_Entries[this->m_SelectedIndex].type != MenuEntryType::HACK) {
		this->Previous(skip + 1);
	}
}
