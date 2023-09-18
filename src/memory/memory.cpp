#include "memory.h"

uint memory::GetProcessIdentifier(const char *processName) {
#ifdef _LINUX
  uint pid = 0;
  DIR *pDirectory = opendir("/proc");
  if (!pDirectory) {
    return 0;
  }

  struct dirent *pDirEntry;
  while (pid == 0 && (pDirEntry = readdir(pDirectory))) {
    uint id = (uint)atoi(pDirEntry->d_name);
    if (id > 0) {
      std::stringstream cmdlinePath;
      cmdlinePath << "/proc/" << id << "/cmdline";

      std::ifstream cmdlineFS(cmdlinePath.str(), std::ios::binary);
      if (!cmdlineFS.is_open()) {
        continue;
      }

      std::stringstream cmdline;
      cmdline << cmdlineFS.rdbuf();
      if (cmdline.str().find(processName) != std::string::npos) {
        pid = id;
      }

      cmdlineFS.close();
    }
  }

  closedir(pDirectory);
  return pid;
#endif
#ifdef _WINDOWS
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
#endif
}

memory::Module memory::GetModule(const char *moduleName, uint pid) {
  memory::Module module;
  module.PID = pid;
#ifdef _LINUX
  long indexStart, indexEnd, tmpPos = 0;

  std::string moduleNameStr = std::string(moduleName);
  // std::cout << "moduleName: " << moduleNameStr << std::endl;

  // CREATE PROCESS-MAPS PATH AND READ IT
  std::stringstream mapsFilePath;
  mapsFilePath << "/proc/" << pid << "/maps";
  // std::cout << "mapsFilePath: " << mapsFilePath.str() << std::endl;

  std::ifstream mapsFileStream(mapsFilePath.str(), std::ios::binary);
  if (!mapsFileStream.is_open()) {
    return module;
  }

  std::stringstream mapsFile;
  mapsFile << mapsFileStream.rdbuf();

  // READ THE WHOLE MODULE LINE
  bool isExecuteable = false;
  long rowIndexStart, rowIndexEnd = 0;
  std::string moduleString;

  do {
    tmpPos = mapsFile.str().find(moduleNameStr, tmpPos);
    tmpPos = mapsFile.str().rfind('\n', tmpPos);

    if (tmpPos == std::string::npos) {
      rowIndexStart = 0;
    } else {
      rowIndexStart = tmpPos + 1; // position after \n
    }

    rowIndexEnd = mapsFile.str().find('\n', rowIndexStart);

    moduleString =
        mapsFile.str().substr(rowIndexStart, rowIndexEnd - rowIndexStart);

    isExecuteable = moduleString.find("r-xp") != std::string::npos;
    if (!isExecuteable) {
      tmpPos = rowIndexEnd + 1;
    }
  } while (!isExecuteable);

  // std::cout << "moduleString: " << moduleString << std::endl;

  // PARSE MODULE PATH
  std::string modulePath;
  indexStart = mapsFile.str().find('/', rowIndexStart);
  indexEnd = mapsFile.str().find('\n', rowIndexEnd);

  modulePath = mapsFile.str().substr(indexStart, indexEnd - indexStart);
  // std::cout << "modulePath: " << modulePath << std::endl;

  // PARSE MODULE STARTADDRESS
  std::string startAddress;
  indexStart = indexEnd = std::string::npos;
  indexStart = 0;
  indexEnd = moduleString.find('-');

  startAddress = moduleString.substr(indexStart, indexEnd - indexStart);
  // std::cout << "startAddress: " << startAddress << std::endl;

  // PARSE MODULE ENDADDRESS
  std::string endAddress;
  indexStart = indexEnd + 1;
  indexEnd = moduleString.find(' ', indexStart);

  endAddress = moduleString.substr(indexStart, indexEnd - indexStart);
  // std::cout << "endAddress: " << endAddress << std::endl;

  // CONVERT STRINGS
  module.StartAddress = (addr)strtoptr(startAddress.c_str(), 0, 16);
  module.EndAddress = (addr)strtoptr(endAddress.c_str(), 0, 16);
  module.Size = module.EndAddress - module.StartAddress;
#endif
#ifdef _WINDOWS
  module.StartAddress = (addr)GetModuleHandle(moduleName);
  MODULEINFO moduleInfo = {};
  GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBaseAddress,
                       &moduleInfo, sizeof(moduleInfo));

  if (!moduleInfo || !moduleInfo.SizeOfImage) {
    module.Size = 0;
  }
  module.Size = moduleInfo.SizeOfImage;
#endif
  return module;
}

addr memory::AllocateMemory(addr source, uint size) {
#ifdef _LINUX
  return true;
#endif
#ifdef _WINDOWS
  return (addr)VirtualAlloc((addr *)source, size, MEM_COMMIT | MEM_RESERVE,
                            PAGE_EXECUTE_READWRITE);
#endif
}

bool memory::FreeMemory(addr source) {
#ifdef _LINUX
  return true;
#endif
#ifdef _WINDOWS
  return VirtualFree((addr *)source, 0, MEM_RELEASE) == 0;
#endif
}

bool memory::PatchBytes(addr *destination, addr *source, uint size) {
#ifdef _LINUX
  return true;
#endif
#ifdef _WINDOWS
  ulong protection;
  if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  if (memcpy_s((addr *)destination, size, source, size) != 0) {
    return false;
  }
  return VirtualProtect(destination, size, protection, &protection);
#endif
}

bool memory::ReadBytes(addr *source, addr *destination, uint size) {
#ifdef _LINUX
  return true;
#endif
#ifdef _WINDOWS
  ulong protection;
  if (!VirtualProtect(source, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  if (memcpy_s((addr *)destination, size, source, size) != 0) {
    return false;
  }
  return VirtualProtect(source, size, protection, &protection);
#endif
}

bool memory::NopBytes(addr *destination, uint size) {
#ifdef _LINUX
  return true;
#endif
#ifdef _WINDOWS
  ulong protection;
  if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &protection)) {
    return false;
  }
  memset(destination, MEMORY_ASM_INSTRUCTION_NOP, size);
  return VirtualProtect(destination, size, protection, &protection) != 0;
#endif
}

addr memory::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
#ifdef _LINUX
  return 0;
#endif
#ifdef _WINDOWS
  addr address = baseAddress;
  for (uint i = 0; i < offsets.size(); ++i) {
    address = *(addr *)address;

    if (!address) {
      return address;
    }

    address += offsets[i];
  }
  return address;
#endif
}
