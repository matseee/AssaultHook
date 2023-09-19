#pragma once
#include "../utils/log.h"
#include "../hacks/hack.h"
#include "../opengl/opengl.h"

const float FONT_HEIGHT = 15.0f;
const float FONT_WIDTH = 9.0f;
const geometry::Point MENU_POSITION(50.0f, 50.0f);
const float MENU_WIDTH = 200.0f;

enum MenuEntryType {
    HACK,
    SPACER,
    TITLE,
};

struct MenuEntry {
    const char* name;
    Hack* hack;
    MenuEntryType type;

    float GetHeight() { return this->type == MenuEntryType::SPACER ? FONT_HEIGHT / 2 : FONT_HEIGHT; };
};

class Menu {
public:
    Menu(std::vector<MenuEntry> m_Entries);
    ~Menu();
    void Tick();

protected:
    void Input();
    void Render();
    void RenderMenu();
    float CalculateMenuHeight();
    void ForwardTick();
    void Next(int iSkip = 0);
    void Previous(int iSkip = 0);

    std::vector<MenuEntry> m_Entries;
    MenuEntry m_SelectedEntry;
    int m_SelectedIndex;
    opengl::Text::Font m_Font;
    bool m_IsOpen;
};

