#include "pch.h"
#include "framework.h"
#include "menu.h"

#include "trampoline_hook.h"
#include "memory.h"

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

// assualtcube modulebase address
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

// player entity
Entity* localPlayerEntity = nullptr;

// entity list
EnitityList* entityList = nullptr;

// menu object
Menu* menu = nullptr;

// menu entries
std::vector<MenuEntry> menuEntries = {
    MenuEntry{ 
         "Unl. Ammo",
         false,
         UnlAmmoHack,
         nullptr,
    },
    MenuEntry{
         "Unl. Health",
         false,
         UnlHealthHack,
         nullptr,
    },
};



bool UnlAmmoHack(bool active) {
    return true;
}

bool UnlHealthHack(bool active) {
    return true;
}

bool bUnlimitedHealth = false;
bool bUnlimitedAmmo = false;

bool bDisplayedMenu = false;

BOOL __stdcall MainLoop(HDC hDc) {
    if (!bDisplayedMenu) {
        std::cout << " F1 - Toggle Unl. Health (" << bUnlimitedHealth << ")" << std::endl;
        std::cout << " F2 - Toggle Unl. Ammo (" << bUnlimitedAmmo << ")" << std::endl;
        std::cout << "F10 - Exit cheat \n\n" << std::endl;
        bDisplayedMenu = !bDisplayedMenu;
    }

    if (!localPlayerEntity) {
        localPlayerEntity = (Entity*)memory::FindDMAAddress(moduleBase + 0x17E254, { 0x0 });
    }

    if (localPlayerEntity) {
        std::cout << "Player Position: (x:"<< localPlayerEntity->Position.x << ", y:" << localPlayerEntity->Position.y << ", z:" << localPlayerEntity->Position.z << ")" << std::endl;
    }

    if (!entityList) {
        entityList = (EnitityList*)memory::FindDMAAddress(moduleBase + 0x18AC04, { 0x0 });
    }

    if (entityList) {
        for (int i = 1; i < 32; i++) {
            if (entityList->Entities[i] && entityList->Entities[i]->VTable == 0x0054D07C) {
                std::cout << "Player " << entityList->Entities[i]->Name << ": (x:" << entityList->Entities[i]->Position.x << ", y : " << entityList->Entities[i]->Position.y << ", z : " << entityList->Entities[i]->Position.z << ")" << std::endl;
            } else {
                break;
            }
        }
    }

    if (GetAsyncKeyState(VK_F1) & 1) { bUnlimitedHealth = !bUnlimitedHealth; }
    if (GetAsyncKeyState(VK_F2) & 1) { bUnlimitedAmmo = !bUnlimitedAmmo; }
    if (GetAsyncKeyState(VK_F10) & 1) {
        std::cout << "Exit cheat in 15 seconds ..." << std::endl;
        Sleep(15000);

        // deactivate console
        DeallocateConsole();

        mainLoopHook.Disable();
        FreeLibraryAndExitThread((HINSTANCE)hThread, 1);
    }

    if (bUnlimitedHealth && localPlayerEntity) {
        localPlayerEntity->Health = 100;
    }

    if (bUnlimitedAmmo) {
        int* pFirstWeaponAmmo = (int*)memory::FindDMAAddress(moduleBase + 0x17E0A8, { 0x364, 0x14, 0x0 });
        if (pFirstWeaponAmmo) {
            *pFirstWeaponAmmo = 100;
        }        
    }

    return originalWglSwapBuffers(hDc);
}

DWORD __stdcall Thread(HMODULE hModule) {
    hThread = hModule;

    AllocateConsole();
    std::cout << "Thread started" << std::endl;

    // create menu
    menu = new Menu("AssaultHook", menuEntries);

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

