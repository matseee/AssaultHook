#pragma once
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <time.h>

#include "system.h"

inline void SysWait(uint milliseconds) {
  timespec sleepSpec{
      0,
      ((long)milliseconds) * 1000 * 1000,
  };
  nanosleep(&sleepSpec, 0);
}
