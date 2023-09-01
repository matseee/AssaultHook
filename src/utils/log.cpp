#include "log.h"

Log* Log::Instance = nullptr;

Log* Log::Get() {
	if (!Log::Instance) {
		Log::Instance = new Log();
	}
	return Log::Instance;
}

std::ostream& Log::Print(LogLevel level) {
	return std::cout << Log::GetTimestamp() << " - " << Log::LogLevelToString(level) << " - ";
}

Log::Log() {
	this->isActive = false;
	this->isConsoleAllocated = false;
	this->fpConsoleOutput = nullptr;
}

void Log::SetActive(bool active) {
	if (this->isActive == active) {
		return;
	}

	this->isActive = active;
	if (this->isActive) {
		this->AllocateConsole();
	} else {
		this->DeallocateConsole();
	}
}

void Log::AllocateConsole() {
	if (this->isConsoleAllocated) {
		return;
	}

	if (!AllocConsole()) {
		return;
	}

	if (freopen_s(&this->fpConsoleOutput, "CONOUT$", "w", stdout)) {
		return;
	}

	this->isConsoleAllocated = true;
}

void Log::DeallocateConsole() {
	if (!this->isConsoleAllocated) {
		return;
	}

	if (fclose(this->fpConsoleOutput)) {
		return;
	}

	if (!FreeConsole()) {
		return;
	}

	this->isConsoleAllocated = false;
}

std::string Log::GetTimestamp() {
	char buffer[20];
	tm locale_now = {};

	std::time_t now = std::time(nullptr);
	localtime_s(&locale_now, &now);
	
	strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &locale_now);
	return std::string(buffer);
}

std::string Log::LogLevelToString(LogLevel l) {
	switch (l) {
	case LOG_DEBUG:
		return "DEBUG";
	case LOG_WARNING:
		return "WARN ";
	case LOG_ERROR:
		return "ERROR";
	default:
		return "INFO ";
	}
}
