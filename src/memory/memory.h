#pragma once
#include <windows.h>
#include <vector>

#include "../utils/log.h"

#include "defines.h"

namespace memory {
    addr AllocateMemory(addr source, uint size);
    bool FreeMemory(addr source);

    bool PatchBytes(addr* destination, addr* source, uint size);
    bool ReadBytes(addr* source, addr* destination, uint size);
    bool NopBytes(addr* destination, uint size);

    addr FindDMAAddress(addr baseAddress, std::vector<uint> offsets);
};