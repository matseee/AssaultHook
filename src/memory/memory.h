#pragma once
#include "../system.h"
#include "defines.h"

namespace memory {
    uint GetProcessIdentifier(const char* processName);
    addr GetModuleBaseAddress(const char* moduleName);
    uint GetModuleSize(addr moduleBaseAddress);

    addr AllocateMemory(addr source, uint size);
    bool FreeMemory(addr source);

    bool PatchBytes(addr* destination, addr* source, uint size);
    bool ReadBytes(addr* source, addr* destination, uint size);
    bool NopBytes(addr* destination, uint size);

    addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
};
