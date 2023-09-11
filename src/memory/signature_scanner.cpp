#include "signature_scanner.h"

memory::Signature::Signature(const char* pattern, uint addressByteOffset) {
	this->pattern = pattern;
	this->addressByteOffset = addressByteOffset;

	for (uint i = 0, charLength = 0; pattern[i] != '\0'; i++) {
		if (pattern[i] == ' ') {
			continue;
		} else if (pattern[i] == '?') {
			addByte(MEMORY_SIGNATURE_WILDCARD_BYTE, '?');
			charLength = 0;
		} else {
			charLength++;
		}

		if (charLength == 2) {
			addByte(decode(pattern + i - 1), 'x');
			charLength = 0;
		}
	}
}

void memory::Signature::addByte(uint byte, char mask) {
	bytes[length] = byte;
	byteMask[length] = mask;
	length++;
}

uint memory::Signature::decode(const char* input) {
	uint out, i, t, hn, ln;
	for (t = 0, i = 0; i < 2; i += 2, ++t) {
		hn = input[i] > '9' ? input[i] - 'A' + 10 : input[i] - '0';
		ln = input[i + 1] > '9' ? input[i + 1] - 'A' + 10 : input[i + 1] - '0';
		((uint*)&out)[t] = (hn << 4) | ln;
	}
	return out;
}

memory::SignatureScanner::SignatureScanner(addr moduleBaseAddress, uint moduleSize) {
	m_ModuleBaseAddress = moduleBaseAddress;
	m_ModuleSize = moduleSize;
}

bool memory::SignatureScanner::Scan(Signature* signature) {
	return ScanMemory(m_ModuleBaseAddress, m_ModuleBaseAddress + m_ModuleSize, signature, 1);
}

bool memory::SignatureScanner::ScanMulti(Signature signatures[], uint signatureCount) {
	return ScanMemory(m_ModuleBaseAddress, m_ModuleBaseAddress + m_ModuleSize, signatures, signatureCount);
}

bool memory::SignatureScanner::ScanMemory(addr fromAddress, addr toAddress, Signature signatures[], uint signatureCount) {
	uint searchRangeSize = toAddress - fromAddress;
	uint signatureComplete = 0;

	for (uint memoryIndex = 0; memoryIndex < searchRangeSize; memoryIndex++) {
		for (uint signatureIndex = 0; signatureIndex < signatureCount; signatureIndex++) {
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
	for (uint signatureIndex = 0; signatureIndex < signature->length; signatureIndex++) {
		if (signature->byteMask[signatureIndex] == '?') {
			continue;
		}
		if(signature->bytes[signatureIndex] != *(char*)(address + signatureIndex)) {
			return false;
		}
	}

	signature->address = address + signature->addressByteOffset;
	return true;
}

