#pragma once
#include <Windows.h>
#include <iostream>

enum LogLevel {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

class Log
{
public:
	static Log* Get();
	static std::ostream& Print(LogLevel level);
	static std::ostream& Debug() { return Log::Print(LOG_DEBUG); };
	static std::ostream& Info() { return Log::Print(LOG_INFO); };
	static std::ostream& Warning() { return Log::Print(LOG_WARNING); };
	static std::ostream& Error() { return Log::Print(LOG_ERROR); };

	void SetActive(bool active);

protected:
	static Log* Instance;

	Log();

	void AllocateConsole();
	void DeallocateConsole();

	static std::string GetTimestamp();
	static std::string LogLevelToString(LogLevel l);

	bool isActive;
	bool isConsoleAllocated;

	FILE* fpConsoleOutput;
};