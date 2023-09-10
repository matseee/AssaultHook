#include "utils/log.h"
#include "menu/menu.h"

#include "memory/trampoline_hook.h"
#include "memory/memory.h"

#include "hacks/aimbot.h"
#include "hacks/health.h"
#include "hacks/memhack.h"
#include "hacks/esp.h"

#include "ac/acState.h"

const char titleAssaultHook[] = ">> AssaultHook <<";
bool bDestroy = false;

AcState* acState = nullptr;
Menu* menu = nullptr;
memory::TrampolineHook* trampolineHook = nullptr;

// Typedefinition of the opengl32.ddl wglSwapBuffers function
typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers wglSwapBuffersGateway;

// Trampoline-Hooked opengl32.dll wglSwapBuffers function. It gets called at the end of every frame
BOOL __stdcall HookedWglSwapBuffers(HDC hDc) {

	// handle menu-navigation and also forwards the tick to the hack-objects
	menu->Tick();

	// unload assaulthook when DELETE was pressed
	if (GetAsyncKeyState(VK_DELETE) & 1) {
		// first deactivate the trampoline hook
		// objects get deleted in the main thread
		trampolineHook->Deactivate();
		bDestroy = true;
	}

	if (!wglSwapBuffersGateway) {
		wglSwapBuffersGateway = (twglSwapBuffers)trampolineHook->GetGateway();
	}

	// call original wglSwapBuffer through the trampoline gateway
	return wglSwapBuffersGateway(hDc);
}

// Main thread of assaulthook. It creates the Log-, AcState-, Menu(its entries,
// including the hacks) and the trampolinehook.
DWORD __stdcall Thread(HMODULE hModule) {
	Log::SetActive(true);
	Log::Info() << titleAssaultHook << Log::Endl;
	Log::Info() << "DllMain::Thread(): Initialization started ..." << Log::Endl;

	AcState* acState = AcState::Get();
	while (!acState->IsReady()) {
		Log::Warning() << "DllMain::Thread(): AcState not ready ... trying again ..." << Log::Endl;
		Sleep(50);
	}

	menu = new Menu(std::vector<MenuEntry> {
		MenuEntry{ ">> AssaultHook <<", nullptr, MenuEntryType::TITLE },
		MenuEntry{ "", nullptr, MenuEntryType::SPACER },
		MenuEntry{ "Aimbot", new Aimbot() },
		MenuEntry{ "", nullptr, MenuEntryType::SPACER },
		MenuEntry{ "ESP Box", new ESPBox() },
		MenuEntry{ "ESP Name", new ESPName() },
		MenuEntry{ "ESP Health", new ESPHealth() },
		MenuEntry{ "ESP Line", new ESPLine() },
		MenuEntry{ "", nullptr, MenuEntryType::SPACER },
		MenuEntry{ "Unl. Health", new Health() },
		MenuEntry{ "Unl. Ammo",
			new Patch(
				(addr)(acState->ModuleBase + ADDR_DECREASE_AMMO_FUNCTION),
				(addr)"\x90\x90", // ac_client.exe+C73EF - 90 90    - nop
				(addr)"\xFF\x08", // ac_client.exe+C73EF - FF 08    - dec[eax]
				2
			),
		},
		MenuEntry{ "No Recoil",
			// instead of running the original calculateRecoil function, return directly
			new Patch(
				(addr)(acState->ModuleBase + ADDR_NORECOIL_FUNCTION),
				(addr)"\xC2\x08\x00", // ac_client.exe+C8BA0 - C2 08 00    - ret 0008 { 8 }
				(addr)"\x83\xEC\x28", // ac_client.exe+C8BA0 - 83 EC 28    - sub esp,28
				3
			)
		},
		MenuEntry{ "", nullptr, MenuEntryType::SPACER },
		MenuEntry{ "Debug Log",
			new Hack(true, [](bool active) { Log::SetActive(active); })
		}
	});

	trampolineHook = new memory::TrampolineHook("opengl32.dll", "wglSwapBuffers", (addr)HookedWglSwapBuffers, 5);

	if (!trampolineHook->Activate()) {
		Log::Error() << "DllMain::Thread(): Initialization failed ..." << Log::Endl;
		return 1;
	}

	Log::Info() << "DllMain::Thread(): Initialization finished ..." << Log::Endl;
	
	while (!bDestroy) {
		Sleep(200);
	}

	Log::Info() << "DllMain::Thread(): Destroying allocated resources ..." << Log::Endl;
	Sleep(200);

	// delete assaulthook objects
	delete menu;
	delete trampolineHook;
	AcState::Destroy();
	Log::Destroy();

	Sleep(200);

	// exit thread and unload dll
	FreeLibraryAndExitThread((HINSTANCE)hModule, 0);
	return 0;
}

// Entry point after the .dll injection. It spawns the main thread.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved) {
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		HANDLE hTread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, nullptr);

		if (hTread) {
			CloseHandle(hTread);
		}
	}

	return TRUE;
}
