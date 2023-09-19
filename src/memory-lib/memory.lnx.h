#pragma once
#include "../system.lnx.h"
#include "defines.h"
#include "memory.h"

// linux implementation of memory functions
namespace memory::lnx {
uint GetProcessIdentifier(const char *processName);
Module GetModule(const char *moduleName, uint pid = 0);

addr AllocateMemory(addr source, uint size);
bool FreeMemory(addr source);

bool PatchBytes(addr *destination, addr *source, uint size);
bool ReadBytes(addr *source, addr *destination, uint size);
bool NopBytes(addr *destination, uint size);

addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
}; // namespace memory::lnx
