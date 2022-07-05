#include "pch.h"
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
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		this->isOpen = !this->isOpen;
	}

	if (this->isOpen) {
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

		if (GetAsyncKeyState(VK_LEFT) & 1 && this->selectedEntry.active) {
			this->entries[this->selectedIndex].active = false;
			if (this->entries[this->selectedIndex].oneTimeCallback) {
				this->entries[this->selectedIndex].oneTimeCallback(false);
			}
		}

		if (GetAsyncKeyState(VK_RIGHT) & 1 && !this->selectedEntry.active) {
			this->entries[this->selectedIndex].active = true;
			if (this->entries[this->selectedIndex].oneTimeCallback) {
				this->entries[this->selectedIndex].oneTimeCallback(true);
			}
		}

		this->Draw();
	}

	// always call the entry-callbacks, even if the menu is closed
	for (int i = 0; i < (int)this->entries.size(); ++i) {
		MenuEntry entry = this->entries[i];
		if (entry.active && entry.callback) {
			entry.callback();
		}
	}
}

void Menu::Draw() {
	HDC currentHDC = wglGetCurrentDC();

	float posX = 50.0f;
	float posY = 50.0f;
	float menuHeight = ((int)this->entries.size() + 1) * FONT_HEIGHT + 5.0f;
	float menuWidth = 200.0f;
	float currentPosY = 50.0f + FONT_HEIGHT;

	if (!this->openGLFont.bBuilt || currentHDC != this->openGLFont.hdc) {
		this->openGLFont.Build(FONT_HEIGHT);
	}

	openGLDraw::SetupOrtho();
	openGLDraw::DrawFilledRect(posX, posY, menuWidth, menuHeight, rgb::black);

	this->openGLFont.Print(this->openGLFont.centerText(posX, menuWidth, strlen(this->title) * FONT_WIDTH), currentPosY, rgb::white, "%s", this->title);

	for (int i = 0; i < (int)this->entries.size(); ++i) {
		MenuEntry entry = this->entries[i];

		currentPosY += FONT_HEIGHT;

		if (i == this->selectedIndex) {
			this->openGLFont.Print(posX + 5.0f, currentPosY, rgb::white, "%s", entry.name);
		}
		else {
			this->openGLFont.Print(posX + 5.0f, currentPosY, rgb::lightgray, "%s", entry.name);
		}

		if (entry.active) {
			this->openGLFont.Print(this->openGLFont.endText(posX, menuWidth, strlen("ON") * FONT_WIDTH), currentPosY, rgb::green, "%s", "ON");
		}
		else {
			this->openGLFont.Print(this->openGLFont.endText(posX, menuWidth, strlen("OFF") * FONT_WIDTH), currentPosY, rgb::red, "%s", "OFF");
		}
	}

	openGLDraw::RestoreGL();
}
