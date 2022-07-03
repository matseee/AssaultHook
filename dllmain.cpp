#include "pch.h"
#include "framework.h"
#include "trampoline_hook.h"
#include "memory.h"

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

// typedef for wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

// original wglSwapBuffers function
twglSwapBuffers originalWglSwapBuffers;

// main loop hook (OpenGL.wglSwapBuffers)
Hook::TrampolineHook mainLoopHook = Hook::TrampolineHook();

// AssualtCube modulebase address
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

BOOL __stdcall MainLoop(HDC hDc) {
    std::cout << "HOOKED BITCH!" << std::endl;


    // DO SOMETHING HERE


    if (GetAsyncKeyState(VK_F10) & 1) {
        mainLoopHook.Disable();
    }

    return originalWglSwapBuffers(hDc);
}

DWORD __stdcall Thread(HMODULE hModule) {
    // activate console
    AllocateConsole();

    std::cout << "Thread started" << std::endl;
    Sleep(1000);

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

    //AllocConsole();
    //freopen_s(&pFile, "CONOUT$", "w", stdout);

    //std::cout << "Injected successfully :))" << std::endl << std::endl;

    //while (true) {
    //    if (GetAsyncKeyState(VK_F10) & 1) {
    //        break;
    //    }

    //    Sleep(20);
    //}

    Sleep(15000);

    FreeLibraryAndExitThread((HINSTANCE)hModule, 1);
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

