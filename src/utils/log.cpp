#include "log.h"

Log* Log::Instance = nullptr;

bool Log::IsActive;
FILE* Log::ConsoleStream;
std::ofstream Log::NullStream;

std::ostream& Log::Print(LogLevel level) {
	Log::CreateIfNotExist();
	
	if (Log::IsActive && Log::ConsoleStream) {
		return std::cout << Log::GetTimestamp() << " - " << Log::LogLevelToString(level) << " - ";
	}

	return Log::NullStream;
}

void Log::Bytes(byte* bytes, unsigned int length) {
	for (unsigned int i = 0; i < length; i++) {
		std::cout << hex(bytes[i]) << " ";
	}
	std::cout << " ..." << std::endl;
};

std::ostream& Log::Endl(std::ostream& ostream) { 
	return std::endl(ostream);
}

void Log::SetActive(bool active) {
	Log::CreateIfNotExist();

	if (Log::IsActive == active) {
		return;
	}

	Log::IsActive = active;
	if (Log::IsActive) {
		Log::AllocateConsole();
	} else {
		Log::DeallocateConsole();
	}
}

void Log::Destroy() {
	 delete Log::Instance;
}

void Log::CreateIfNotExist() {
	if (!Log::Instance) {
		Log::Instance = new Log();
	}
}

Log::Log() {
	Log::IsActive = false;
	Log::ConsoleStream = nullptr;
	Log::NullStream.open("nul", std::ofstream::out | std::ofstream::app);
}

Log::~Log() {
	Log::DeallocateConsole();
	Log::NullStream.close();
	delete Log::ConsoleStream;
}

void Log::AllocateConsole() {
	if (Log::ConsoleStream) {
		return;
	}

	AllocConsole();
	freopen_s(&Log::ConsoleStream, "CONOUT$", "w", stdout);
}

void Log::DeallocateConsole() {
	if (!Log::ConsoleStream) {
		return;
	}

	FreeConsole();
	fclose(Log::ConsoleStream);

	Log::ConsoleStream = nullptr;
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
