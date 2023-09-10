#pragma once
#include "memory.h"

namespace memory {
	struct Signature {
		const char* pattern;
		const char* mask;
		uint patternLength = NULL;
		addr address = NULL;
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