#pragma once
#include "memory.h"

namespace memory {
	struct Signature {
		const char* pattern;
		const char* mask;
		uint patternLength = 0;
		addr address = 0;
	};

	class SignatureScanner {
	public:
		SignatureScanner(addr moduleBaseAddress, uint moduleSize)
			: SignatureScanner(moduleBaseAddress, moduleSize, 4096, 1) {};
		SignatureScanner(addr moduleBaseAddress, uint moduleSize, uint chunkSize)
			: SignatureScanner(moduleBaseAddress, moduleSize, chunkSize, 1) {};
		SignatureScanner(addr moduleBaseAddress, uint moduleSize, uint chunkSize, uint multiScanThreadCount);
		~SignatureScanner() {};

		bool Scan(Signature* signature);
		bool ScanMulti(Signature signature[]);

	protected:
		bool ScanMemory(addr from, addr to, Signature* signature);
		uint GetChunkCount();

		addr m_ModuleBaseAddress;
		uint m_ModuleSize;
		uint m_ChunkSize;
		uint m_MultiScanThreadCount;
	};
}