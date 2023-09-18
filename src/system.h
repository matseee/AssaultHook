#pragma once
#include <stdint.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#if defined(_M_IX86) || defined(__i386__) || __WORDSIZE == 32
#define ARCH_86 /* The code is being compiled for 32 bits */
#elif defined(_M_X64) || defined(__LP64__) || defined(_LP64) || __WORDSIZE == 64
#define ARCH_64 /* The code is being compiled for 64 bits */
#endif

/* Helper macros */
#if defined(ARCH_86)
#define strtoptr(nptr, endptr, base) strtoul(nptr, endptr, base)
#elif defined(ARCH_64)
#define strtoptr(nptr, endptr, base) strtoull(nptr, endptr, base)
#endif

// active operating system
#define _LINUX 0x1
// #define _WINDOWS 0x1

// types for both operating systems
typedef uintptr_t	addr;	// pointer address
typedef unsigned char	byte;	// single byte
typedef unsigned int	uint;	// unsigned int
typedef unsigned long	ulong;	// unsigned long

// includes for any unix like system
#ifdef _LINUX
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <time.h>

inline void SysWait(uint milliseconds) {
    timespec sleepSpec{
	0,
	((long)milliseconds) * 1000 * 1000,
    };
    nanosleep(&sleepSpec, 0);
}
#endif

// includes for a windows system (win10, win11)
#ifdef _WINDOWS
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <gl\GL.h>

inline void SysWait(uint milliseconds) {
    Sleep(milliseconds);
}
#endif

