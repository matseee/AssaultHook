#include "trampoline_hook.h"

memory::TrampolineHook::TrampolineHook(const char* moduleName, const char* procName, addr destinationAddress, uint length)
    : TrampolineHook(GetModuleHandleA(moduleName), procName, destinationAddress, length) {}

memory::TrampolineHook::TrampolineHook(HMODULE module, const char* procName, addr destinationAddress, uint length)
    : TrampolineHook((addr)GetProcAddress(module, procName), destinationAddress, length) {}

memory::TrampolineHook::TrampolineHook(addr sourceAddress, addr destinationAddress, uint length) {
    m_SourceAddress = sourceAddress;
    m_DestinationAddress = destinationAddress;
    m_Length = length;
    m_GatewayAddress = 0;

    if (!CheckAllowed()) {
        return;
    }
    if (!CreateGatewayWithHook()) {
        return;
    }
}

memory::TrampolineHook::~TrampolineHook() {
    Destroy();
}

bool memory::TrampolineHook::Activate() {
    if (!m_GatewayHook || !m_GatewayHook->IsActive()) {
        Log::Error() << "TrampolineHook::Activate(): Gateway is not created ..." << Log::Endl;
        return false;
    }

    if (!CreateSourceHook()) {
        return false;
    }

    Log::Info() << "TrampolineHook::Activate(): Activation of TrampolineHook successful ..." << Log::Endl;
    return true;
}

bool memory::TrampolineHook::Deactivate() {
    if (!m_SourceHook || !m_SourceHook->IsActive()) {
        Log::Error() << "TrampolineHook::Deactivate(): Hook is not active ..." << Log::Endl;
        return false;
    }

    Log::Info() << "TrampolineHook::Deactivate(): Deactivation of TrampolineHook successful ..." << Log::Endl;
    return DeactivateSourceHook();
}

addr memory::TrampolineHook::GetGateway() {
    return m_GatewayAddress;
}

void memory::TrampolineHook::Destroy() {
    if (!DeactivateSourceHook()) {
        return;
    }
    if (!DestroyGateway()) {
        return;
    }
    Log::Info() << "TrampolineHook::Destroy(): Trampoline destroyed successfully ..." << Log::Endl;
}

bool memory::TrampolineHook::CheckAllowed() {
    if (!m_SourceAddress || !m_DestinationAddress) {
        Log::Error() << "TrampolineHook::CheckAllowed(): \"m_SourceAddress\" or \"m_DestinationAddress\" not set ..." << Log::Endl;
        return false;
    }
    if (m_Length < MEMORY_HOOK_MINLENGTH) {
        Log::Error() << "TrampolineHook::CheckAllowed(): \"m_Length\" is smaller than " << MEMORY_HOOK_MINLENGTH << " ..." << Log::Endl;
        return false;
    }
    return true;
}

bool memory::TrampolineHook::CreateGatewayWithHook() {
    // allocate memory for gateway
    m_GatewayAddress = memory::AllocateMemory(0, m_Length);
    if (!m_GatewayAddress) {
        Log::Error() << "TrampolineHook::CreateGatewayWithHook(): AllocateMemory() failed ..." << Log::Endl;
        return false;
    }

    // write the stolen bytes to the previously allocated memory
    if (!memory::PatchBytes((addr*)m_GatewayAddress, (addr*)m_SourceAddress, m_Length)) {
        Log::Error() << "TrampolineHook::CreateGatewayWithHook(): Writing stolen bytes to gateway failed ..." << Log::Endl;
        return false;
    }

    // create a hook from the gateway to the hooked (source) function
    m_GatewayHook = new memory::Hook(m_GatewayAddress, m_SourceAddress, m_Length, m_Length);

    if (!m_GatewayHook->Activate()) {
        Log::Error() << "TrampolineHook::CreateGatewayWithHook(): Creation of hook from gateway to source failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "TrampolineHook::CreateGatewayWithHook(): 0x" << (void*)m_GatewayAddress << " => 0x" << (void*)m_SourceAddress << " successful ..." << Log::Endl;
    return true;
}

bool memory::TrampolineHook::CreateSourceHook() {
    // creates a hook from the source function to our own destination function.
    m_SourceHook = new memory::Hook(m_SourceAddress, m_DestinationAddress, m_Length);

    if (!m_SourceHook->Activate()) {
        Log::Error() << "TrampolineHook::CreateSourceHook(): 0x" << (void*)m_SourceAddress << " => 0x" << (void*)m_DestinationAddress << " failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "TrampolineHook::CreateSourceHook(): 0x" << (void*)m_SourceAddress << " => 0x" << (void*)m_DestinationAddress << " successful ..." << Log::Endl;
    return true;
}

bool memory::TrampolineHook::DeactivateSourceHook() {
    // by patching the stolen bytes back to the destination function.
    if (!m_SourceHook || !m_SourceHook->IsActive()) {
        Log::Warning() << "TrampolineHook::DeactivateSourceHook(): No hook active ..." << Log::Endl;
        return true;
    }

    if (!m_SourceHook->Deactivate()) {
        Log::Error() << "TrampolineHook::DeactivateSourceHook(): Restoring \"stolen bytes\" failed ..." << Log::Endl;
        return false;
    }

    return true;
}

bool memory::TrampolineHook::DestroyGateway() {
    // by releasing the allocated memory for the gateway
    if (!m_GatewayAddress) {
        Log::Warning() << "TrampolineHook::DestroyGateway(): No gateway available ..." << Log::Endl;
        return true;
    }

    if (!memory::FreeMemory(m_GatewayAddress)) {
        Log::Error() << "TrampolineHook::DestroyGateway(): FreeMemory of 0x" << (void*)m_GatewayAddress << " failed ..." << Log::Endl;
        return false;
    }
    return true;
}
