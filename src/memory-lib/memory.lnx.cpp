#include "memory.lnx.h"

bool memory::lnx::InjectSharedLibrary(const char *processName,
                                      const char *pathToLibrary) {
  uint processIdentifier = 0;

  std::cout << "Searching for process " << processName << " .";
  while (!processIdentifier) {
    processIdentifier = memory::lnx::GetProcessIdentifier(processName);
    SysWait(100);
    std::cout << ".";
  }
  std::cout << std::endl;

  memory::Module mod = memory::GetModule(processName, processIdentifier);
  std::cout << "Found module (" << (void *)mod.StartAddress << "-"
            << (void *)mod.EndAddress << ") ..." << std::endl;

  return true;
}

bool InjectSyscall(uint processIdentifier, int syscall_n, void *arg0,
                   void *arg1, void *arg2, void *arg3, void *arg4, void *arg5) {
  void *ret = (void *)-1;
  int status;
  struct user_regs_struct old_regs, regs;
  void *injection_addr = (void *)-1;

  // This buffer is our payload, which will run a syscall properly on x86/x64
  unsigned char injection_buf[] = {
#if defined(ARCH_86) // 32 bits syscall
    0xcd,
    0x80,              // int80 (syscall)
#elif defined(ARCH_64) // 64 bits syscall
    0x0f,
    0x05, // syscall
#endif
    /* these nops are here because
     * we're going to write memory using
     * ptrace, and it always writes the size
     * of a word, which means we have to make
     * sure the buffer is long enough
     */
    0x90, // nop
    0x90, // nop
    0x90, // nop
    0x90, // nop
    0x90, // nop
    0x90  // nop
  };

  // As ptrace will always write a uintptr_t, let's make sure we're using proper
  // buffers
  uintptr_t old_data;
  uintptr_t injection_buffer;
  memcpy(&injection_buffer, injection_buf, sizeof(injection_buffer));

  // Attach to process using 'PTRACE_ATTACH'
  ptrace(PTRACE_ATTACH, processIdentifier, NULL, NULL);
  wait(&status);

  /* Get the current registers using 'PTRACE_GETREGS' so that
   * we can restore the execution later
   * and also modify the bytes of EIP/RIP
   */

  ptrace(PTRACE_GETREGS, processIdentifier, NULL, &old_regs);
  regs = old_regs;

  // Now, let's set up the registers that will be injected into the tracee

#if defined(ARCH_86)
  regs.eax = (uintptr_t)syscall_n;
  regs.ebx = (uintptr_t)arg0;
  regs.ecx = (uintptr_t)arg1;
  regs.edx = (uintptr_t)arg2;
  regs.esi = (uintptr_t)arg3;
  regs.edi = (uintptr_t)arg4;
  regs.ebp = (uintptr_t)arg5;
  injection_addr = (void *)regs.eip;
#elif defined(ARCH_64)
  regs.rax = (uintptr_t)syscall_n;
  regs.rdi = (uintptr_t)arg0;
  regs.rsi = (uintptr_t)arg1;
  regs.rdx = (uintptr_t)arg2;
  regs.r10 = (uintptr_t)arg3;
  regs.r8 = (uintptr_t)arg4;
  regs.r9 = (uintptr_t)arg5;
  injection_addr = (void *)regs.rip;
#endif

  // Let's store the buffer at EIP/RIP that we're going to modify into
  // 'old_data' using 'PTRACE_PEEKDATA'
  old_data = (uintptr_t)ptrace(PTRACE_PEEKDATA, processIdentifier,
                               injection_addr, NULL);

  // Let's write our payload into the EIP/RIP of the target process using
  // 'PTRACE_POKEDATA'
  ptrace(PTRACE_POKEDATA, processIdentifier, injection_addr, injection_buffer);

  // Let's inject our modified registers into the target process using
  // 'PTRACE_SETREGS'
  ptrace(PTRACE_SETREGS, processIdentifier, NULL, &regs);

  // Let's run a single step in the target process (execute one assembly
  // instruction)
  ptrace(PTRACE_SINGLESTEP, processIdentifier, NULL, NULL);
  waitpid(processIdentifier, &status,
          WSTOPPED); // Wait for the instruction to run

  // Let's get the registers after the syscall to store the return value
  ptrace(PTRACE_GETREGS, processIdentifier, NULL, &regs);
#if defined(ARCH_86)
  ret = (void *)regs.eax;
#elif defined(ARCH_64)
  ret = (void *)regs.rax;
#endif

  // Let's write the old data at EIP/RIP
  ptrace(PTRACE_POKEDATA, processIdentifier, (void *)injection_addr, old_data);

  // Let's restore the old registers to continue the normal execution
  ptrace(PTRACE_SETREGS, processIdentifier, NULL, &old_regs);
  ptrace(PTRACE_DETACH, processIdentifier, NULL,
         NULL); // Detach and continue the execution

  return ret;
}

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

memory::Module memory::lnx::GetModule(const char *moduleName) {
  return memory::lnx::GetModuleEx(getpid(), moduleName);
}

