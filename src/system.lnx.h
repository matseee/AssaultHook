#pragma once
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <link.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "system.h"

inline void SysWait(uint milliseconds) {
  timespec sleepSpec{
      0,
      ((long)milliseconds) * 1000 * 1000,
  };
  nanosleep(&sleepSpec, 0);
}
