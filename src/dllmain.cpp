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

// opengl ingame menu
Menu* menu = nullptr;

uintptr_t moduleBaseAddress = (uintptr_t)GetModuleHandle(L"ac_client.exe");
Entity* localPlayerEntity = nullptr;
EnitityList* entityList = nullptr;

void UnlHealthHack() {
    if (localPlayerEntity) {
        localPlayerEntity->Health = 999;
    }
}

void UnlAmmoHack() {
    int* pFirstWeaponAmmo = (int*)memory::FindDMAAddress(moduleBaseAddress + 0x17E0A8, { 0x364, 0x14, 0x0 });
    if (pFirstWeaponAmmo) {
        *pFirstWeaponAmmo = 100;
    }
}

void NoRecoil(bool active) {
    if (active) {
        // instead of running the original calculateRecoil (?) function return directly
        // ac_client.exe+C8BA0 - C2 0800               - ret 0008 { 8 }
        memory::Patch((BYTE*)(moduleBaseAddress + 0xC8BA0), (BYTE*)"\xC2\x08\x00", 3);
    } else {
        // ac_client.exe+C8BA0 - 83 EC 28              - sub esp,28
        memory::Patch((BYTE*)(moduleBaseAddress + 0xC8BA0), (BYTE*)"\x83\xEC\x28", 3);
    }
}

// menu entries
std::vector<MenuEntry> menuEntries = {
    MenuEntry{
         "Unl. Health",
         false,
         nullptr,
         UnlHealthHack,
    },
    MenuEntry{ 
         "Unl. Ammo",
         false,
         nullptr,
         UnlAmmoHack,
    },
    MenuEntry{
         "No Recoil",
         false,
         NoRecoil,
         nullptr,
    },
};

BOOL __stdcall MainLoop(HDC hDc) {
    if (!localPlayerEntity) {
        localPlayerEntity = (Entity*)memory::FindDMAAddress(moduleBaseAddress + 0x17E254, { 0x0 });
    }

    if (localPlayerEntity) {
        std::cout << "Player Position: (x:"<< localPlayerEntity->Position.x << ", y:" << localPlayerEntity->Position.y << ", z:" << localPlayerEntity->Position.z << ")" << std::endl;
    }

    //if (!entityList) {
    //    entityList = (EnitityList*)memory::FindDMAAddress(moduleBaseAddress + 0x18AC04, { 0x0 });
    //}

    //if (entityList) {
    //    for (int i = 1; i < 32; i++) {
    //        if (entityList->Entities[i] && entityList->Entities[i]->VTable == 0x0054D07C) {
    //            std::cout << "Player " << entityList->Entities[i]->Name << ": (x:" << entityList->Entities[i]->Position.x << ", y : " << entityList->Entities[i]->Position.y << ", z : " << entityList->Entities[i]->Position.z << ")" << std::endl;
    //        } else {
    //            break;
    //        }
    //    }
    //}

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

