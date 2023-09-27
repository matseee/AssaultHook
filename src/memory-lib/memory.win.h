#pragma once
#include "../system.win.h"
#include "defines.h"
#include "memory.h"

// windows implementation of memory functions
namespace memory::win {
uint GetProcessIdentifier(const char *processName);

bool InjectSharedLibrary(const char* processName, const char* pathToLibrary);

Module GetModule(const char *moduleName);
Module GetModuleEx(uint processIdentifier, const char *moduleName);

addr AllocateMemory(addr source, uint size);
addr AllocateMemoryEx(uint processIdentifier, addr source, uint size);

bool FreeMemory(addr source);
bool FreeMemoryEx(uint processIdentifier, addr source);

bool PatchBytes(addr *destination, addr *source, uint size);
bool PatchBytesEx(uint processIdentifier, addr *destination, addr *source, uint size);

bool ReadBytes(addr *source, addr *destination, uint size);
bool ReadBytesEx(uint processIdentifier, addr *source, addr *destination, uint size);

bool NopBytes(addr *destination, uint size);

addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
}; // namespace memory::lnx
