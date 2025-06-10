
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#include "XmpUtils.hpp"

namespace XmpUtils {
void clearXmpKey(Exiv2::XmpData &xmpData, const std::string &key) {
  auto it = xmpData.begin();
  while (it != xmpData.end()) {
    if (it->key().find(key) != std::string::npos) {
      it = xmpData.erase(it);
    } else {
      ++it;
    }
  }
}

std::string doubleToStringWithPrecision(double value, int precision) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(precision) << value;
  return oss.str();
}
} // namespace XmpUtils
