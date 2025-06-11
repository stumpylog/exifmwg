#pragma once

#include <string>

// Logging functions
void log_to_python(const std::string& message);

#define LOG_DEBUG(msg) log_to_python(msg)
#define LOG_INFO(msg) log_to_python(msg)
#define LOG_WARNING(msg) log_to_python(msg)
#define LOG_ERROR(msg) log_to_python(msg)
