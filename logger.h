#pragma once

#define LOG_TRACE(Message, ...) (Logger::Trace(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_DEBUG(Message, ...) (Logger::Debug(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_INFO(Message, ...) (Logger::Info(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_WARN(Message, ...) (Logger::Warn(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_ERROR(Message, ...) (Logger::Error(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_FATAL(Message, ...) (Logger::Fatal(__LINE__, __FILE__, Message, __VA_ARGS__))

#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>

using namespace std;

enum class LogPriority {
	TracePriority, DebugPriority, InfoPriority, WarnPriority, ErrorPriority, FatalPriority
};

class Logger {
private:
	LogPriority priority = LogPriority::InfoPriority; // Default priority
	mutex log_mutex;
	const char* filepath = "log.txt";
	ofstream file;

	Logger() {}

	~Logger() {
		free_file();
	}

	static Logger &get_instance() {
		static Logger logger;
		return logger;
	}

	template<typename... Args>
	void log(int line_number, const char* source_file, const char* message_priority_str, LogPriority message_priority, const char *message, Args... args) {
		if (priority <= message_priority) {
			auto start = chrono::system_clock::now();
			auto legacyStart = chrono::system_clock::to_time_t(start);
			char tmBuff[26];
			ctime_s(tmBuff, sizeof(tmBuff), &legacyStart);

			lock_guard<mutex> lock(log_mutex);

			if (file) {
				for (int i = 0; i < 19; i++) { // Not printing year and line break
					file << tmBuff[i];
				}
				file << " ";
				file << message_priority_str;
				file << "\"" << message << " " << forward<Args...>(args...) << "\"";
				file << " on line " << line_number << " in " << source_file;
				file << endl;
			}
		}
	}

	void enable_file_output() {
		file.open(filepath);
	}

	void free_file() {
		file.close();
	}

public:
	static void SetPriority(LogPriority new_priority) {
		get_instance().priority = new_priority;
	}

	static void EnableFileOutput() {
		Logger &logger_instance = get_instance();
		logger_instance.filepath = "log.txt";
		logger_instance.enable_file_output();
	}

	template<typename... Args>
	static void Trace(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[TRACE] ", LogPriority::TracePriority, message, args...);
	}

	template<typename... Args>
	static void Debug(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[DEBUG] ", LogPriority::DebugPriority, message, args...);
	}

	template<typename... Args>
	static void Info(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[INFO] ", LogPriority::InfoPriority, message, args...);
	}

	template<typename... Args>
	static void Warn(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[WARN] ", LogPriority::WarnPriority, message, args...);
	}

	template<typename... Args>
	static void Error(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[ERROR] ", LogPriority::ErrorPriority, message, args...);
	}

	template<typename... Args>
	static void Fatal(int line, const char* source_file, const char* message, Args... args) {
		get_instance().log(line, source_file, "[FATAL] ", LogPriority::FatalPriority, message, args...);
	}
};