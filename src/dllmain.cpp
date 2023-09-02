#include "utils/log.h"
#include "menu/menu.h"

#include "memory/trampoline_hook.h"
#include "memory/memory.h"

#include "hacks/aimbot.h"
#include "hacks/memhack.h"
#include "hacks/esp.h"

#include "ac/acAddresses.h"
#include "ac/acStructs.h"
#include "ac/acState.h"

HMODULE hThread = nullptr;

TrampolineHook* trampolineHook = nullptr;
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffers;

AcState* acState = nullptr;
Menu* menu = nullptr;

// hacking loop that is hooked into the opengl wglSwapBuffers function
BOOL __stdcall HookedWglSwapBuffers(HDC hDc) {
	// create menu tick
	menu->Tick();

	if (GetAsyncKeyState(VK_DELETE) & 1) {
		Log::SetActive(false);
		trampolineHook->Destroy();
		FreeLibraryAndExitThread((HINSTANCE)hThread, 0);
	}

	return wglSwapBuffers(hDc);
}

// thread that creates the game hacking objects and hooks the opengl wglSawpBuffers function
DWORD __stdcall Thread(HMODULE hModule) {
	hThread = hModule;

	Log::SetActive(true);
	Log::Info() << "DllMain::Thread(): Initialization started ..." << Log::Endl;

	AcState* acState = AcState::Get();
	while (!acState->IsReady()) {
		Log::Warning() << "DllMain::Thread(): AcState not ready ... trying again ..." << Log::Endl;
	}

	std::vector<MenuEntry> menuEntries = {
		MenuEntry{ "Aimbot", new Aimbot() },
		MenuEntry{ "ESP Box", new ESPBox() },
		MenuEntry{ "ESP Name", new ESPName() },
		MenuEntry{ "ESP Health", new ESPHealth() },
		MenuEntry{ "ESP Line", new ESPLine() },
		MenuEntry{
			"Unl. Health",
			new Freeze<int>(
				(uintptr_t)&acState->LocalPlayer->Health,
				69420
			)
		},
		MenuEntry{
			"Unl. Ammo",
			new Freeze<int>(
				(uintptr_t)memory::FindDMAAddress(
					acState->ModuleBase + ADDR_FIRST_WEAPON_AMMO, OFF_FIRST_WEAPON_AMMO),
				69420
			),
		},
		MenuEntry{
			"No Recoil",
			// instead of running the original calculateRecoil function, return directly
			new Patch(
				(uintptr_t)ADDR_NORECOIL_FUNCTION,
				// ac_client.exe+C8BA0 - C2 08 00              - ret 0008 { 8 }
				(uintptr_t)"\xC2\x08\x00",
				// ac_client.exe+C8BA0 - 83 EC 28              - sub esp,28
				(uintptr_t)"\x83\xEC\x28",
				3
			)
		},
		MenuEntry{
			"Debug Log",
			new Hack(true, [](bool active) { Log::SetActive(active); })
		}
	};

	menu = new Menu("AssaultHook v1.0", menuEntries);

	wglSwapBuffers = (twglSwapBuffers)GetProcAddress((HMODULE)acState->ModuleOpenGl, "wglSwapBuffers");
	if (!wglSwapBuffers) {
		Log::Error() << "DllMain::Thread(): Could not get \"wglSwapBuffers\" address ..." << Log::Endl;
	}
	Log::Debug() << "DllMain::Thread(): GetProcAddress(\"wglSwapBuffers\") => " << (void*)wglSwapBuffers << Log::Endl;

	trampolineHook = new TrampolineHook((uintptr_t)wglSwapBuffers, (uintptr_t)HookedWglSwapBuffers, 5);
	wglSwapBuffers = (twglSwapBuffers)trampolineHook->Create();

	Log::Info() << "DllMain::Thread(): Initialization finished ..." << Log::Endl;

	while (true) {
		Log::Debug() << "Dll::Thread(): Still alive ..." << Log::Endl;
		// Sleep so that the usage of this thread is not 100%
		Sleep(500);
	}

	return 0;
}

// entry point of the dll that creates a new thread
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved) {
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		HANDLE hTread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, nullptr);

		if (hTread) {
			CloseHandle(hTread);
		}
	}

	return TRUE;
}
