#pragma once
#include "../system.h"
#include "defines.h"
#include "memory.h"

namespace memory {
    class Hook {
    public:
        Hook(addr sourceAddress, addr destinationAddress, uint length);
        Hook(addr sourceAddress, addr destinationAddress, uint length, uint sourcePadding);
        ~Hook();

        bool Activate();
        bool Deactivate();
        bool IsActive();

    protected:
        bool CheckAllowed();
        bool Create();
        bool Destroy();

        addr m_SourceAddress;
        addr m_DestinationAddress;
        uint m_Length;
        uint m_SourcePadding;

        bool m_IsActive;
        byte m_StolenBytes[20];
    };
};