memory::Module memory::lnx::GetModuleEx(uint processIdentifier,
                                        const char *moduleName) {
  memory::Module module;
  module.PID = processIdentifier;

  long indexStart, indexEnd, tmpPos = 0;

  std::string moduleNameStr = std::string(moduleName);
  // std::cout << "moduleName: " << moduleNameStr << std::endl;

  // CREATE PROCESS-MAPS PATH AND READ IT
  std::stringstream mapsFilePath;
  mapsFilePath << "/proc/" << module.PID << "/maps";
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
  // TODO: Implement me
  return true;
}

addr memory::lnx::AllocateMemoryEx(uint processIdentifier, uint size,
                                   int protection) {
  // mmap template:
  // void *mmap (void *__addr, size_t __len, int __prot, int __flags, int __fd,
  // __off_t __offset);

  void *ret = (void *)-1;
#if defined(ARCH_86)
  ret = inject_syscall(pid,
                       //__NR_mmap has been deprecated for 32 bits a long time
                       // ago, so we're going to use __NR_mmap2
                       __NR_mmap2, // syscall number
                       // arguments
                       (void *)0, (void *)size, (void *)protection,
                       (void *)(MAP_ANON | MAP_PRIVATE), (void *)-1, (void *)0);
#elif defined(ARCH_64)
  ret = inject_syscall(pid,
                       __NR_mmap, // syscall number
                       // arguments
                       (void *)0, (void *)size, (void *)(uintptr_t)protection,
                       (void *)(MAP_ANON | MAP_PRIVATE), (void *)-1, (void *)0);
#endif

  return (addr)ret;
}

bool memory::lnx::FreeMemory(addr source) { return true; }

bool memory::lnx::FreeMemoryEx(uint processIdentifier, addr source, uint size) {
  // munmap template
  // int munmap (void *__addr, size_t __len);
  memory::lnx::InjectSyscall(processIdentifier, __NR_munmap, (void *)source,
                             (void *)&size, NULL, NULL, NULL, NULL);
  return true;
}

bool memory::lnx::ProtectMemory(addr source, uint size, int protection) {
  // TODO: Implement me
  return true;
}

bool memory::lnx::ProtectMemoryEx(uint processIdentifier, addr source,
                                  uint size, int protection) {
  // mprotect template
  // int mprotect (void *__addr, size_t __len, int __prot);
  return memory::lnx::InjectSyscall(
      processIdentifier, __NR_mprotect, (void *)source, (void *)&size,
      (void *)(uintptr_t)protection, NULL, NULL, NULL);
}

bool memory::lnx::PatchBytes(addr *destination, addr *source, uint size) {
  // TODO: Implement me
  return true;
}

bool memory::lnx::PatchBytesEx(uint processIdentifier, addr *destination,
                               addr *source, uint size) {
  struct iovec iosrc;
  struct iovec iodst;
  iodst.iov_base = destination;
  iodst.iov_len = size;
  iosrc.iov_base = source;
  iosrc.iov_len = size;
  process_vm_writev(processIdentifier, &iosrc, 1, &iodst, 1, 0);
  return true;
}

bool memory::lnx::ReadBytes(addr *source, addr *destination, uint size) {
  return true;
}

bool memory::lnx::ReadBytesEx(uint processIdentifier, addr *source,
                              addr *destination, uint size) {
  struct iovec iosrc;
  struct iovec iodst;
  iodst.iov_base = destination;
  iodst.iov_len = size;
  iosrc.iov_base = source;
  iosrc.iov_len = size;
  process_vm_readv(processIdentifier, &iodst, 1, &iosrc, 1, 0);
  return true;
}

bool memory::lnx::NopBytes(addr *destination, uint size) { return true; }

addr memory::lnx::FindDMAAddress(addr baseAddress, std::vector<uint> offsets) {
  return 0;
}

addr memory::lnx::InjectSyscall(uint processIdentifier, int syscall_n,
                                addr arg0, addr arg1, addr arg2, addr arg3,
                                addr arg4, addr arg5) {
  addr returnAddress = 0;
  int status;
  struct user_registers_struct oldRegisters, registers;
  addr injectionAddress = 0;

  unsigned char injectionBuffer[] = {
#ifdef ARCH_86
      0xcd, 0x80, // int80 syscall
#else
      0x0f, 0x05, // syscall
#endif
      0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

  addr oldDataAddress, injectionBufferAddress;

  memcpy(&injectionBufferAddress, injectionBuffer, sizeof(injectionBuffer));

  ptrace(PTRACE_ATTACH, processIdentifier, NULL, NULL);
  wait(&status);

  ptrace(PTRACE_GETREGS, processIdentifier, NULL, &oldRegisters);
  registers = oldRegisters;

#ifdef ARCH_86
  registers.eax = (addr)syscall_n;
  registers.ebx = (addr)arg0;
  registers.ecx = (addr)arg1;
  registers.edx = (addr)arg2;
  registers.esi = (addr)arg3;
  registers.edi = (addr)arg4;
  registers.ebp = (addr)arg5;
  injectionAddress = &registers.rip;
#else
  registers.rax = (addr)syscall_n;
  registers.rdi = (addr)arg0;
  registers.rsi = (addr)arg1;
  registers.rdx = (addr)arg2;
  registers.r10 = (addr)arg3;
  registers.r8 = (addr)arg4;
  registers.r9 = (addr)arg5;
  injectionAddress = &registers.rip;
#endif
}
