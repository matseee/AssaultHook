#pragma once
#include <asm-generic/errno.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

#define BUILD_OS 0 // should be replaced by makefile

#define _OS_LINUX 0
#define _OS_WINDOWS 1

#if BUILD_OS == _OS_LINUX
#define _LINUX
#elif BUILD_OS == _OS_WINDOWS
#define _WINDOWS
#endif

// architecture
#if defined(_M_IX86) || defined(__i386__) || __WORDSIZE == 32
#define _ARCH_86 // the code is being compiled for 32 bits
#elif defined(_M_X64) || defined(__LP64__) || defined(_LP64) || __WORDSIZE == 64
#define _ARCH_64 // the code is being compiled for 64 bits
#endif

// other system includes
#ifdef _LINUX
#include "system.lnx.h"
#else // _WINDOWS
#include "system.win.h"
#endif

// helper macros
#if defined(_ARCH_86)
#define strtoptr(nptr, endptr, base) strtoul(nptr, endptr, base)
#elif defined(_ARCH_64)
#define strtoptr(nptr, endptr, base) strtoull(nptr, endptr, base)
#endif

// types for both operating systems
typedef uintptr_t addr;      // pointer address
typedef unsigned char byte;  // single byte
typedef unsigned int uint;   // unsigned int
typedef unsigned long ulong; // unsigned long
