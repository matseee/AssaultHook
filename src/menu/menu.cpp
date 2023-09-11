#include "menu.h"

Menu::Menu(std::vector<MenuEntry> vEntries) {
    m_IsOpen = false;
    m_Entries = vEntries;

    for (unsigned int i = 0; i < m_Entries.size(); ++i) {
        m_SelectedEntry = m_Entries[i];
        m_SelectedIndex = i;
        if (m_SelectedEntry.type == MenuEntryType::HACK) {
            break;
        }
    }
}

Menu::~Menu() {
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        if (m_Entries[i].type != MenuEntryType::HACK) {
            continue;
        }
        m_Entries[i].hack->Deactivate();
    }
}

void Menu::Tick() {
    Input();
    Render();
}

void Menu::Input() {
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        m_IsOpen = !m_IsOpen;
        Log::Debug() << "Menu::Input(): Menu => " << (m_IsOpen ? "ON" : "OFF") << Log::Endl;
    }
    if (!m_IsOpen) {
        return;
    }

    if (GetAsyncKeyState(VK_UP) & 1) { Previous(0); }
    if (GetAsyncKeyState(VK_DOWN) & 1) { Next(0); }

    m_SelectedEntry = m_Entries[m_SelectedIndex];
    if (GetAsyncKeyState(VK_LEFT) & 1 && m_SelectedEntry.hack->IsActive()) {
        m_SelectedEntry.hack->Deactivate();
        Log::Debug() << "Menu::Input(): " << m_SelectedEntry.name << " => OFF" << Log::Endl;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 1 && !m_SelectedEntry.hack->IsActive()) {
        m_SelectedEntry.hack->Activate();
        Log::Debug() << "Menu::Input(): " << m_SelectedEntry.name << " => ON" << Log::Endl;
    }
}

void Menu::Render() {
    opengl::SetupOrtho();
    ForwardTick();
    RenderMenu();
    opengl::RestoreGL();
}

void Menu::RenderMenu() {
    if (!m_IsOpen) {
        return;
    }

    HDC currentHDC = wglGetCurrentDC();

    geometry::Rect menuRect(MENU_POSITION, MENU_WIDTH, CalculateMenuHeight());
    float currentPosY = MENU_POSITION.y;

    if (!m_Font.bBuilt || currentHDC != m_Font.hdc) {
        m_Font.Build((int)FONT_HEIGHT);
    }

    opengl::Draw::FilledRect(menuRect, opengl::Color::BLACK);

    for (unsigned int i = 0; i < m_Entries.size(); ++i) {
        MenuEntry entry = m_Entries[i];
        currentPosY += entry.GetHeight();

        if (entry.type == MenuEntryType::SPACER) {
            continue;
        }

        if (entry.type == MenuEntryType::TITLE) {
            m_Font.Print(m_Font.CenterText(menuRect.x, menuRect.width, (float)(strlen(entry.name) * FONT_WIDTH)), currentPosY, opengl::Color::WHITE, "%s", entry.name);
            continue;
        }

        if (i == m_SelectedIndex) {
            m_Font.Print(menuRect.x + 5.0f, currentPosY, opengl::Color::WHITE, "%s", entry.name);
        }
        else {
            m_Font.Print(menuRect.x + 5.0f, currentPosY, opengl::Color::GRAY, "%s", entry.name);
        }

        if (entry.hack->IsActive()) {
            const GLubyte* green = (i == m_SelectedIndex) ? opengl::Color::GREENLIGHT : opengl::Color::GREEN;
            m_Font.Print(m_Font.EndText(menuRect.x, menuRect.width, ((float)strlen("ON") * FONT_WIDTH)), currentPosY, green, "%s", "ON");
        }
        else {
            const GLubyte* red = (i == m_SelectedIndex) ? opengl::Color::REDLIGHT : opengl::Color::RED;
            m_Font.Print(m_Font.EndText(menuRect.x, menuRect.width, ((float)strlen("OFF") * FONT_WIDTH)), currentPosY, red, "%s", "OFF");
        }
    }
}

float Menu::CalculateMenuHeight() {
    float menuHeight = 5.0f;
    for (unsigned int i = 0; i < m_Entries.size(); ++i) {
        menuHeight += m_Entries[i].GetHeight();
    }
    return menuHeight;
}

void Menu::ForwardTick() {
    for (int i = 0; i < (int)m_Entries.size(); ++i) {
        if (m_Entries[i].hack) {
            m_Entries[i].hack->Tick();
        }
    }
}

void Menu::Next(int iSkip) {
    if (m_SelectedIndex + 1 == (int)m_Entries.size()) {
        m_SelectedIndex -= iSkip;
        return;
    }

    m_SelectedIndex++;
    if (m_Entries[m_SelectedIndex].type != MenuEntryType::HACK) {
        Next(iSkip + 1);
    }
}

void Menu::Previous(int iSkip) {
    if (m_SelectedIndex - 1 == 0) {
        m_SelectedIndex += iSkip;
        return;
    }

    m_SelectedIndex--;
    if (m_Entries[m_SelectedIndex].type != MenuEntryType::HACK) {
        Previous(iSkip + 1);
    }
}
