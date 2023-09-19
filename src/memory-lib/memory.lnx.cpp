#include "memory.lnx.h"

uint memory::lnx::GetProcessIdentifier(const char *processName) {
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
}

memory::Module memory::lnx::GetModule(const char *moduleName, uint pid) {
  memory::Module module;
  module.PID = pid;

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
  return module;
}

addr memory::lnx::AllocateMemory(addr source, uint size) {
  return true;
}

bool memory::lnx::FreeMemory(addr source) {
  return true;
}

bool memory::lnx::PatchBytes(addr *destination, addr *source, uint size) {
  return true;
}

bool memory::lnx::ReadBytes(addr *source, addr *destination, uint size) {
  return true;
}

bool memory::lnx::NopBytes(addr *destination, uint size) {
  return true;
}

addr memory::lnx::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
  return 0;
}
