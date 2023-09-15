#pragma once
#include <stdint.h>

// active operating system
#define _LINUX 0x1

// types for both operating systems
typedef uintptr_t	addr;	// pointer address
typedef unsigned char	byte;	// single byte
typedef unsigned int	uint;	// unsigned int
typedef unsigned long	ulong;	// unsigned long

// includes for any unix like system
#ifdef _LINUX
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

inline void SysWait(uint milliseconds) {
   // CALL LINUX SLEEP FUNCTION
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

