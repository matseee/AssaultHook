#pragma once
#include <Windows.h>

// type aliases - to be "independend" from windows 
// and be easily able to switch to linux
typedef uintptr_t		addr;	// pointer address
typedef unsigned char	byte;	// single byte
typedef unsigned int	uint;	// unsigned int
typedef unsigned long	ulong;	// unsigned long

// defined independing from arch
#define MEMORY_ASM_INSTRUCTION_NOP 0x90

// defines dependening of arch
#ifdef _WIN64
#define MEMORY_ASM_INSTRUCTION_JMP 0x08EB0000000215FF
#define MEMORY_HOOK_MINLENGTH	16
#else
#define MEMORY_ASM_INSTRUCTION_JMP 0xE9
#define MEMORY_HOOK_MINLENGTH	5
#endif

