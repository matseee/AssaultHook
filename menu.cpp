#include "pch.h"
#include "menu.h"

Menu::Menu(const char* title, std::vector<MenuEntry> entries) {
	this->Title = title;
	this->Entries = entries;
	this->SelectedEntry = entries[0];
	this->SelectedIndex = 0;
}

void Menu::Tick() {
	if (GetAsyncKeyState(VK_UP) & 1) {
		if (this->SelectedIndex > 0) {
			this->SelectedIndex--;
		}
	}

	if (GetAsyncKeyState(VK_DOWN) & 1) {
		if (this->SelectedIndex < (int)this->Entries.size() - 1) {
			this->SelectedIndex++;
		}
	}

	this->SelectedEntry = this->Entries[this->SelectedIndex];

	if (GetAsyncKeyState(VK_LEFT) & 1 && this->SelectedEntry.Active) {
		this->SelectedEntry.Active = false;
		this->SelectedEntry.OneTimeCallback(false);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 1 && !this->SelectedEntry.Active) {
		this->SelectedEntry.Active = true;
		this->SelectedEntry.OneTimeCallback(true);
	}

	this->Draw();
}

void Menu::Draw() {
	for (int i = 0; i < (int)this->Entries.size(); ++i) {
		MenuEntry entry = this->Entries[i];

		// RENDER ENTRY

		if (entry.Active && entry.Callback) {
			entry.Callback();
		}
	}
}
