#pragma once

#include <string>
#include <vector>

// Logging functions
void log_to_python(const std::string& level, const std::string& message);

#define LOG_DEBUG(msg) log_to_python("debug", msg)
#define LOG_INFO(msg) log_to_python("info", msg)
#define LOG_WARNING(msg) log_to_python("warning", msg)
#define LOG_ERROR(msg) log_to_python("error", msg)

// String utility functions
std::vector<std::string> split_string(const std::string& str, char delimiter);
std::string              trim(const std::string& str);
std::string              clean_xmp_text(const std::string& xmpValue);
std::string doubleToStringWithPrecision(double value, int precision = 10);
