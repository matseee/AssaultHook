#include "menu.h"

const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;

Menu::Menu(const char* title, std::vector<MenuEntry> entries) {
	this->title = title;
	this->isOpen = false;
	this->entries = entries;
	this->selectedEntry = entries[0];
	this->selectedIndex = 0;
}

void Menu::Tick() {
	this->Input();
	this->Render();
}

void Menu::Input() {
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		this->isOpen = !this->isOpen;
		Log::Debug() << "Menu::Input(): Menu => " << (this->isOpen ? "ON" : "OFF") << Log::Endl;
	}
	if (!this->isOpen) {
		return;
	}

	if (GetAsyncKeyState(VK_UP) & 1) {
		if (this->selectedIndex > 0) {
			this->selectedIndex--;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) & 1) {
		if (this->selectedIndex < (int)this->entries.size() - 1) {
			this->selectedIndex++;
		}
	}

	this->selectedEntry = this->entries[this->selectedIndex];
	if (GetAsyncKeyState(VK_LEFT) & 1 && this->selectedEntry.hack->IsActive()) {
		this->selectedEntry.hack->Deactivate();
		Log::Debug() << "Menu::Input(): " << this->selectedEntry.name << " => OFF" << Log::Endl;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 1 && !this->selectedEntry.hack->IsActive()) {
		this->selectedEntry.hack->Activate();
		Log::Debug() << "Menu::Input(): " << this->selectedEntry.name << " => ON" << Log::Endl;
	}
}

void Menu::Render() {
	opengl::SetupOrtho();
	this->ForwardTick();
	this->RenderMenu();
	opengl::RestoreGL();
}

void Menu::RenderMenu() {
	if (!this->isOpen) {
		return;
	}

	HDC currentHDC = wglGetCurrentDC();

	float posX = 50.0f;
	float posY = 50.0f;
	float menuHeight = ((int)this->entries.size() + 1) * FONT_HEIGHT + 5.0f;
	float menuWidth = 200.0f;
	float currentPosY = 50.0f + FONT_HEIGHT;

	if (!this->openGLFont.bBuilt || currentHDC != this->openGLFont.hdc) {
		this->openGLFont.Build(FONT_HEIGHT);
	}

	opengl::Draw::FilledRect(posX, posY, menuWidth, menuHeight, opengl::Color::BLACK);

	this->openGLFont.Print(this->openGLFont.CenterText(posX, menuWidth, (float)(strlen(this->title) * FONT_WIDTH)), currentPosY, opengl::Color::WHITE, "%s", this->title);

	for (int i = 0; i < (int)this->entries.size(); ++i) {
		MenuEntry entry = this->entries[i];

		currentPosY += FONT_HEIGHT;

		if (i == this->selectedIndex) {
			this->openGLFont.Print(posX + 5.0f, currentPosY, opengl::Color::WHITE, "%s", entry.name);
		}
		else {
			this->openGLFont.Print(posX + 5.0f, currentPosY, opengl::Color::LIGHTGRAY, "%s", entry.name);
		}

		if (entry.hack->IsActive()) {
			this->openGLFont.Print(this->openGLFont.EndText(posX, menuWidth, ((float)strlen("ON") * FONT_WIDTH)), currentPosY, opengl::Color::GREEN, "%s", "ON");
		}
		else {
			this->openGLFont.Print(this->openGLFont.EndText(posX, menuWidth, ((float)strlen("OFF") * FONT_WIDTH)), currentPosY, opengl::Color::RED, "%s", "OFF");
		}
	}
}

void Menu::ForwardTick() {
	for (int i = 0; i < (int)this->entries.size(); ++i) {
		this->entries[i].hack->Tick();
	}
}
