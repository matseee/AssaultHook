#include "memory.h"

#ifdef _LINUX
addr memory::GetModuleBaseAddress(const char* moduleName) { return 0; }

uint memory::GetModuleSize(addr moduleBaseAddress) { return 0; }

addr memory::AllocateMemory(addr source, uint size) { return 0; }

bool memory::FreeMemory(addr source) { return true; }

bool memory::PatchBytes(addr* destination, addr* source, uint size) { return false; }

bool memory::ReadBytes(addr* source, addr* destination, uint size) { return false; }

bool memory::NopBytes(addr* destination, uint size) { return false; }

addr memory::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) { return 0; }
#endif

uint memory::GetProcessIdentifier(const char* processName) {
#ifdef _LINUX
    uint pid = 0;
    DIR* pDirectory = opendir("/proc");
    if (!pDirectory) {
	return 0;
    }

    struct dirent* pDirEntry;
    while (pid == 0 && (pDirEntry = readdir(pDirectory))) {
        uint id = (uint)atoi(pDirEntry->d_name);
	if (id > 0) {
            std::stringstream cmdlinePath;
	    cmdlinePath << "/proc/" << id << "/cmdline";

	    std::ifstream cmdlineFS(cmdlinePath.str(), std::ios::binary);
	    if (!cmdlineFS.is_open()) {
	        continue;
	    }

	    std::stringstream cmdline;
	    cmdline << cmdlineFS.rdbuf();

	    int found = cmdline.str().find(processName);
	    if (found != std::string::npos) {
                pid = id;
	    	cmdlineFS.close();
		break;
	    }
	    cmdlineFS.close();
	}
    }

    closedir(pDirectory);
    return pid;
#endif
#ifdef _WINDOWS
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
#endif
}

#ifdef _WINDOWS
addr memory::GetModuleBaseAddress(const char* moduleName) {
    return (addr)GetModuleHandle(moduleName);
}

uint memory::GetModuleSize(addr moduleBaseAddress) {
    MODULEINFO moduleInfo = {};
    GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBaseAddress, &moduleInfo, sizeof(moduleInfo));

    if (!moduleInfo || !moduleInfo.SizeOfImage) {
        return 0;
    }
    return moduleInfo.SizeOfImage;
}

addr memory::AllocateMemory(addr source, uint size) {
    return (addr)VirtualAlloc((addr*)source, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

bool memory::FreeMemory(addr source) {
    return VirtualFree((addr*)source, 0, MEM_RELEASE) == 0;
}

bool memory::PatchBytes(addr* destination, addr* source, uint size) {
    ulong protection;
    if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
        return false;
    }
    if (memcpy_s((addr*)destination, size, source, size) != 0) {
        return false;
    }
    return VirtualProtect(destination, size, protection, &protection);
}

bool memory::ReadBytes(addr* source, addr* destination, uint size) {
    ulong protection;
    if (!VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &protection)) {
        return false;
    }
    if (memcpy_s((addr*)destination, size, source, size) != 0) {
        return false;
    }
    return VirtualProtect(source, size, protection, &protection);
}

bool memory::NopBytes(addr* destination, uint size) {
    ulong protection;
    if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
        return false;
    }
    memset(destination, MEMORY_ASM_INSTRUCTION_NOP, size);
    return VirtualProtect(destination, size, protection, &protection) != 0;
}

addr memory::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
    addr address = baseAddress;
    for (uint i = 0; i < offsets.size(); ++i) {
        address = *(addr*)address;

        if (!address) {
            return address;
        }

        address += offsets[i];
    }
    return address;
}
#endif
