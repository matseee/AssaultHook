#include "hook.h"

memory::Hook::Hook(addr sourceAddress, addr destinationAddress, uint length)
    : Hook(sourceAddress, destinationAddress, length, 0) {};

memory::Hook::Hook(addr sourceAddress, addr destinationAddress, uint length, uint sourcePadding) {
    m_SourceAddress = sourceAddress;
    m_DestinationAddress = destinationAddress;
    m_Length = length;
    m_SourcePadding = sourcePadding;

    m_IsActive = false;
}

memory::Hook::~Hook() {
    Deactivate();
}

bool memory::Hook::Activate() {
    if (!CheckAllowed()) {
        Log::Error() << "Hook::Activate(): Hook is not allowed (missing parameters) ..." << Log::Endl;
        return false;
    }

    if (m_IsActive) {
        Log::Warning() << "Hook::Activate(): Hook is already active ..." << Log::Endl;
        return true;
    }

    if (!Create()) {
        Log::Debug() << "Hook::Activate(): Hook activation failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "Hook::Activate(): Hook activation successful ..." << Log::Endl;
    return m_IsActive = true;
}

bool memory::Hook::Deactivate() {
    if (!m_IsActive) {
        Log::Warning() << "Hook::Deactivate(): Hook is already deactivated ..." << Log::Endl;
        return true;
    }

    if (!Destroy()) {
        Log::Error() << "Hook::Deactivate(): Hook deactivation failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "Hook::Deactivate(): Hook deactivation successful ..." << Log::Endl;
    return !(m_IsActive = false);
}

bool memory::Hook::IsActive() {
    return m_IsActive;
}

bool memory::Hook::CheckAllowed() {
    return m_SourceAddress && m_DestinationAddress && m_Length >= MEMORY_HOOK_MINLENGTH;
}

bool memory::Hook::Create() {
    // also called detour - we simply overwrite some instructions of the targets process with an unconditional
    // jump (JMP) to manipulate the program flow to our needs.
    addr source = (m_SourceAddress + m_SourcePadding);

    // save "stolen bytes" - so that we can restore the original instructions later
    if (!memory::ReadBytes((addr*)source, (addr*)&m_StolenBytes, m_Length)) {
        return false;
    }

    Log::Debug() << "Hook::Create(): Backup \"stolen bytes\" from 0x" << (void*)source << " => ";
    Log::Bytes(m_StolenBytes, m_Length);

    // clear bytes that should be written next
    if (!memory::NopBytes((addr*)source, m_Length)) {
        return false;
    }

    // prepare new jump instruction that gets written into the stolen bytes
    byte bInstruction[MEMORY_HOOK_MINLENGTH];
    bInstruction[0] = MEMORY_ASM_INSTRUCTION_JMP;

#ifdef _WIN64
    // windows 64bit: we have to do an absolute jmp to the destination address
    // absolute JMP instruction + absolute address
    * (addr*)(bInstruction + 8) = m_DestinationAddress;

    Log::Debug() << "Hook::Create(): Write JMP to 0x" << (void*)m_DestinationAddress << " instruction into 0x" << (void*)source << " => ";
    Log::Bytes(bInstruction, m_Length);
#else
    // windows 32bit: we have to do a relative jmp to the destination address
    // calculate the relative destination address 
    addr relativeAddress = m_DestinationAddress - m_SourceAddress - 5;

    // relative JMP instruction + the relative address (jmp "to") 
    *(addr*)(bInstruction + 1) = relativeAddress;

    Log::Debug() << "Hook::Create(): Write JMP to 0x" << (void*)relativeAddress << " instruction into 0x" << (void*)source << " => ";
    Log::Bytes(bInstruction, m_Length);
#endif

    // overwrite "stolen bytes" with jump instruction towards our destination
    if (!memory::PatchBytes((addr*)source, (addr*)&bInstruction, m_Length)) {
        Log::Error() << "Hook::Create(): Write JMP instruction failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "Hook::Create(): Write JMP instruction successful ..." << Log::Endl;
    return true;
}

bool memory::Hook::Destroy() {
    addr source = (m_SourceAddress + m_SourcePadding);

    Log::Debug() << "Hook::Destroy(): Write JMP instruction to 0x" << (void*)source << " => ";
    Log::Bytes(m_StolenBytes, m_Length);

    if (!memory::PatchBytes((addr*)source, (addr*)m_StolenBytes, m_Length)) {
        Log::Error() << "Hook::Destroy(): Write JMP instruction failed ..." << Log::Endl;
        return false;
    }

    Log::Debug() << "Hook::Create(): Write JMP instruction successful ..." << Log::Endl;
    return true;
}
