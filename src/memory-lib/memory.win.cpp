#include "memory.win.h"

bool memory::win::InjectSharedLibrary(const char *processName,
                                      const char *pathToLibrary) {

  uint processIdentifier = 0;
  while (!processIdentifier) {
    processIdentifier = memory::lnx::GetProcessIdentifier(processName);
    SysWait(100);
  }

  HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processIdentifier);
  if (!process || process == INVALID_HANDLE_VALUE) {
    std::cout << "InjectDll(): Could not open process with identifier \""
              << process << "\"" << std::endl;
    return false;
  }

  uintptr_t *allocation = (uintptr_t *)VirtualAllocEx(
      process, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!allocation) {
    std::cout << "InjectDll(): Allocation of memory inside the targeted "
                 "process failed ..."
              << std::endl;
    return false;
  }

  WriteProcessMemory(process, allocation, pathToLibrary,
                     strlen(pathToLibrary) + 1, NULL);

  HANDLE thread = CreateRemoteThread(process, NULL, NULL,
                                     (LPTHREAD_START_ROUTINE)LoadLibraryA,
                                     allocation, NULL, NULL);
  if (!thread) {
    return false;
  }

  CloseHandle(thread);
  return true;
}

uint memory::win::GetProcessIdentifier(const char *processName) {
  HANDLE processList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 entry;
  entry.dwSize = sizeof(entry);

  if (!Process32First(processList, &entry)) {
    return 0;
  }

  do {
    if (!_stricmp(entry.szExeFile, processName)) {
      CloseHandle(processList);
      return entry.th32ProcessID;
    }
  } while (Process32Next(processList, &entry));

  return 0;
}

memory::Module memory::win::GetModule(const char *moduleName) {
  memory::Module module;
  module.StartAddress = (addr)GetModuleHandle(moduleName);

  MODULEINFO moduleInfo = {};
  GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBaseAddress,
                       &moduleInfo, sizeof(moduleInfo));

  if (!moduleInfo || !moduleInfo.SizeOfImage) {
    module.Size = 0;
  }
  module.Size = moduleInfo.SizeOfImage;
  return module;
}

memory::Module memory::win::GetModule(uint processIdentifier, const char *moduleName) {
  memory::Module module;
  module.PID = pid;
  module.StartAddress = (addr)GetModuleHandleEx(moduleName);

  MODULEINFO moduleInfo = {};
  GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBaseAddress,
                       &moduleInfo, sizeof(moduleInfo));

  if (!moduleInfo || !moduleInfo.SizeOfImage) {
    module.Size = 0;
  }
  module.Size = moduleInfo.SizeOfImage;
  return module;
}

addr memory::win::AllocateMemory(addr source, uint size) {
  return (addr)VirtualAlloc((addr *)source, size, MEM_COMMIT | MEM_RESERVE,
                            PAGE_EXECUTE_READWRITE);
}

bool memory::win::FreeMemory(addr source) {
  return VirtualFree((addr *)source, 0, MEM_RELEASE) == 0;
}

bool memory::win::PatchBytes(addr *destination, addr *source, uint size) {
  ulong protection;
  if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  if (memcpy_s((addr *)destination, size, source, size) != 0) {
    return false;
  }
  return VirtualProtect(destination, size, protection, &protection);
}

bool memory::win::ReadBytes(addr *source, addr *destination, uint size) {
  ulong protection;
  if (!VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  if (memcpy_s((addr *)destination, size, source, size) != 0) {
    return false;
  }
  return VirtualProtect(source, size, protection, &protection);
}

bool memory::win::NopBytes(addr *destination, uint size) {
  ulong protection;
  if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  memset(destination, MEMORY_ASM_INSTRUCTION_NOP, size);
  return VirtualProtect(destination, size, protection, &protection) != 0;
}

addr memory::win::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
  addr address = baseAddress;
  for (uint i = 0; i < offsets.size(); ++i) {
    address = *(addr *)address;

    if (!address) {
      return address;
    }

    address += offsets[i];
  }
  return address;
}
