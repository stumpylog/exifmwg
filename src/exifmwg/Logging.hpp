#pragma once

#include <string>

// Logging functions
void log_to_python(const std::string& message);

#ifdef NDEBUG

#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARNING(msg)
#define LOG_ERROR(msg)

#else

#define LOG_DEBUG(msg) log_to_python(msg)
#define LOG_INFO(msg) log_to_python(msg)
#define LOG_WARNING(msg) log_to_python(msg)
#define LOG_ERROR(msg) log_to_python(msg)

#endif
