#include <filesystem>
#include <iostream>

#include "../system.h"
#include "../memory-lib/memory.h"

#ifdef _LINUX
const char *PROCESS = "linux_64_client";
const char *FILENAME = "AssaultHook.so";
#endif

#ifdef _WINDOWS
const char *PROCESS = "ac_client.exe";
const char *FILENAME = "AssaultHook.dll";
#endif

int main() {
  std::filesystem::path pathToLibrary =
      std::filesystem::current_path() / FILENAME;

  if (memory::InjectSharedLibrary(PROCESS, pathToLibrary.string().c_str())) {
    std::cout << "AssaultHook-Injection succesfully ..." << std::endl;
  } else {
    std::cout << "AssaultHook-Injection failed ..." << std::endl;
  }

  SysWait(10000);
  return 0;
}
