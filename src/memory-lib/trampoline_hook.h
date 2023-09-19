#pragma once
#include "defines.h"
#include "memory.h"
#include "hook.h"
#include "../utils/log.h"

namespace memory {
    class TrampolineHook {
    public:
        TrampolineHook(const char* moduleName, const char* processName, addr destinationAddress, uint length);
        TrampolineHook(HMODULE module, const char* processName, addr destinationAddress, uint length);
        TrampolineHook(addr sourceAddress, addr destinationAddress, uint length);
        ~TrampolineHook();

        bool Activate();
        bool Deactivate();
        void Destroy();

        addr GetGateway();

    protected:
        bool CheckAllowed();
        bool CreateGatewayWithHook();
        bool CreateSourceHook();

        bool DestroyGateway();
        bool DeactivateSourceHook();

        addr m_SourceAddress;
        addr m_DestinationAddress;
        uint m_Length;

        addr m_GatewayAddress;
        memory::Hook* m_GatewayHook;

        memory::Hook* m_SourceHook;
    };
};