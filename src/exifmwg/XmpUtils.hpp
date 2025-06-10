#pragma once
#include <exiv2/exiv2.hpp>
#include <string>
namespace XmpUtils {
void clearXmpKey(Exiv2::XmpData &xmpData, const std::string &key);
std::string doubleToStringWithPrecision(double value, int precision = 10);
} // namespace XmpUtils
