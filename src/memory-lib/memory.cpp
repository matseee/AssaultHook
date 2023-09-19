#include "memory.h"

#define _LINUX

#ifdef _LINUX
#include "memory.lnx.h"
#else
#include "memory.win.h"
#endif

uint memory::GetProcessIdentifier(const char *processName) {
#ifdef _LINUX
  return memory::lnx::GetProcessIdentifier(processName);
#else
  return memory::win::GetProcessIdentifier(processName);
#endif
}

memory::Module memory::GetModule(const char *moduleName, uint pid) {
#ifdef _LINUX
  return memory::lnx::GetModule(moduleName, pid);
#else
  return memory::win::GetModule(moduleName, pid);
#endif
}

addr memory::AllocateMemory(addr source, uint size) {
#ifdef _LINUX
  return memory::lnx::AllocateMemory(source, size);
#else
  return memory::win::AllocateMemory(source, size);
#endif
}

bool memory::FreeMemory(addr source) {
#ifdef _LINUX
  return memory::lnx::FreeMemory(source);
#else
  return memory::win::FreeMemory(source);
#endif
}

bool memory::PatchBytes(addr *destination, addr *source, uint size) {
#ifdef _LINUX
  return memory::lnx::PatchBytes(destination, source, size);
#else
  return memory::win::PatchBytes(destination, source, size);
#endif
}

bool memory::ReadBytes(addr *source, addr *destination, uint size) {
#ifdef _LINUX
  return memory::lnx::ReadBytes(source, destination, size);
#else
  return memory::win::ReadBytes(source, destination, size);
#endif
}

bool memory::NopBytes(addr *destination, uint size) {
#ifdef _LINUX
  return memory::lnx::NopBytes(destination, size);
#else
  return memory::win::NopBytes(destination, size);
#endif
}

addr memory::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
#ifdef _LINUX
  return memory::lnx::FindDMAAddress(baseAddress, offsets);
#else
  return memory::win::FindDMAAddress(baseAddress, offsets);
#endif
}
