#pragma once
#include <sstream>
#include <iostream>
#include <vector>
#include "memory.h"

#define MEMORY_SIGNATURE_WILDCARD_BYTE 0xFF
#define MEMORY_SIGNATURE_MAX_SIZE 255

namespace memory {
    struct Signature {
        const char* pattern;
        uint addressByteOffset;
        addr address = 0;

        uint length = 0;
        char bytes[MEMORY_SIGNATURE_MAX_SIZE] = {};
        char byteMask[MEMORY_SIGNATURE_MAX_SIZE] = {};

        Signature(const char* pattern) : Signature(pattern, 0) {};
        Signature(const char* pattern, uint addressByteOffset);

    private:
        void AddByte(uint byte, char mask);
        uint Decode(const char* input);
    };

    class SignatureScanner {
    public:
        SignatureScanner(addr moduleBaseAddress, uint moduleSize);
        ~SignatureScanner() {};

        bool Scan(Signature* signature);
        bool ScanMulti(Signature signatures[], uint signatureCount);

    protected:
        bool ScanMemory(addr from, addr to, Signature signature[], uint signatureCount);
        bool CheckSignature(addr address, Signature* signature);

        addr m_ModuleBaseAddress;
        uint m_ModuleSize;
    };
}
