#pragma once
#include <Psapi.h>
#include <Windows.h>
#include <gl\GL.h>
#include <tlhelp32.h>

#include "system.h"

inline void SysWait(uint milliseconds) { Sleep(milliseconds); }
