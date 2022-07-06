#include "pch.h"
#include "framework.h"
#include "menu.h"

#include "trampoline_hook.h"
#include "memory.h"
#include "aimbot.h"
#include "esp.h"

#include "AssaultCubeAddresses.h"
#include "AssaultCubeStructs.h"

// Allocate and deallocate console
FILE* fpConsoleOutput;

void AllocateConsole() {
    AllocConsole();
    freopen_s(&fpConsoleOutput, "CONOUT$", "w", stdout);
}

void DeallocateConsole() {
    fclose(fpConsoleOutput);
    FreeConsole();
}

// hmodule for the created thread
HMODULE hThread = nullptr;                                              

// typedef for wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
// original wglSwapBuffers function
twglSwapBuffers originalWglSwapBuffers;

// main loop hook (OpenGL.wglSwapBuffers)
Hook::TrampolineHook mainLoopHook = Hook::TrampolineHook();

// opengl ingame menu
Menu* menu = nullptr;

uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");

int* gameMode = nullptr;
int* playerCount = nullptr;

float* matrix = nullptr;

Entity* localPlayer = nullptr;
EnitityList* entityList = nullptr;

Aimbot* aimbot = nullptr;
ESP* esp = nullptr;

void UnlHealthHackCallback() {
    if (localPlayer) {
        localPlayer->Health = 999;
    } else {
        localPlayer = *((Entity**)(moduleBaseAddress + ADDR_LOCAL_PLAYER_ENTITY));
    }
}

void UnlAmmoHackCallback() {
    int* pFirstWeaponAmmo = (int*)memory::FindDMAAddress(moduleBaseAddress + ADDR_FIRST_WEAPON_AMMO, OFF_FIRST_WEAPON_AMMO);
    if (pFirstWeaponAmmo) {
        *pFirstWeaponAmmo = 999;
    }
}

void NoRecoilCallback(bool active) {
    if (active) {
        // instead of running the original calculateRecoil (?) function return directly
        // ac_client.exe+C8BA0 - C2 0800               - ret 0008 { 8 }
        memory::Patch((BYTE*)(moduleBaseAddress + ADDR_NORECOIL_FUNCTION), (BYTE*)"\xC2\x08\x00", 3);
    } else {
        // ac_client.exe+C8BA0 - 83 EC 28              - sub esp,28
        memory::Patch((BYTE*)(moduleBaseAddress + ADDR_NORECOIL_FUNCTION), (BYTE*)"\x83\xEC\x28", 3);
    }
}

void ESPChangeCallback(bool active) {
    if (active && !esp->IsInitialized()) {
        esp->Initialize(localPlayer, entityList, matrix, gameMode, playerCount);
    }

    esp->SetBoxActive(active);
}

void SnaplinesChangeCallback(bool active) {
    if (active && !esp->IsInitialized()) {
        esp->Initialize(localPlayer, entityList, matrix, gameMode, playerCount);
    }

    esp->SetSnaplineActive(active);
}

void AimbotChangeCallback(bool active) {
    if (active && !aimbot->IsInitialized()) {
        aimbot->Initialize(localPlayer, entityList);
    }

    aimbot->SetEnabled(active);
}

void ESPTick() {
    esp->Tick();
}

void AimbotTick() {
    aimbot->Tick();
}

// menu entries
std::vector<MenuEntry> menuEntries = {
    MenuEntry{ "Unl. Health",   false, nullptr,                 UnlHealthHackCallback,  nullptr },
    MenuEntry{ "Unl. Ammo",     false, nullptr,                 UnlAmmoHackCallback,    nullptr },
    MenuEntry{ "No Recoil",     false, NoRecoilCallback,        nullptr,                nullptr },
    MenuEntry{ "ESP",           false, ESPChangeCallback,       nullptr,                ESPTick },
    MenuEntry{ "Snapline",      false, SnaplinesChangeCallback, nullptr,                nullptr },
    MenuEntry{ "Aimbot",        false, AimbotChangeCallback,    nullptr,                AimbotTick },
};

BOOL __stdcall MainLoop(HDC hDc) {
    if (!gameMode) {
        gameMode = (int*)(moduleBaseAddress + ADDR_GAME_MODE);
    }

    if (!playerCount) {
        playerCount = (int*)(moduleBaseAddress + ADDR_NUM_PLAYERS);
    }

    if (!matrix) {
        matrix = (float*)(ADDR_MATRIX);
    }

    if (!localPlayer) {
        localPlayer = *((Entity**)(moduleBaseAddress + ADDR_LOCAL_PLAYER_ENTITY));
    }

    if (!entityList) {
        entityList = *(EnitityList**)(moduleBaseAddress + ADDR_ENTITY_LIST);
    }

    // create menu tick
    menu->Tick();

    if (GetAsyncKeyState(VK_DELETE) & 1) {
        std::cout << "Exit cheat in 15 seconds ..." << std::endl;
        Sleep(15000);

        // deactivate console
        DeallocateConsole();

        mainLoopHook.Disable();
        FreeLibraryAndExitThread((HINSTANCE)hThread, 1);
    }

    return originalWglSwapBuffers(hDc);
}

DWORD __stdcall Thread(HMODULE hModule) {
    hThread = hModule;

    AllocateConsole();
    std::cout << "Thread started" << std::endl;

    menu = new Menu("AssaultHook v1.0", menuEntries);
    esp = new ESP();
    aimbot = new Aimbot();

    // get handle to opengl module
    HMODULE hModuleOpenGL = GetModuleHandle(L"opengl32.dll");

    if (hModuleOpenGL) {
        // get address for wglSwapBuffers function
        originalWglSwapBuffers = (twglSwapBuffers)GetProcAddress(hModuleOpenGL, "wglSwapBuffers");

        if (originalWglSwapBuffers) {
            // create trampoline hook for the function wglSwapBuffers
            mainLoopHook.initialize((BYTE*)originalWglSwapBuffers, (BYTE*)MainLoop, 5);

            // enable the hook -> start MainLoop
            originalWglSwapBuffers = (twglSwapBuffers)mainLoopHook.Enable();
        }
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved) {
    if (dwReasonForCall == DLL_PROCESS_ATTACH) {
        HANDLE hTread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, nullptr);
        
        if (hTread) {
            CloseHandle(hTread);
        }
    }

    return TRUE;
}

