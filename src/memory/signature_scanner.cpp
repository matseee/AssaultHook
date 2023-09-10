#include "signature_scanner.h"

memory::SignatureScanner::SignatureScanner(addr moduleBaseAddress, uint moduleSize, uint chunkSize, uint multiScanThreadCount) {
	m_ModuleBaseAddress = moduleBaseAddress;
	m_ModuleSize = moduleSize;
	m_ChunkSize = chunkSize;
	m_MultiScanThreadCount = multiScanThreadCount;
}

bool memory::SignatureScanner::Scan(Signature* signature) {
	return ScanMemory(m_ModuleBaseAddress, m_ModuleBaseAddress + m_ModuleSize, signature);
}

bool memory::SignatureScanner::ScanMulti(Signature signatures[]) {
	uint signatureCount = sizeof(signatures) / sizeof(Signature);
	// TODO: implement me:
	// - devide module into chunks
	// - spread the chunks in new arrays, for each thread one array
	// - spawn the threads with the ScanMemory function and its subarrays
	return true;
}

bool memory::SignatureScanner::ScanMemory(addr fromAddress, addr toAddress, Signature* signature) {
	signature->patternLength = strlen(signature->pattern);

	uint searchRangeSize = toAddress - fromAddress - signature->patternLength;
	for (uint memoryIndex = 0; memoryIndex < searchRangeSize; ++memoryIndex) {
		bool match = true;

		for (uint signatureIndex = 0; signatureIndex < signature->patternLength; ++signatureIndex) {
			if (signature->mask[signatureIndex] != '?'
				&& signature->pattern[signatureIndex] != *(char*)(fromAddress + memoryIndex + signatureIndex)) {
				match = false;
				break;
			}
		}

		if (match) {
			signature->address = fromAddress + memoryIndex;
			return true;
		}
	}

	return false;
}

uint memory::SignatureScanner::GetChunkCount() {
	return m_ModuleSize / m_ChunkSize + (m_ModuleSize % m_ChunkSize == 0 ? 0 : 1);
}
