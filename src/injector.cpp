#include "system.h"
#include <iostream>
#include <filesystem>

#include "memory/memory.h"

#ifdef _LINUX
bool InjectLibrary(uint processIdentifier, const char* pathToLibrary) {

    return true;
}

const char* PROCESS = "ac_client";
const char* FILENAME = "AssaultHook.so";
#endif

#ifdef _WINDOWS
bool InjectLibrary(DWORD processIdentifier, const char* pathToLibrary) {
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processIdentifier);
    if (!process || process == INVALID_HANDLE_VALUE) {
        std::cout << "InjectDll(): Could not open process with identifier \"" << process << "\"" << std::endl;
        return false;
    }

    uintptr_t* allocation = (uintptr_t*)VirtualAllocEx(process, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocation) {
        std::cout << "InjectDll(): Allocation of memory inside the targeted process failed ..." << std::endl;
        return false;
    }

    WriteProcessMemory(process, allocation, pathToLibrary, strlen(pathToLibrary) + 1, NULL);

    HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocation, NULL, NULL);
    if (!thread) {
        return false;
    }

    CloseHandle(thread);
    return true;
}

const char* PROCESS = "ac_client.exe";
const char* FILENAME = "AssaultHook.dll";
#endif

int main() {
    std::filesystem::path pathToLibrary = std::filesystem::current_path() / FILENAME;

    std::cout << "Looking for \"ac_client\" ..." << std::endl;
    uint ac_client = 0;
    while (!ac_client) {
        ac_client = memory::GetProcessIdentifier(PROCESS);
        SysWait(100);
    }

    std::cout << "Injecting Library ..." << std::endl;
    if (InjectLibrary(ac_client, pathToLibrary.string().c_str())) {
        std::cout << "AssaultHook-Injection succesfully ..." << std::endl;
    }
    else {
        std::cout << "AssaultHook-Injection failed ..." << std::endl;
    }

    SysWait(1000);
    return 0;
}
