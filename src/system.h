#pragma once
#include <stdint.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

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

