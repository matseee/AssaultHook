#pragma once
#include <iostream>
#include <fstream>

#include "../system.h"
#include "hex.h"

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class Log {
public:
    static std::ostream& Print(LogLevel level);
    static std::ostream& Debug() { return Log::Print(LOG_DEBUG); };
    static std::ostream& Info() { return Log::Print(LOG_INFO); };
    static std::ostream& Warning() { return Log::Print(LOG_WARNING); };
    static std::ostream& Error() { return Log::Print(LOG_ERROR); };

    static std::ostream& Endl(std::ostream& ostream);

    static void Bytes(byte* bytes, unsigned int length);

    static void SetActive(bool active);
    static void Destroy();

protected:
    Log();
    ~Log();

    static void CreateIfNotExist();

    static void AllocateConsole();
    static void DeallocateConsole();

    static std::string GetTimestamp();
    static std::string LogLevelToString(LogLevel l);

    static Log* Instance;

    static bool IsActive;
    static FILE* ConsoleStream;
    static std::ofstream NullStream;
};
