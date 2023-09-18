#pragma once
#include "../system.h"
#include "defines.h"

namespace memory {
struct Module {
  addr StartAddress;
  addr EndAddress;
  long Size;
  uint PID;
};

uint GetProcessIdentifier(const char *processName);
Module GetModule(const char *moduleName, uint pid = 0);

addr AllocateMemory(addr source, uint size);
bool FreeMemory(addr source);

bool PatchBytes(addr *destination, addr *source, uint size);
bool ReadBytes(addr *source, addr *destination, uint size);
bool NopBytes(addr *destination, uint size);

addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
}; // namespace memory
