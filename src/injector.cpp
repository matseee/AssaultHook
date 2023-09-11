#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>

DWORD GetProcessIdentifier(const char* processName) {
    HANDLE processList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (!Process32First(processList, &entry)) {
        return NULL;
    }

    do {
        if (!_stricmp(entry.szExeFile, processName)) {
            CloseHandle(processList);
            return entry.th32ProcessID;
        }
    } while (Process32Next(processList, &entry));

    return NULL;
}

bool InjectDLL(DWORD processIdentifier, const char* pathToDll) {
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

    WriteProcessMemory(process, allocation, pathToDll, strlen(pathToDll) + 1, NULL);

    HANDLE thread = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocation, NULL, NULL);
    if (!thread) {
        return false;
    }

    CloseHandle(thread);
    return true;
}

int main() {
    const char* processName = "ac_client.exe";
    const char* pathToDll = "D:\\sonstige\\source\\repos\\AssaultHook\\src\\Debug\\AssaultHook.dll";

    std::cout << "Looking for \"ac_client.exe\" ..." << std::endl;
    DWORD ac_client = NULL;
    while (!ac_client) {
        ac_client = GetProcessIdentifier(processName);
        Sleep(100);
    }

    std::cout << "Injecting DLL ..." << std::endl;
    if (InjectDLL(ac_client, pathToDll)) {
        std::cout << "AssaultHook-Injection succesfully ..." << std::endl;
    }
    else {
        std::cout << "AssaultHook-Injection failed ..." << std::endl;
    }

    Sleep(1000);
    return 0;
}
