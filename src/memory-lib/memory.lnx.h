#pragma once
#include "../system.lnx.h"
#include "defines.h"
#include "memory.h"

// linux implementation of memory functions
namespace memory::lnx {
uint GetProcessIdentifier(const char *processName);

bool InjectSharedLibrary(const char* processName, const char* pathToLibrary);

bool InjectSyscall(uint processIdentifier, int syscall_n, void* arg0, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5);

Module GetModule(const char *moduleName);
Module GetModuleEx(uint processIdentifier, const char *moduleName);

addr AllocateMemory(addr source, uint size);
addr AllocateMemoryEx(uint processIdentifier, uint size, int protection);

bool FreeMemory(addr source);
bool FreeMemoryEx(uint processIdentifier, addr source, uint size);

bool ProtectMemory(addr source, uint size, int protection);
bool ProtectMemoryEx(uint processIdentifier, addr source, uint size, int protection);

bool PatchBytes(addr *destination, addr *source, uint size);
bool PatchBytesEx(uint processIdentifier, addr *destination, addr *source, uint size);

bool ReadBytes(addr *source, addr *destination, uint size);
bool ReadBytesEx(uint processIdentifier, addr *source, addr *destination, uint size);

bool NopBytes(addr *destination, uint size);

addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
}; // namespace memory::lnx
