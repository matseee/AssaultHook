#include "signature_scanner.h"

memory::SignatureScanner::SignatureScanner(addr moduleBaseAddress, uint moduleSize) {
	m_ModuleBaseAddress = moduleBaseAddress;
	m_ModuleSize = moduleSize;
}

bool memory::SignatureScanner::Scan(Signature* signature) {
	signature->patternLength = strlen(signature->pattern);
	return ScanMemory(m_ModuleBaseAddress, m_ModuleBaseAddress + m_ModuleSize, signature, 1);
}

bool memory::SignatureScanner::ScanMulti(Signature signatures[], uint signatureCount) {
	for (uint i = 0; i < signatureCount; ++i) {
		signatures[i].patternLength = strlen(signatures[i].pattern);
	}

	return ScanMemory(m_ModuleBaseAddress, m_ModuleBaseAddress + m_ModuleSize, signatures, signatureCount);
}

bool memory::SignatureScanner::ScanMemory(addr fromAddress, addr toAddress, Signature signatures[], uint signatureCount) {
	uint searchRangeSize = toAddress - fromAddress;
	uint signatureComplete = 0;

	// TODO: Check if it is enough space left inside the module

	for (uint memoryIndex = 0; memoryIndex < searchRangeSize; ++memoryIndex) {
		for (uint signatureIndex = 0; signatureIndex < signatureCount; ++signatureIndex) {
			if (signatures[signatureIndex].address != NULL) {
				continue;
			}

			if (CheckSignature(fromAddress + memoryIndex, &signatures[signatureIndex])) {
				signatureComplete++;
			}
		}

		if (signatureComplete == signatureCount) {
			return true;
		}
	}
	return false;
}

bool memory::SignatureScanner::CheckSignature(addr address, Signature* signature) {
	for (uint signatureIndex = 0; signatureIndex < signature->patternLength; ++signatureIndex) {
		if (signature->mask[signatureIndex] != '?'
			&& signature->pattern[signatureIndex] != *(char*)(address + signatureIndex)) {
			return false;
		}
	}

	signature->address = address;
	return true;
}

