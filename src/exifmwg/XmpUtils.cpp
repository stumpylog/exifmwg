
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

std::string trimWhitespace(const std::string &str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos) {
    return "";
  }
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

std::vector<std::string> splitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

std::string cleanXmpText(const std::string &xmpValue) {
  // Handle XMP localized text format: lang="x-default" Actual text content
  std::string cleaned = xmpValue;

  // Look for the pattern lang="x-default" or similar language tags
  size_t langPos = cleaned.find("lang=\"");
  if (langPos != std::string::npos) {
    // Find the end of the language attribute (closing quote + space)
    size_t quoteEnd =
        cleaned.find("\"", langPos + 6); // 6 = length of "lang=\""
    if (quoteEnd != std::string::npos) {
      // Skip past the quote and any following whitespace
      size_t textStart = quoteEnd + 1;
      while (textStart < cleaned.length() && std::isspace(cleaned[textStart])) {
        textStart++;
      }
      cleaned = cleaned.substr(textStart);
    }
  }

  return cleaned;
}

std::vector<std::string> parseDelimitedString(const Exiv2::XmpData &xmpData,
                                              const std::string &key,
                                              char delimiter) {
  std::vector<std::string> result;

  auto it = xmpData.findKey(Exiv2::XmpKey(key));
  if (it != xmpData.end()) {
    std::string value = it->toString();

    // Split on delimiter
    std::vector<std::string> tokens = splitString(value, delimiter);
    for (const auto &token : tokens) {
      std::string trimmed = trimWhitespace(token);
      if (!trimmed.empty()) {
        result.push_back(trimmed);
      }
    }
  }

  return result;
}

} // namespace XmpUtils
