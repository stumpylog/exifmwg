#pragma once

#include <exiv2/exiv2.hpp>

#include <string>

namespace XmpUtils {
void clearXmpKey(Exiv2::XmpData &xmpData, const std::string &key);

// Standard string utils
std::string doubleToStringWithPrecision(double value, int precision = 10);
std::string trimWhitespace(const std::string &str);
std::string cleanXmpText(const std::string &xmpValue);

// Parsing utils
std::vector<std::string> splitString(const std::string &str, char delimiter);
std::vector<std::string> parseDelimitedString(const Exiv2::XmpData &xmpData,
                                              const std::string &key,
                                              char delimiter);

} // namespace XmpUtils
